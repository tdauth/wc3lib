/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef WC3LIB_BLP_BLP_HPP
#define WC3LIB_BLP_BLP_HPP

#include <boost/multi_array.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/variant.hpp>
#include <boost/foreach.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace blp
{

/**
 * \brief Provides uniformed access to Blizzard's texture formats "BLP0", "BLP1" and "BLP2".
 *
 * \note For image format conversion see \ref conversion.
 *
 * The class offers read and write functions for which several options can be specified like how many MIP maps should be read or written.
 * \ref Blp::MipMap stores the actual picture data. All MIP maps can be accessed using \ref Blp::mipMaps().
 * The first MIP map should always have the actual size of the texture and can be used in any code which should display the original image.
 * To change the number of MIP maps after reading the texture you can use \ref Blp::generateRequiredMipMaps().
 *
 * \ref Blp::format(), \ref Blp::compression() and \ref Blp::flags() return the texture's header information. Depending on the format it supports different compression types.
 * "BLP0" and "BLP1" images can have two different compression modes:
 * <ul>
 * <li>JPEG compression (JFIF)</li>
 * <li>Paletted compression</li>
 * </ul>
 *
 * "BLP2" introduced additional compression types:
 * <ul>
 * <li>\ref Blp::Compression::Uncompressed </li>
 * <li>\ref Blp::Compression::DirectXCompression </li>
 * </ul>
 *
 * "BLP" format allows you to save up to 16 MIP maps per file.
 * Specification says:
 * A full MIP map chain must be present. The last MIP map must be 1x1 (no larger).
 * If an image is 32x8 the MIP map chain must be 32x8, 16x4, 8x2, 4x1, 2x1, 1x1.
 * Sizes not of powers of 2 seems to work fine too, the same rules for MIP map
 * still applies. Ex: 24x17, 12x8 (rounded down), 6x4, 3x2, 1x1 (rounded down).
 *
 * Format "BLP0" isn't different from "BLP1".
 * "BLP0" and "BLP1" are used in games Warcraft III and Warcraft III: The Frozen Throne.
 * "BLP2" is used in World of Warcraft.
 *
 * \section conversion Conversion
 * If you want to convert BLP images into other formats (e. g. JPEG or PNG) you can either write your own converter or use wc3lib's BLP Qt plugin (\ref wc3lib::editor::BlpIOPlugin).
 */
class Blp : public Format
{
	public:
		/// Number of maximum MIP maps which can be hold by one BLP file.
		static const std::size_t maxMipMaps;
		/// Size of color palette (always the same) used by BLP files with compression \ref Blp::Compression::Paletted.
		static const std::size_t compressedPaletteSize;
		/// Default JPEG quality which reaches from 0 - 100.
		static const int defaultQuality;
		static const std::size_t defaultMipMaps;
		static const bool defaultSharedHeader;

		/**
		 * \brief One scaled version of the image.
		 *
		 * Imagine each mip map as single image containing several pixels with various colors (and alpha values).
		 * MIP map colors are stored in a map hashed by their coordinates for fast access.
		 * Palette index is also stored for providing paletted compression support when writing the BLP/mip map into output stream.
		 */
		class MipMap
		{
			public:
				/**
				 * \brief Color value of one single pixel of a MIP map.
				 */
				class Color
				{
					public:
						/**
						 * Internal constructor to construct an empty list for the MIP map.
						 */
						Color();

						/**
						 * Creates a color with the RGBA color value \p rgba.
						 * This should be used for non-paletted images.
						 */
						Color(color rgba);
						/**
						 * Creates a color with the color palette index \p paletteIndex but without any custom alpha value.
						 * The alpha value from the palette is taken.
						 */
						Color(byte paletteIndex);
						/**
						 * Creates a color with the color palette index \p paletteIndex and the custom alpha value \p alpha.
						 * The alpha value from the palette is ignored.
						 */
						Color(byte paletteIndex, byte alpha);

						~Color();

						bool hasRgba() const;
						void setRgba(color rgba);
						color rgba() const;
						bool hasAlpha() const;
						void setAlpha(byte alpha);
						byte alpha() const;
						bool hasPaletteIndex() const;
						/**
						 * Sets palette index to value \p paletteIndex.
						 * \copydoc Color::paletteIndex()
						 */
						void setPaletteIndex(byte paletteIndex);
						/**
						 * \ref Color::paletteIndex() is an optional value for paletted compression (\ref Blp::Compression::Paletted) which refers two the colors color value in palette.
						 */
						byte paletteIndex() const;

						/**
						 * Gets the color value from palette \p palette.
						 * Depending on whether the pixel has its own alpha value or not this uses the alpha value from the palette or not.
						 *
						 * \param palette Array with size \ref Blp::compressedPaletteSize.
						 *
						 * \return Returns the full RGBA color from the palette.
						 */
						color paletteColor(const color *palette) const;

					protected:
						/**
						 * For paletted compression the alpha value is optional.
						 */
						typedef boost::optional<byte> Alpha;
						/**
						 * Stores the index to the color palette and the alpha value.
						 */
						typedef std::pair<byte, Alpha> IndexAndAlpha;
						/// Stores either RGBA value or palette index + alpha.
						typedef boost::variant<color, IndexAndAlpha> Value;

						Value m_value;
				};

				/**
				 * \brief Each color is mapped by it's 2-dimensional coordinates on the image. So this is the hash/key type of color values on mip maps. First is width, second is height.
				 */
				typedef boost::multi_array<Color, 2> Colors;

				/**
				 * \note Resizes allocated color array!
				 * \ingroup setSize
				 */
				void setWidth(dword width);
				dword width() const;
				/**
				 * \ingroup setSize
				 */
				void setHeight(dword height);
				dword height() const;

				void setColor(dword width, dword height, Color color);

				/**
				 * Assigns MIP map color \p rgba at position (\p width | \p height) with alpha \p alpha and palette index \p paletteIndex.
				 */
				void setColorRgba(dword width, dword height, color rgba);
				void setColorAlpha(dword width, dword height, byte alpha);
				/**
				 * Assigns MIP map color index to the image's color palette.
				 */
				void setColorIndex(dword width, dword height, byte index);
				/**
				 * Colors are stored as 2-dimensional array where the first index is the color's coordinates are [width][height].
				 */
				const Colors& colors() const;
				Colors& colors();
				const Color& colorAt(dword width, dword height) const;
				Color& colorAt(dword width, dword height);
				byte paletteIndexAt(dword width, dword height) const;
				byte alphaAt(dword width, dword height) const;

				/**
				 * \note Allocates \ref colors() with size of \p width * \p height.
				 */
				MipMap(dword width, dword height);
				~MipMap();

			protected:
				dword m_width;
				dword m_height;
				Colors m_colors; //[mip map width * mip map height];
		};

		/// \todo Don't seem to have the right values!!!
		enum class Format : dword
		{
			Blp0 = (dword)'BLP0', /// Reign of Chaos
			Blp1 = (dword)'BLP1', /// Warcraft The Frozen Throne
			Blp2 = (dword)'BLP2' /// World of Warcraft
		};

		enum class Compression : dword
		{
			Jpeg = 0, /// JFIF!
			Paletted = 1, /// Uses color palette with up to 256 different colors (\ref Blp::compressedPaletteSize).
			Uncompressed = 2, /// BLP2 only.
			DirectXCompression = 3 /// BLP2 only.
		};

		enum class Flags : dword
		{
			NoAlpha = 0,
			Alpha1Bit = 1, /// BLP2 only.
			Alpha4Bit = 4,  /// BLP2 only (DXT3 only).
			Alpha = 8
		};

		enum class PictureType : dword
		{
			PalettedWithAlpha1 = 3,
			PalettedWithAlpha2 = 4,
			PalettedWithoutAlpha = 5
		};

		/**
		 * \return Returns auto-detected BLP format of buffer \p buffer with size \p bufferSize. Buffer can be larger than required (only first 4 bytes are checked).
		 * \throw Exception Is thrown if no format was detected.
		 */
		static Format format(const byte *buffer, const std::size_t bufferSize);
		/**
		 * \return Returns true if \p buffer with size of \p bufferSize contains any BLP magic number/format identifier. Otherwise it returns false.
		 */
		static bool hasFormat(const byte *buffer, const std::size_t bufferSize);

		Blp();
		virtual ~Blp();

		void setFormat(Format format);
		Format format() const;
		void setCompression(Compression compression);
		Compression compression() const;
		void setFlags(Flags flags);
		Flags flags() const;
		void setWidth(dword width);
		dword width() const;
		void setHeight(dword height);
		dword height() const;
		void setPictureType(PictureType pictureType);
		PictureType pictureType() const;
		void setPictureSubType(dword pictureSubType);
		dword pictureSubType() const;

		/**
		 * MIP maps are stored in a vector indicated by their corresponding MIP number which can be 0-15.
		 */
		typedef boost::ptr_vector<boost::nullable<MipMap> > MipMaps;

		const MipMaps& mipMaps() const;
		MipMaps& mipMaps();

		/**
		 * Clears all BLP data (deletes MIP maps and resets properties).
		 */
		void clear();
		/**
		 * Reads BLP texture from input stream \p istream and detects its format and necessary data automatically.
		 * Throws an exception of type \ref Exception if any error occured during read process.
		 * \param istream Input stream which is read from.
		 * \param mipMaps Number of MIP maps which should be read (1 - 16). 0 means actual number of existing MIP maps. If there is less MIP maps than this parameter specifies only all available will be read!
		 * \return Size of read bytes (\ref byte). Note that this value can be smaller than the BLP file since it seems that there are unnecessary 0 bytes in some BLP files.
		 */
		std::streamsize read(InputStream &istream, const std::size_t &mipMaps);

		virtual std::streamsize read(InputStream &istream) override { return read(istream, 0); }
		/**
		 * \param ostream The output stream in which the BLP file is written to.
		 * \param quality Quality for JPEG/JFIF compression (0 - 100). -1 or another invalid value means default (\ref Blp::defaultQuality).
		 * \param mipMaps Number of MIP maps which should be written (1 - 16). 0 means actual number of existing MIP maps.
		 * \param sharedHeader If this value is true, for the JPEG format a shared header will be written shared by all MIP maps. Usuall this makes the file smaller but the compression process slower as well.
		 *
		 * \return Returns the number of written bytes.
		 */
		std::streamsize write(OutputStream &ostream, int quality, std::size_t mipMaps, bool sharedHeader) const;
		virtual std::streamsize write(OutputStream &ostream) const override { return write(ostream, defaultQuality, defaultMipMaps, defaultSharedHeader); }
		virtual uint32_t version() const override;

		/**
		 * Generates empty MIP maps of the required number.
		 */
		int generateRequiredMipMaps();

		/**
		 * \return Returns true if compression type is \ref Compression::Paletted .
		 */
		bool hasPalette() const;

		typedef boost::scoped_array<color> ColorPtr;

		/**
		 * Palette of RGBA colors with fixed size of \ref Blp::compressedPaletteSize.
		 * Each pixel has an index for a corresponding color of the palette.
		 * This sets the maximum of colors to \ref Blp::compressedPaletteSize but reduces the memory usage of the image.
		 * \return Returns corresponding color palette with size \ref Blp::compressedPaletteSize. If palette doesn't exist it will be generated automatically.
		 * \sa hasPalette()
		 * \throw Exception Exception safe. Throws an exception if compression is not paletted.
		 *
		 * @{
		 */
		const ColorPtr& palette() const;
		ColorPtr& palette();
		/**
		 * @}
		 */

		static std::size_t requiredMipMaps(std::size_t width, std::size_t height);

	protected:
		dword mipMapWidth(std::size_t index) const;
		dword mipMapHeight(std::size_t index) const;

		// header
		Format m_format;
		Compression m_compression;		//0 - Uses JPEG compression
						//1 - Uses palettes (uncompressed)
		Flags m_flags;			//#8 - Uses alpha channel (?)
		dword m_width;
		dword m_height;
		PictureType m_pictureType;
		dword m_pictureSubType; //1 - ???
		MipMaps m_mipMaps;
		ColorPtr m_palette;
};

inline constexpr bool operator&(Blp::Flags x, Blp::Flags y)
{
	return static_cast<bool>(static_cast<dword>(x) & static_cast<dword>(y));
}

inline bool Blp::MipMap::Color::hasRgba() const
{
	return this->m_value.which() == 0;
}

inline void Blp::MipMap::Color::setRgba(color rgba)
{
	this->m_value = rgba;
}

inline color Blp::MipMap::Color::rgba() const
{
	return boost::get<color>(this->m_value);
}

inline bool Blp::MipMap::Color::hasAlpha() const
{
	return this->m_value.which() == 1 && boost::get<IndexAndAlpha>(m_value).second.is_initialized();
}

inline void Blp::MipMap::Color::setAlpha(byte alpha)
{
	boost::get<IndexAndAlpha>(m_value).second = alpha;
}

inline byte Blp::MipMap::Color::alpha() const
{
	return boost::get<IndexAndAlpha>(m_value).second.get();
}

inline bool Blp::MipMap::Color::hasPaletteIndex() const
{
	return this->m_value.which() == 1;
}

inline void Blp::MipMap::Color::setPaletteIndex(byte paletteIndex)
{
	boost::get<IndexAndAlpha>(m_value).first = paletteIndex;
}

inline byte Blp::MipMap::Color::paletteIndex() const
{
	return boost::get<IndexAndAlpha>(m_value).first;
}

inline color Blp::MipMap::Color::paletteColor(const color *palette) const
{
	if (this->hasAlpha())
	{
		return ((palette[paletteIndex()] << 8) & 0xFFFFFF00) | alpha();
	}

	return palette[paletteIndex()];
}

inline void Blp::MipMap::setWidth(dword width)
{
	this->m_width = width;
	this->m_colors.resize(boost::extents[this->width()][this->height()]);
}

inline dword Blp::MipMap::width() const
{
	return this->m_width;
}

inline void Blp::MipMap::setHeight(dword height)
{
	this->m_height = height;
	this->m_colors.resize(boost::extents[this->width()][this->height()]);
}

inline dword Blp::MipMap::height() const
{
	return this->m_height;
}

inline void Blp::MipMap::setColor(dword width, dword height, Blp::MipMap::Color color)
{
	this->m_colors[width][height] = color;
}

inline void Blp::MipMap::setColorRgba(dword width, dword height, color rgba)
{
	this->m_colors[width][height].setRgba(rgba);
}

inline void Blp::MipMap::setColorIndex(dword width, dword height, byte index)
{
	this->m_colors[width][height].setPaletteIndex(index);
}

inline void Blp::MipMap::setColorAlpha(dword width, dword height, byte alpha)
{
	this->m_colors[width][height].setAlpha(alpha);
}

inline const Blp::MipMap::Colors& Blp::MipMap::colors() const
{
	return this->m_colors;
}

inline Blp::MipMap::Colors& Blp::MipMap::colors()
{
	return this->m_colors;
}

inline const Blp::MipMap::Color& Blp::MipMap::colorAt(dword width, dword height) const
{
	return colors()[width][height];
}

inline Blp::MipMap::Color& Blp::MipMap::colorAt(dword width, dword height)
{
	return colors()[width][height];
}

inline blp::byte Blp::MipMap::paletteIndexAt(dword width, dword height) const
{
	return colors()[width][height].paletteIndex();
}

inline blp::byte Blp::MipMap::alphaAt(dword width, dword height) const
{
	return colors()[width][height].alpha();
}

inline void Blp::setFormat(Format format)
{
	this->m_format = format;
}

inline Blp::Format Blp::format() const
{
	return this->m_format;
}

inline void Blp::setCompression(Blp::Compression compression)
{
	this->m_compression = compression;
}

inline Blp::Compression Blp::compression() const
{
	return this->m_compression;
}

inline void Blp::setFlags(Blp::Flags flags)
{
	this->m_flags = flags;
}

inline Blp::Flags Blp::flags() const
{
	return this->m_flags;
}

inline void Blp::setWidth(dword width)
{
	this->m_width = width;
}

inline dword Blp::width() const
{
	return this->m_width;
}

inline void Blp::setHeight(dword height)
{
	this->m_height = height;
}

inline dword Blp::height() const
{
	return this->m_height;
}

inline void Blp::setPictureType(Blp::PictureType pictureType)
{
	this->m_pictureType = pictureType;
}

inline Blp::PictureType Blp::pictureType() const
{
	return this->m_pictureType;
}

inline void Blp::setPictureSubType(dword pictureSubType)
{
	this->m_pictureSubType = pictureSubType;
}

inline dword Blp::pictureSubType() const
{
	return this->m_pictureSubType;
}

inline const Blp::MipMaps& Blp::mipMaps() const
{
	return this->m_mipMaps;
}

inline Blp::MipMaps& Blp::mipMaps()
{
	return this->m_mipMaps;
}

inline bool Blp::hasPalette() const
{
	return compression() == Compression::Paletted;
}

}

}

#endif
