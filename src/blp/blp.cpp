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

#include "blp.hpp"

#include <jpeglib.h>

// NOTE doesn't work with libjpegturbo
//#if JPEG_LIB_VERSION_MAJOR < 8
//#error Version 8 of jpeglib is required because of memory buffer management.
//#endif

//static_assert(BITS_IN_JSAMPLE > sizeof(byte) * 8, "Too many bits in one single sample (one single pixel color channel): BITS_IN_JSAMPLE. BLP/wc3lib allows maximum sample size of sizeof(byte) * 8.");

namespace wc3lib
{

namespace blp
{

const std::size_t Blp::maxMipMaps = 16;
const std::size_t Blp::compressedPaletteSize = 256;
const int Blp::defaultQuality = 100;
const std::size_t Blp::defaultMipMaps = 0;

Blp::MipMap::Color::Color() : m_alpha(0)
{
}

Blp::MipMap::Color::~Color()
{
}

Blp::MipMap::Color::Color(color argb, byte alpha) : m_value(argb), m_alpha(alpha)
{
}

Blp::MipMap::Color::Color(byte paletteIndex, byte alpha) : m_value(paletteIndex), m_alpha(alpha)
{
}

Blp::MipMap::MipMap(dword width, dword height) : m_width(width), m_height(height), m_colors(boost::extents[width][height])
{
}

Blp::MipMap::~MipMap()
{
}

Blp::Format Blp::format(const byte *buffer, const std::size_t bufferSize)
{
	if (bufferSize < sizeof(dword))
		throw Exception(boost::format(_("Error while reading BLP file. BLP identifier is too short: %1%. Expected size of %2%.")) % bufferSize % sizeof(dword));

	/*
	static const dword identifier0 = Blp::Format::Blp0;
	static const dword identifier1 = Blp::Format::Blp1;
	static const dword identifier2 = Blp::Format::Blp2;
	*/

	if (memcmp(buffer, "BLP0", sizeof(dword)) == 0) /// TODO &identifier0
	{
		return Blp::Format::Blp0;
	}
	else if (memcmp(buffer, "BLP1", sizeof(dword)) == 0)
	{
		return Blp::Format::Blp1;
	}
	else if (memcmp(buffer, "BLP2", sizeof(dword)) == 0)
	{
		return Blp::Format::Blp2;
	}

	throw Exception(boost::format(_("Error while reading BLP file. Missing BLP identifier, got \"%1%\".")) % reinterpret_cast<const char*>(buffer));
}

bool Blp::hasFormat(const byte *buffer, const std::size_t bufferSize)
{
	if (bufferSize < sizeof(dword))
		return false;

	/*
	 * TODO FIXME
	static const dword identifier0 = Blp::Format::Blp0;
	static const dword identifier1 = Blp::Format::Blp1;
	static const dword identifier2 = Blp::Format::Blp2;
	*/

	if (memcmp(buffer, "BLP0", sizeof(dword)) == 0)
		return true;
	else if (memcmp(buffer, "BLP1", sizeof(dword)) == 0)
		return true;
	else if (memcmp(buffer, "BLP2", sizeof(dword)) == 0)
		return true;

	return false;
}

Blp::Blp()
{
	this->clear();
}

Blp::~Blp()
{
	this->clear();
}

void Blp::clear()
{
	this->m_format = Blp::Format::Blp0;
	this->m_compression = Blp::Compression::Paletted;
	this->m_flags = Blp::Flags::NoAlpha;
	this->m_width = 0;
	this->m_height = 0;
	this->m_pictureType = 5;
	this->m_pictureSubType = 0;

	this->mipMaps().clear();
	this->palette().reset();
}

namespace
{

/**
* \author PitzerMike, Jean-Francois Roy, Tamino Dauth
*/
std::size_t requiredMipMaps(std::size_t width, std::size_t height)
{
	std::size_t mips = 0;
	std::size_t value = std::min<int>(width, height);

	while (value > 0 && mips < Blp::maxMipMaps)
	{
		value /= 2;
		++mips;
	}

	return mips;
}

std::string jpegError(const std::string &message)
{
	return boost::str(boost::format(message) % (jpeg_std_error(0)->jpeg_message_table != 0 ? jpeg_std_error(0)->jpeg_message_table[jpeg_std_error(0)->last_jpeg_message] : _("No error")));
}

/**
 * Reads JPEG MIP map data from \p buffer of size \p bufferSize into \p mipMap.
 */
void readMipMapJpeg(Blp::MipMap &mipMap, byte *buffer, dword bufferSize)
{
	JSAMPARRAY scanlines = 0; // will be filled later
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	try
	{
		jpeg_mem_src(&cinfo, buffer, bufferSize);

		if (jpeg_read_header(&cinfo, true) != JPEG_HEADER_OK) // TODO segmentation fault, when it's not a JPEG file and doesn start with 0xff 0xd8!
		{
			throw Exception(jpegError(_("Did not find header. Error: %1%.")));
		}

		if (!jpeg_start_decompress(&cinfo))
		{
			throw Exception(jpegError(_("Could not start decompress. Error: %1%.")));
		}

		if (mipMap.width() != cinfo.image_width)
		{
			std::cerr << boost::format(_("Corrupted MIP map: Image width (%1%) is not equal to mip map width (%2%).")) % cinfo.image_width % mipMap.width() << std::endl;

			mipMap.setWidth(cinfo.image_width); // adjusting new width before reading
		}

		if (mipMap.height() != cinfo.image_height)
		{
			std::cerr << boost::format(_("Corrupted MIP map: Image height (%1%) is not equal to mip map height (%2%).")) % cinfo.image_height % mipMap.height() << std::endl;

			mipMap.setHeight(cinfo.image_height); // adjusting new height before reading
		}

		if (cinfo.out_color_space != JCS_RGB)
		{
			std::cerr << boost::format(_("Warning: Image color space (%1%) is not equal to RGB (%2%).")) % cinfo.out_color_space % JCS_RGB << std::endl;
		}

		/// \todo Get as much required scanlines as possible (highest divident) to increase speed. Actually it could be equal to the MIP maps height which will lead to reading the whole MIP map with one single \ref jpeg_read_scanlines call
		const JDIMENSION requiredScanlines = cinfo.output_height; // increase this value to read more scanlines in one step
		assert(requiredScanlines <= cinfo.output_height && requiredScanlines > 0);
		const JDIMENSION scanlineSize = cinfo.output_width * cinfo.output_components; // JSAMPLEs per row in output buffer
		scanlines = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, scanlineSize, requiredScanlines); // TODO allocate via boost scoped pointer please!

		// per scanline
		while (cinfo.output_scanline < cinfo.output_height)
		{
			const JDIMENSION currentScanline = cinfo.output_scanline;
			const JDIMENSION dimension = jpeg_read_scanlines(&cinfo, scanlines, requiredScanlines); // scanlinesSize

			if (dimension != requiredScanlines)
				std::cerr << boost::format(_("Number of scanned lines is not equal to %1%. It is %2%.")) % requiredScanlines % dimension << std::endl;

			for (dword height = 0; height < dimension; ++height)
			{
				dword width = 0;

				// cinfo.output_components should be 3 if RGB and 4 if RGBA
				for (int component = 0; component < scanlineSize; component += cinfo.output_components)
				{
					// store as ARGB (BLP)
					// TODO why is component 0 blue, component 1 green and component 2 red?
					// Red and Blue colors are swapped.
					// http://www.wc3c.net/showpost.php?p=1046264&postcount=2
					color argb = ((color)scanlines[height][component]) | ((color)scanlines[height][component + 1] << 8) | ((color)scanlines[height][component + 2] << 16);

					if (cinfo.output_components == 4) // we do have an alpha channel
						argb |= ((color)(0xFF - scanlines[height][component + 3]) << 24);

					mipMap.setColor(width, height + currentScanline, argb); /// \todo Get alpha?!
					++width;
				}
			}
		}
	}
	/*
	 * Clean up before throwing exception.
	 */
	catch (std::exception &exception)
	{
		// jpeg_abort_decompress is only used when cinfo has to be used again.
		jpeg_destroy_decompress(&cinfo); // discard object

		throw exception;
	}

	jpeg_finish_decompress(&cinfo);

	if (!cinfo.saw_JFIF_marker)
	{
		std::cerr << boost::format(_("Warning: Did not find JFIF marker. JFIF format is used by default!\nThis is the JFIF version of the image %1%.%2%")) % cinfo.JFIF_major_version % cinfo.JFIF_minor_version << std::endl;
	}

	jpeg_destroy_decompress(&cinfo);
}

/**
 * See section "Abbreviated datastreams and multiple images" in
 * https://www.opensource.apple.com/source/tcl/tcl-87/tcl_ext/tkimg/tkimg/libjpeg/libjpeg.doc
 * or
 * /usr/share/doc/libjpeg-turbo-1.3.0-r3/libjpeg.txt.bz2
 *
 * \param isFirst Is the first MIP map. If so it will contain more header information than others.
 * \param quality 0-100
 */
void writeMipMapJpeg(const Blp::MipMap &mipMap, unsigned char *&buffer, unsigned long &bufferSize, bool isFirst, unsigned long &headerSize, int quality)
{
	JSAMPARRAY scanlines = 0; // will be filled later

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	try
	{
		const JDIMENSION imageWidth = boost::numeric_cast<JDIMENSION>(mipMap.width());
		const JDIMENSION imageHeight = boost::numeric_cast<JDIMENSION>(mipMap.height());
		const int inputComponents = 3; // TODO ARGB, 4
		/// \todo Get as much required scanlines as possible (highest divident) to increase speed. Actually it could be equal to the MIP maps height which will lead to reading the whole MIP map with one single \ref jpeg_write_scanlines call
		const JDIMENSION requiredScanlines = imageHeight; // increase this value to read more scanlines in one step
		assert(requiredScanlines <= imageHeight && requiredScanlines > 0);
		const JDIMENSION scanlineSize = imageWidth * inputComponents; // JSAMPLEs per row in output buffer

		jpeg_mem_dest(&cinfo, &buffer, &bufferSize);
		cinfo.image_width = imageWidth;
		cinfo.image_height = imageHeight;
		cinfo.input_components = inputComponents;
		cinfo.in_color_space = JCS_RGB;

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, quality, false);

		// only write tables for the first MIP map, so the first MIP map contains all header data as well!
		if (isFirst)
		{
			jpeg_write_tables(&cinfo);
		}

		jpeg_start_compress(&cinfo, false);
		headerSize = bufferSize;
		std::cout << "Internal header size: " << headerSize << std::endl;
		// TODO also share JFIF header? cinfo.write_JFIF_header = writeData->isFirst;

		scanlines = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, scanlineSize, requiredScanlines);

		while (cinfo.next_scanline < cinfo.image_height)
		{
			std::cout << "next line is < image height" << std::endl;

			for (dword height = cinfo.next_scanline; height < requiredScanlines; ++height)
			{
				std::cout << "Height " << height << std::endl;
				dword width = 0;

				// cinfo.output_components should be 3 if RGB and 4 if RGBA
				for (int component = 0; component < scanlineSize; component += cinfo.input_components)
				{
					// TODO why is component 0 blue, component 1 green and component 2 red?
					// Red and Blue colors are swapped.
					// http://www.wc3c.net/showpost.php?p=1046264&postcount=2
					const color argb = mipMap.colorAt(width, height).argb();
					scanlines[height][component] = blue(argb);
					scanlines[height][component + 1] = green(argb);
					scanlines[height][component + 2] = red(argb);

					if (cinfo.input_components == 4) // we do have an alpha channel
					{
						scanlines[height][component + 3] = 0xFF - alpha(argb);
					}

					++width;
				}
			}

			const JDIMENSION dimension = jpeg_write_scanlines(&cinfo, scanlines, requiredScanlines);
			std::cout << "Written " << dimension << " scanlines " << " and we have size " << bufferSize << std::endl;

			if (dimension != requiredScanlines)
			{
				throw Exception(boost::format(_("Number of written scan lines is not equal to %1%. It is %2%.")) % requiredScanlines % dimension);
			}
		}

		// output buffer data for further treatment
		std::cout << "Free in buffer " << cinfo.dest->free_in_buffer << std::endl;
	}
	/*
	 * Cleanup before throwing exception.
	 */
	catch (std::exception &exception)
	{
		// jpeg_abort_compress is only used when cinfo has to be used again.
		jpeg_destroy_compress(&cinfo); // discard object

		throw exception;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
}

}

namespace
{

/**
 * \brief Used to store offset and size of a MIP map from header data of the BLP image for later usage.
 *
 * Instances of this struct are stored in an array to access them later for each MIP map by its index.
 * We need this separate struct since there is two different header structs: \ref BlpHeader and \ref BlpHeader2
 * which share the offset and size data but we want to access both for one single MIP map.
 */
struct MipMapHeader
{
	MipMapHeader() : offset(0), size(0)
	{
	}

	dword offset, size;
};

}

std::streamsize Blp::read(InputStream &istream,  const std::size_t &mipMaps)
{
	this->clear();
	// header
	dword identifier;
	std::streamsize size = 0;
	wc3lib::read(istream, identifier, size);
	this->m_format = format((byte*)&identifier, sizeof(identifier));

	/*
	 * Store offset and size of MIP maps in an array to access it by its index later.
	 */
	std::vector<MipMapHeader> mipMapHeaders(Blp::maxMipMaps);

	if (this->format() == Blp::Format::Blp0 || this->format() == Blp::Format::Blp1)
	{
		struct BlpHeader header;
		wc3lib::read(istream, header, size);
		this->m_compression = static_cast<Compression>(header.compression);
		this->m_flags = static_cast<Flags>(header.flags);
		this->m_width = header.width;
		this->m_height = header.height;
		this->m_pictureType = header.pictureType;
		this->m_pictureSubType = header.pictureSubType;

		for (std::size_t i = 0; i < Blp::maxMipMaps; ++i)
		{
			mipMapHeaders[i].offset = header.mipMapOffset[i],
			mipMapHeaders[i].size = header.mipMapSize[i];
		}
	}
	// BLP2
	else if (this->format() == Blp::Format::Blp2)
	{
		struct Blp2Header header;
		wc3lib::read(istream, header, size);

		if (header.type == 0)
			this->m_compression = Blp::Compression::Jpeg;
		else if (header.type == 1)
		{
			if (header.encoding == 1)
				this->m_compression = Blp::Compression::Uncompressed;
			else if (header.encoding == 2)
				this->m_compression = Blp::Compression::DirectXCompression;
		}

		this->m_flags = static_cast<Flags>(header.alphaDepth);
		// header.alphaEncoding
		/*
		TODO
		0: DXT1 alpha (0 or 1 bit alpha)
		1: DXT2/3 alpha (4 bit alpha)
		7: DXT4/5 alpha (interpolated alpha)
		*/

		// fill palette
		if (this->compression() == Blp::Compression::Uncompressed || this->compression() == Blp::Compression::DirectXCompression)
			//header.palette;
			;

		// TODO header.hasMipMaps

		for (std::size_t i = 0; i < Blp::maxMipMaps; ++i)
		{
			mipMapHeaders[i].offset = header.mipMapOffset[i],
			mipMapHeaders[i].size = header.mipMapSize[i];
		}
	}

	/*
	 * Calculate number of required MIP maps.
	 * If the user has specified a number of MIP maps to be read (mipMaps != 0)
	 * which is in range of require MIP maps it is used.
	 * The user defined number cannot be greater than the calculated number of MIP maps.
	 */
	const bool useCustomMipMapsCount = mipMaps != 0; // user has specified a number of MIP maps to be read
	std::size_t mipMapsCount = requiredMipMaps(this->m_width, this->m_height);

	if (useCustomMipMapsCount)
	{
		if (mipMaps < mipMapsCount)
		{
			mipMapsCount = mipMaps;
		}
		else if (mipMaps > mipMapsCount)
		{
			std::cerr << boost::format(_("Invalid specified number of MIP maps to be read: %1%. Maximum number is: %2%.")) % mipMaps % mipMapsCount << std::endl;
		}
	}

	/*
	 * Reading 0 MIP maps makes no sense but we still support it to avoid errors.
	 */
	if (mipMapsCount == 0)
	{
		std::cerr << _("Detected 0 MIP maps (too little).") << std::endl;
	}

	/*
	 * Allocate all MIP maps.
	 */
	std::cerr << "Reading " << mipMapsCount << " MIP maps." << std::endl;
	this->mipMaps().resize(mipMapsCount, 0);

	for (std::size_t i = 0; i < mipMapsCount; ++i)
	{
		this->mipMaps().replace(i, new MipMap(this->mipMapWidth(i), this->mipMapHeight(i)));
	}

	/*
	 * Read process depending on compression type.
	 */
	switch (this->compression())
	{
		case Blp::Compression::Jpeg:
		{
			if (BITS_IN_JSAMPLE > sizeof(byte) * 8)
			{
				throw Exception(boost::format(_("Too many bits in one single sample (one single pixel color channel): %1%. BLP/wc3lib allows maximum sample size of %2%.")) % BITS_IN_JSAMPLE % (sizeof(byte) * 8));
			}

			dword jpegHeaderSize = 0;
			wc3lib::read(istream, jpegHeaderSize, size);

			if (jpegHeaderSize != 624) // usual size of headers of Blizzard BLPs
			{
				std::cerr << boost::format(_("Warning: JPEG (JFIF) header size is not equal to 624 which is the usual size of Blizzard's JPEG compressed BLPs. It is %1%.")) % jpegHeaderSize << std::endl;
			}

			boost::scoped_array<byte> jpegHeader(new byte[jpegHeaderSize]);
			wc3lib::read(istream, jpegHeader[0], size, jpegHeaderSize * sizeof(byte));

			for (std::size_t i = 0; i < this->mipMaps().size(); ++i)
			{
				const dword mipMapOffset = mipMapHeaders[i].offset;
				const dword mipMapSize = mipMapHeaders[i].size;
				istream.seekg(mipMapOffset);
				/*
				 * Allocated buffer which uses JPEG header for each MIP map as well.
				 */
				const dword bufferSize = jpegHeaderSize + mipMapSize;
				boost::scoped_array<byte> buffer(new byte[jpegHeaderSize + mipMapSize]);
				memcpy(buffer.get(), jpegHeader.get(), jpegHeaderSize);


				// read mip map data starting at header offset, header has already been copied into buffer
				wc3lib::read(istream, buffer[jpegHeaderSize], size, boost::numeric_cast<std::streamsize>(mipMapSize));


				readMipMapJpeg(this->mipMaps()[i], buffer.get(), bufferSize);
			}

			break;
		}

		case Blp::Compression::Paletted:
		{
			/*
			 * Uncompressed 1 and 2 only use 256 different colors.
			 *
			 * Read color palette.
			 */
			m_palette.reset(new color[Blp::compressedPaletteSize]);
			wc3lib::read(istream, m_palette[0], size, Blp::compressedPaletteSize * sizeof(color));

			for (std::size_t i = 0; i < this->mipMaps().size(); ++i)
			{
				const dword mipMapOffset = mipMapHeaders[i].offset;

				/*
				 * Move to absolute offset and read color indices.
				 */
				istream.seekg(mipMapOffset);

				for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
				{
					for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
					{
						byte index = 0;
						wc3lib::read(istream, index, size);
						this->mipMaps()[i].setColorIndex(width, height, index);
					}
				}

				if (this->flags() & Blp::Flags::Alpha)
				{
					for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
					{
						for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
						{
							byte alpha = 0;
							wc3lib::read(istream, alpha, size);
							this->mipMaps()[i].setColorAlpha(width, height, alpha);
						}
					}
				}
			}

			break;
		}

		default:
		{
			std::cerr << boost::format(_("Unsupported compression mode: %1%.")) % static_cast<dword>(this->compression()) << std::endl;

			break;
		}
	}

	return size;
}

namespace
{

/**
 * Searches for specified marker \p marker in buffer \p buffer. When found it writes marker data of size \p markerSize (if \p variableMarkerSize is true it ignores \p markerSize and reads size after marker from buffer) into output stream \p ostream.
 * \param variableMarkerSize If this value is true you do not have to define \p markerSize since marker size is read immediately after the marker's indicating byte (size of \ref word).
 * \param markerSize Includes size bytes!
 * \param marker Marker's indicating byte value (e. g. 0xD8 for start of image).
 * \return Returns true if marker was found and data has been written into output stream (no stream checks!).
 * \throw Exception Throws an exception if there is not enough data in buffer.
 * <a href="https://secure.wikimedia.org/wikipedia/en/wiki/JPEG#Syntax_and_structure">Source</a>
 * \todo (from Wikipedia) Within the entropy-coded data, after any 0xFF byte, a 0x00 byte is inserted by the encoder before the next byte, so that there does not appear to be a marker where none is intended, preventing framing errors. Decoders must skip this 0x00 byte. This technique, called byte stuffing (see JPEG specification section F.1.2.3), is only applied to the entropy-coded data, not to marker payload data.
 */
bool writeJpegMarkerFromBufferToStream(Blp::OutputStream &ostream, std::streamsize &size, bool variableMarkerSize, word markerSize, const byte marker, const unsigned char *buffer, const std::size_t bufferSize)
{
	if (marker == 0x00)
		throw Exception(_("0x00 marker is invalid! This value must be safed for 0xFF bytes in encoded data!"));

	for (std::size_t i = 0; i < bufferSize; ++i)
	{
		if (buffer[i] == 0xFF)
		{
			std::size_t j = i + 1;

			if (j >= bufferSize)
				return false;

			if (buffer[j] != marker)
				continue;

			if (variableMarkerSize || markerSize > 0)
			{
				++j;

				if (j + 1 > bufferSize) // we need 2 bytes for marker size (which could be 0 as well)
					throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data to get its own data size.")) % hexValue(marker));

				if (variableMarkerSize)
				{
					//memcpy(&markerSize, &(buffer[j]), sizeof(markerSize));
					markerSize = (word(buffer[j]) << 8) + word(buffer[j + 1]); // stored as little endian
				}
			}

			// 0xFF + marker + marker size
			if (i + 2 + markerSize > bufferSize)
				throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data with marker size %2% and data size %3%, starting at %4%.")) % hexValue(marker) % markerSize % bufferSize % (i + 2));

			// marker size is 2 bytes long and includes its own size!
			wc3lib::write(ostream, buffer[i], size, markerSize + 2); // + sizeof 0xFF and marker

			return true;
		}
	}

	return false;
}

}

std::streamsize Blp::write(OutputStream &ostream, int quality, std::size_t mipMaps) const
{
	/*
	 * Check and fix input options.
	 */
	if (quality < -1)
	{
		std::cerr << boost::format(_("Invalid quality %1%. Minimum value is -1 which indicates using default quality.")) % quality << std::endl;
		quality = -1;
	}
	else if (quality > 100)
	{
		std::cerr << boost::format(_("Invalid quality %1%. Maximum value is 100 which indicates using best quality.")) % quality << std::endl;
		quality = 100;
	}

	if (mipMaps > Blp::maxMipMaps)
	{
		std::cerr << boost::format(_("Invalid MIP maps number %1%. Maximum value is %2%.")) % mipMaps % Blp::maxMipMaps << std::endl;
		mipMaps = Blp::maxMipMaps;
	}

	/*
	 * If MIP maps number is not specified (mipMaps == 0) use the current number.
	 * Otherwise use the user defined number.
	 */
	const std::size_t actualMipMaps = mipMaps == 0 ? this->mipMaps().size() : mipMaps;
	std::streamsize size = 0;

	// TODO doesn't work
	/*
	dword fm = this->format();
	std::cout << fm << std::endl;
	std::cout << (char*)&fm << std::endl;
	wc3lib::write(ostream, (dword)format(), size);
	*/
	switch (this->format())
	{
		case Blp::Format::Blp0:
			wc3lib::write(ostream, "BLP0", size, 4);

			break;

		case Blp::Format::Blp1:
			wc3lib::write(ostream, "BLP1", size, 4);

			break;

		case Blp::Format::Blp2:
			wc3lib::write(ostream, "BLP2", size, 4);

			break;
	}

	// Offset where MIP map offsets and sizes are written down at the end of the whole writing process.
	dword startOffset = 0;

	if (this->format() == Blp::Format::Blp0 || this->format() == Blp::Format::Blp1)
	{
		struct BlpHeader header;
		header.compression = (dword)(this->compression());
		header.flags = (dword)(this->flags());
		header.width = this->width();
		header.height = this->height();
		header.pictureType = this->pictureType();
		header.pictureSubType = this->pictureSubType();

		// offsets and sizes are assigned after writing mip maps (later, see below)
		for (std::size_t i = 0; i < Blp::maxMipMaps; ++i)
		{
			header.mipMapOffset[i] = 0;
			header.mipMapSize[i] = 0;
		}

		wc3lib::write(ostream, header, size);
		// assign MIP map header data offset
		/*
		 * dword identifier
		 * dword compression, flags, width, height, pictureType, pictureSubType;
		 */
		startOffset = 28;
	}
	// BLP2
	else if (this->format() == Blp::Format::Blp2)
	{
		struct Blp2Header header;

		if (this->compression() == Compression::Jpeg)
			header.type = 0;
		else
		{
			header.type = 1;

			if (this->compression() == Blp::Compression::Uncompressed)
			{
				header.encoding = 1;
			}
			else if (this->compression() == Blp::Compression::DirectXCompression)
			{
				header.encoding = 2;
			}
			else
			{
				throw Exception(boost::format(_("Unsupported compression for BLP2: %1%")) % static_cast<dword>(this->compression()));
			}
		}

		header.alphaDepth = (byte)(this->flags());
		// header.alphaEncoding
		/*
		TODO
		0: DXT1 alpha (0 or 1 bit alpha)
		1: DXT2/3 alpha (4 bit alpha)
		7: DXT4/5 alpha (interpolated alpha)
		*/
		header.alphaEncoding = 0;

		// fill palette
		if (this->compression() == Blp::Compression::Uncompressed || this->compression() == Blp::Compression::DirectXCompression)
		{
			memcpy(static_cast<void*>(header.palette), static_cast<const void*>(palette().get()), Blp::compressedPaletteSize);
		}

		header.hasMips = (actualMipMaps > 1);

		// offsets and sizes are assigned after writing mip maps (later, see below)
		for (std::size_t i = 0; i < Blp::maxMipMaps; ++i)
		{
			header.mipMapOffset[i] = 0;
			header.mipMapSize[i] = 0;
		}

		wc3lib::write(ostream, header, size);
		// assign MIP map header data offset
		/*
		 * dword identifier
		 * byte type;
		 * byte encoding;
		 * byte alphaDepth;
		 * byte alphaEncoding;
		 * byte hasMips;
		 * dword width, height; //height+7
		 */
		startOffset = 17;
	}

	std::vector<MipMapHeader> mipMapHeaders(actualMipMaps);

	switch (this->compression())
	{
		case Blp::Compression::Jpeg:
		{
			for (std::size_t i = 0; i < actualMipMaps; ++i)
			{
				unsigned char *tmpBuffer = 0;
				unsigned long bufferSize = 0;
				unsigned long headerSize = 0;

				writeMipMapJpeg(this->mipMaps()[i], tmpBuffer, bufferSize, i == 0, headerSize, quality);

				/*
				 * Make sure buffer is freed on leaving the scope to avoid memory leaks.
				 */
				boost::scoped_array<unsigned char> buffer;
				buffer.reset(tmpBuffer);

				if (i == 0)
				{
					std::cout << "Header size: " << headerSize << std::endl;
					wc3lib::write(ostream, boost::numeric_cast<dword>(headerSize), size);
				}

				/*
				 * Write JPEG header before first MIP map.
				 */
				/*
				if (i == 0)
				{
					// skip shared header size
					std::streampos headerPosition = ostream.tellp();
					ostream.seekp(sizeof(dword), std::ios::cur);
					std::streamsize headerSize = 0;

					// NOTE marker reference: https://secure.wikimedia.org/wikipedia/en/wiki/JPEG#Syntax_and_structure
					// TODO would be much faster not to write unnecessary marker data for other MIP maps than first but seems to be impossible with jpeglib. Unfortunately, MIP maps still need some parts of their headers including their size information!
					// TODO jpeg lib written buffer starts with 0x98 and doesn't contain 0xD8
					//if (!writeJpegMarker(ostream, headerSize, false, 0, 0xD8, &(writeData[0].data[0]), writeData[0].headerSize)) // image start
					//	throw Exception(_("Missing image start marker 0xD8."));

					wc3lib::write<word>(ostream, 0xD8FF, headerSize); // marker 0xD8FF swapped for little endian

					// start after image start to increase performance
					// TODO huffman table marker data size seems to be too large (marker is found and size is read - 2 bytes - which include its own size of 2).
					if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xC4, buffer.get(), bufferSize)) // huffman table
						throw Exception(_("Missing huffman table marker 0xC4."));

					// start after huffman table to increase performance
					if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xDB, buffer.get(), bufferSize)) // quantization table
						throw Exception(_("Missing quantization table marker 0xDB."));

					// TODO support APPn marker which should be equal for all MIP maps (meta data)
					// TODO Are markers really stored end to end as written down on Wikipedia? If not we cannot start after already read markers!
					// start after quantization table to increase performance
					writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xFE, buffer.get(), bufferSize); // image comment

					if (headerSize != 624) // usual size of headers of Blizzard BLPs
						std::cerr << boost::format(_("Warning: JPEG (JFIF) header size is not equal to 624 which is the usual size of Blizzard's JPEG compressed BLPs. It is %1%.")) % headerSize << std::endl;

					// write shared header size and jump back again
					writeByteCount(ostream, boost::numeric_cast<dword>(headerSize), headerPosition, size);
					size += headerSize;
				}
				*/

				mipMapHeaders[i].offset = boost::numeric_cast<dword>(OutputStream::off_type(ostream.tellp()));

				std::streamsize mipMapSize = 0;

				/*
				// write non-shared MIP map header data
				if (!writeJpegMarkerFromBufferToStream(ostream, mipMapSize, true, 0, 0xC0, buffer.get(), bufferSize)) // start of frame
				{
					throw Exception(boost::format(_("Missing start of frame marker 0xC0 for MIP map %1%.")) % i);
				}

				std::cout << "Written " << mipMapSize << " of start of frame" << std::endl;

				if (!writeJpegMarkerFromBufferToStream(ostream, mipMapSize, true, 0, 0xDA, buffer.get(), bufferSize)) // start of scan
				{
					throw Exception(boost::format(_("Missing start of scan marker 0xDA for MIP map %1%.")) % i);
				}
				*/

				wc3lib::write(ostream, buffer[0], size, bufferSize);
				mipMapHeaders[i].size = boost::numeric_cast<dword>(bufferSize) + boost::numeric_cast<dword>(mipMapSize);
			}

			/*
			*TODO seems to be optional
			if (!writeJpegMarker(ostream, size, false, 0, 0xD9, &(writeData[0].data[writeData[0].dataSize- 4]), 4))
				throw Exception(_("Missing end of image marker 0xD9 for MIP map 0/the whole image."));
			*/

			std::cout << "Written " << size << " bytes!" << std::endl;

			break;
		}

		case Blp::Compression::Paletted:
		{
			// write palette, palette has always size of Blp::compressedPaletteSize (remaining colors have value 0).
			if (format() != Blp::Format::Blp2)
			{
				wc3lib::write(ostream, palette()[0], size, Blp::compressedPaletteSize * sizeof(color));
			}

			// write mip maps
			for (std::size_t i = 0; i < actualMipMaps; ++i)
			{
				const dword mipMapOffset = boost::numeric_cast<dword>(OutputStream::off_type(ostream.tellp()));
				std::streamsize mipMapSize = 0;

				for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
				{
					for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
					{
						const byte index = this->mipMaps()[i].colorAt(width, height).paletteIndex();
						wc3lib::write(ostream, index, mipMapSize);
					}
				}

				if (this->flags() & Blp::Flags::Alpha)
				{
					for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
					{
						for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
						{
							const byte alpha = this->mipMaps()[i].colorAt(width, height).alpha();
							wc3lib::write(ostream, alpha, mipMapSize);
						}
					}
				}

				mipMapHeaders[i].offset = mipMapOffset;
				mipMapHeaders[i].size = boost::numeric_cast<dword>(mipMapSize);

				size += mipMapSize;
			}

			break;
		}

		default:
		{
			std::cerr << boost::format(_("Unknown compression mode: %1%.")) % (dword)this->compression() << std::endl;

			break;
		}
	}

	// write MIP map header data, jump to header
	std::streampos position = ostream.tellp();
	ostream.seekp(startOffset);

	for (std::size_t index = 0; index < actualMipMaps; ++index)
	{
		std::streamsize tmpSize = 0;
		wc3lib::write(ostream, mipMapHeaders[index].offset, tmpSize);

		// For BLP2 we would count size at twice since it has been in header already.
		if (format() != Blp::Format::Blp2)
		{
			size += tmpSize;
		}
	}

	// skip remaining offsets
	const std::size_t remainingMipMaps = Blp::maxMipMaps - this->mipMaps().size();
	ostream.seekp(remainingMipMaps * sizeof(dword), std::ios::cur);

	for (std::size_t index = 0; index < actualMipMaps; ++index)
	{
		std::streamsize tmpSize = 0;
		wc3lib::write(ostream, mipMapHeaders[index].size, tmpSize);

		// For BLP2 we would count size at twice since it has been in header already.
		if (format() != Blp::Format::Blp2)
		{
			size += tmpSize;
		}

	}

	// Add skipped MIP map header data size to written size. This data has been skipped at the beginning of writing process.
	if (format() != Blp::Format::Blp2 && remainingMipMaps > 0)
	{
		size += remainingMipMaps * sizeof(dword) * 2;
	}

	ostream.seekp(position); // jump back to the end of stream

	return size;
}

uint32_t Blp::version() const
{
	return (uint32_t)format();
}

int Blp::generateMipMaps(std::size_t number, bool regenerate)
{
	if (number > Blp::maxMipMaps)
		std::cerr << boost::format(_("MIP map number %1% is bigger than maximum %2%")) % number % Blp::maxMipMaps << std::endl;

	number = std::max<std::size_t>(number, 1);
	number = std::min<std::size_t>(number, Blp::maxMipMaps);

	if (regenerate)
		m_mipMaps.clear();

	if (number < mipMaps().size())
	{
		mipMaps().resize(mipMaps().size() - number, 0);

		return number - mipMaps().size();
	}
	else if (number > mipMaps().size())
	{
		dword width = this->width();
		dword height = this->height();
		const std::size_t oldSize = mipMaps().size();
		const int result = number - oldSize;
		mipMaps().resize(number, 0);

		for (std::size_t i = oldSize; i < number; ++i)
		{
			this->mipMaps().replace(i, new MipMap(width, height));
			/// @todo Generate new scaled index and alpha list.
			width /= 2;
			height /= 2;
		}

		return result;
	}

	return 0;
}

const Blp::ColorPtr& Blp::palette() const
{
	return const_cast<Blp*>(this)->palette();
}

Blp::ColorPtr& Blp::palette()
{
	if (!hasPalette())
		throw Exception(_("BLP has no palette."));

	return m_palette;
}

dword Blp::mipMapWidth(std::size_t index) const
{
	dword width = this->m_width;

	for (std::size_t i = 0; i < index; ++i)
		width /= 2;

	return width;
}

dword Blp::mipMapHeight(std::size_t index) const
{
	dword height = this->m_height;

	for (std::size_t i = 0; i < index; ++i)
		height /= 2;

	return height;
}

}

}
