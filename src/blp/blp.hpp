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

#include "platform.hpp"

namespace wc3lib
{

namespace blp
{

/**
 * \brief Provides access to the BLP format.
 * BLP is Blizzard Entertainment's texture format.
 * There are two different format specifications: BLP1 and BLP2.
 * Besides there is the BLP0 format which isn't different from BLP1.
 * BLP0 and BLP1 are used in games Warcraft 3 and Warcraft 3 The Frozen Throne.
 * BLP2 is used in World of Warcraft.
 * BLP0 and BLP1 images can have two different compression modes:
 * <ul>
 * <li>JPEG compression (JFIF)</li>
 * <li>Paletted compression</li>
 * </ul>
 * Additionally the BLP format allows you to save up to 16 mip maps per file.
 * A full mipmap chain must be present. The last mipmap must be 1x1 (no larger).
 * If an image is 32x8 the mipmap chain must be 32x8, 16x4, 8x2, 4x1, 2x1, 1x1.
 * Sizes not of powers of 2 seems to work fine too, the same rules for mipmaps
 * still applies. Ex: 24x17, 12x8 (rounded down), 6x4, 3x2, 1x1 (rounded down).
 * \note JFIF compression and decompression is realized by using library "jpeg" which is loaded dynamically (at runtime) by using class \ref LibraryLoader. Unfortunatelly some properties (structures for instance) cannot be loaded from shared object/DLL and therefore are compiled statically into wc3lib. Please make sure that you're having installed a compatible version of library "jpeg".
 *
 *
 * Little loading example:
 * \code
 * #include <wc3lib/core.hpp>
 * #include <wc3lib/blp.hpp>
 *
 * using namespace wc3lib::blp;
 * std::ifstream ifstream("test.blp", std::ifstream::binary | std::ifstream::in);
 * boost::scoped_ptr<Blp> blp(new Blp());
 * blp->read(ifstream);
 * std::cout << boost::format("We have %1% MIP maps here.") % blp->mipMaps().size() << std::endl;
 *
 * BOOST_FOREACH(const Blp::MipMapPtr mipMap, blp->mipMaps())
 * 	std::cout << boost::format("This mip map has height %1% and width %2%.") % mipMap->height() % mipMap->width() << std::endl;
 * {
 * 	for (dword width = 0; width < mipMap->width(); ++width)
 *	{
 *		for (dword height = 0; height < mipMap->height; ++height)
 * 			std::cout << boost::format("Color %1% at position (%2%|%3%)") % mipMap->colorAt(width, height).argb() % width % height << std::endl;
 * 	}
 * }
 *
 * std::cout << boost::format("Generated %1% MIP maps.") % blp->generateMipMaps() << std::endl;
 *
 *
 * \endcode
 * \section Conversion
 * If you want to convert BLP images into other formats (e. g. JPEG or PNG) you can either write your own converter or use wc3lib's BLP Qt plugin.
 */
class Blp : public Format
{
	public:
		/**
		 * Imagine each mip map as single image containing several pixels with various colors (and alpha values).
		 * MIP map colors are stored in a map hashed by their coordinates for fast access.
		 * Palette index is also stored for providing paletted compression support when writing the BLP/mip map into output stream.
		 * BLP supports ARGB colors.
		 */
		class MipMap
		{
			public:
				class Color
				{
					public:
						/**
						 * Compares color with color \p other.
						 * \return Returns true if ARGB and alpha values are equal. Otherwise it returns false.
						 */
						bool operator==(const class Color &other) const;
						/**
						 * Compares color with color \p other.
						 * \return Returns true if ARGB or alpha values are not equal. Otherwise it returns false.
						 */
						bool operator!=(const class Color &other) const;

						Color();
						~Color();

						void setArgb(color argb);
						color argb() const;
						void setAlpha(byte alpha);
						byte alpha() const;
						/**
						 * Sets palette index to value \p paletteIndex.
						 * \copydoc Color::paletteIndex()
						 */
						void setPaletteIndex(byte paletteIndex);
						/**
						 * \ref Color::paletteIndex() is an optional value for paletted compression (\ref Blp::Compression::Paletted) which refers two the colors color value in palette.
						 */
						byte paletteIndex() const;

					protected:
						friend class MipMap;

						Color(color argb, byte alpha, byte paletteIndex);

						color m_argb;
						byte m_alpha;
						byte m_paletteIndex; // only used for paletted compression
				};

				/**
				 * \brief Each color is mapped by it's 2-dimensional coordinates on the image. So this is the hash/key type of color values on mip maps. First is width, second is height.
				 */
				typedef boost::multi_array<Color, 2> Colors;

				dword width() const;
				dword height() const;

				/**
				 * Assigns MIP map color \p argb at position (\p width | \p height) with alpha \p alpha and palette index \p paletteIndex.
				 */
				void setColor(dword width, dword height, color argb, byte alpha = 0, byte paletteIndex = 0);
				void setColorAlpha(dword width, dword height, byte alpha);
				/**
				 * Colors are stored as 2-dimensional array where the first index is the color's coordinates are [width][height].
				 */
				const Colors& colors() const;
				Colors& colors();
				const Color& colorAt(dword width, dword height) const;
				Color& colorAt(dword width, dword height);

			protected:
				friend class Blp;
				friend class boost::shared_ptr<MipMap>; // for destruction by shared ptr
				template<class T>
				friend void boost::checked_delete(T*); // for destruction by shared ptr

				/**
				 * \note Allocates \ref colors() with size of \p width * \p height.
				 */
				MipMap(dword width, dword height);
				~MipMap();

				dword m_width;
				dword m_height;
				Colors m_colors; //[mip map width * mip map height];
		};

		/// Number of maximum MIP maps which can be hold by one BLP file.
		static const std::size_t maxMipMaps;
		/// Size of color palette (always the same) used by BLP files with compression \ref BLP::Compression::Paletted.
		static const std::size_t compressedPaletteSize;
		/// Default JPEG quality which reaches from 0 - 100.
		static const int defaultQuality;

		/// \todo Don't seem to have the right values!!!
		BOOST_SCOPED_ENUM_START(Format) //: dword
		{
			Blp0 = (dword)'BLP0', /// Reign of Chaos
			Blp1 = (dword)'BLP1', /// Warcraft The Frozen Throne
			Blp2 = (dword)'BLP2' /// World of Warcraft
		};
		BOOST_SCOPED_ENUM_END

		BOOST_SCOPED_ENUM_START(Compression) //: dword
		{
			Jpeg = 0, /// JFIF!
			Paletted = 1, /// Uses color palette with up to 256 different colors (\ref Blp::compressedPaletteSize).
			Uncompressed = 2, /// BLP2 only.
			DirectXCompression = 3 /// BLP2 only.
		};
		BOOST_SCOPED_ENUM_END

		BOOST_SCOPED_ENUM_START(Flags) //: dword
		{
			NoAlpha = 0,
			Alpha1Bit = 1, /// BLP2 only.
			Alpha4Bit = 4,  /// BLP2 only (DXT3 only).
			Alpha = 8
		};
		BOOST_SCOPED_ENUM_END

		typedef boost::shared_array<color> ColorPtr;

		/**
		 * \return Returns auto-detected BLP format of buffer \p buffer with size \p bufferSize. Buffer can be larger than required (only first 4 bytes are checked).
		 * \throw Exception Is thrown if no format was detected.
		 */
		static BOOST_SCOPED_ENUM(Format) format(const byte *buffer, const std::size_t bufferSize) throw (class Exception);
		/**
		 * \return Returns true if \p buffer with size of \p bufferSize contains any BLP magic number/format identifier. Otherwise it returns false.
		 */
		static bool hasFormat(const byte *buffer, const std::size_t bufferSize);

		Blp();
		~Blp();

		void setFormat(BOOST_SCOPED_ENUM(Format) format);
		BOOST_SCOPED_ENUM(Format) format() const;
		void setCompression(BOOST_SCOPED_ENUM(Compression) compression);
		BOOST_SCOPED_ENUM(Compression) compression() const;
		void setFlags(BOOST_SCOPED_ENUM(Flags) flags);
		BOOST_SCOPED_ENUM(Flags) flags() const;
		void setWidth(dword width);
		dword width() const;
		void setHeight(dword height);
		dword height() const;
		void setPictureType(dword pictureType);
		dword pictureType() const;
		void setPictureSubType(dword pictureSubType);
		dword pictureSubType() const;

		typedef boost::shared_ptr<MipMap> MipMapPtr;
		/**
		 * MIP maps are stored in a vector indicated by their corresponding MIP number which can be 0-15.
		 */
		typedef std::vector<MipMapPtr> MipMaps;

		const MipMaps& mipMaps() const;
		MipMaps& mipMaps();

		/**
		 * Clears all BLP data (deletes MIP maps and resets properties).
		 */
		void clear();
		/**
		 * Reads BLP texture from input stream \p istream and detects its format and necessary data automatically.
		 * Throws an exception of type \ref Exception if any error occured during read process.
		 * \note JFIF decompression is implemented by using exactly one thread for each MIP map. First buffer is filled with required compressed MIP map data from stream and after that a thread is started which decompresses the buffer's data.
		 * \todo Add multithreading support for MIP maps of paletted compression (JPEG is already implemented).
		 * \param istream Input stream which is read from.
		 * \param mipMaps Number of MIP maps which should be read (1 - 16). 0 means actual number of existing MIP maps. If there is less MIP maps than this parameter specifies only all available will be read!
		 * \return Size of read bytes (\ref byte). Note that this value can be smaller than the BLP file since it seems that there are unnecessary 0 bytes in some BLP files.
		 */
		std::streamsize read(InputStream &istream, const std::size_t &mipMaps) throw (class Exception);

		std::streamsize read(InputStream &istream) throw (class Exception) { read(istream, 0); }
		/**
		 * \param quality Quality for JPEG/JFIF compression (0 - 100). -1 or another invalid value means default (\ref Blp::defaultQuality).
		 * \param mipMaps Number of MIP maps which should be written (1 - 16). 0 means actual number of existing MIP maps.
		 */
		std::streamsize write(OutputStream &ostream, const int &quality, const std::size_t &mipMaps) const throw (class Exception);
		std::streamsize write(OutputStream &ostream) const throw (class Exception) { write(ostream, -1, 0); }
		virtual uint32_t version() const;

		/**
		 * Assigns exactly \p number MIP maps to the BLP image.
		 * \param number Has to be at least 1 and less than \ref Blp::maxMipMaps.
		 * \param regenerate If this value is false function only compares current MIP map number to \p number and if they are not equal it generates or removes necessary or unnecessary MIP maps. Otherwise it clears all MIP maps and truly regenerates them.
		 * \return Returns number of truly newly created MIP maps (negative if some has been removed and 0 if it remains like it was before).
		 * \sa Blp::mipMaps()
		 */
		int generateMipMaps(std::size_t number = Blp::maxMipMaps, bool regenerate = false) throw (class Exception);

		bool hasPalette() const;
		/**
		 * Generates or regenerates color palette by color entry indices.
		 * \param number If this value is 0 or too large it will be set to \ref Blp::compressedPaletteSize automatically.
		 * \throw Exception Exception safe. Throws an exception if compression is not paletted.
		 * \sa hasPalette(), palette()
		 */
		ColorPtr generatePalette(std::size_t number = Blp::compressedPaletteSize) throw (Exception);
		/**
		 * \return Returns corresponding color palette with size \ref Blp::compressedPaletteSize. If palette doesn't exist it will be generated automatically.
		 * \sa hasPalette(), generatePalette()
		 * \throw Exception Exception safe. Throws an exception if compression is not paletted.
		 */
		const ColorPtr& palette() const throw (Exception);
		ColorPtr& palette() throw (Exception);

	protected:
		dword mipMapWidth(std::size_t index) const;
		dword mipMapHeight(std::size_t index) const;

		// header
		BOOST_SCOPED_ENUM(Format) m_format;
		BOOST_SCOPED_ENUM(Compression) m_compression;		//0 - Uses JPEG compression
						//1 - Uses palettes (uncompressed)
		BOOST_SCOPED_ENUM(Flags) m_flags;			//#8 - Uses alpha channel (?)
		dword m_width;
		dword m_height;
		dword m_pictureType;		//3 - Uncompressed index list + alpha list
						//4 - Uncompressed index list + alpha list
						//5 - Uncompressed index list
		dword m_pictureSubType;		//1 - ???
		MipMaps m_mipMaps;
		ColorPtr m_palette;
};

inline void Blp::MipMap::Color::setArgb(color argb)
{
	this->m_argb = argb;
}

inline color Blp::MipMap::Color::argb() const
{
	return this->m_argb;
}

inline void Blp::MipMap::Color::setAlpha(byte alpha)
{
	this->m_alpha = alpha;
}

inline byte Blp::MipMap::Color::alpha() const
{
	return this->m_alpha;
}

inline void Blp::MipMap::Color::setPaletteIndex(byte paletteIndex)
{
	this->m_paletteIndex = paletteIndex;
}

inline byte Blp::MipMap::Color::paletteIndex() const
{
	return this->m_paletteIndex;
}

inline dword Blp::MipMap::width() const
{
	return this->m_width;
}

inline dword Blp::MipMap::height() const
{
	return this->m_height;
}

inline void Blp::MipMap::setColor(dword width, dword height, color argb, byte alpha, byte paletteIndex)
{
	this->m_colors[width][height] = Color(argb, alpha, paletteIndex);
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

inline void Blp::setFormat(BOOST_SCOPED_ENUM(Format) format)
{
	this->m_format = format;
}

inline BOOST_SCOPED_ENUM(Blp::Format) Blp::format() const
{
	return this->m_format;
}

inline void Blp::setCompression(BOOST_SCOPED_ENUM(Blp::Compression) compression)
{
	this->m_compression = compression;
}

inline BOOST_SCOPED_ENUM(Blp::Compression) Blp::compression() const
{
	return this->m_compression;
}

inline void Blp::setFlags(BOOST_SCOPED_ENUM(Blp::Flags) flags)
{
	this->m_flags = flags;
}

inline BOOST_SCOPED_ENUM(Blp::Flags) Blp::flags() const
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

inline void Blp::setPictureType(dword pictureType)
{
	this->m_pictureType = pictureType;
}

inline dword Blp::pictureType() const
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
