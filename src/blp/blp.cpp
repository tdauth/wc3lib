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

#include <boost/thread.hpp>

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
const bool Blp::defaultThreads = true;

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

BOOST_SCOPED_ENUM(Blp::Format) Blp::format(const byte *buffer, const std::size_t bufferSize) throw (Exception)
{
	if (bufferSize < sizeof(dword))
		throw Exception(boost::format(_("Error while reading BLP file. BLP identifier is too short: %1%. Expected size of %2%.")) % bufferSize % sizeof(dword));

	/*
	static const dword identifier0 = Blp::Format::Blp0;
	static const dword identifier1 = Blp::Format::Blp1;
	static const dword identifier2 = Blp::Format::Blp2;
	*/

	if (memcmp(buffer, "BLP0", sizeof(dword)) == 0) /// TODO &identifier0
		return Blp::Format::Blp0;
	else if (memcmp(buffer, "BLP1", sizeof(dword)) == 0)
		return Blp::Format::Blp1;
	else if (memcmp(buffer, "BLP2", sizeof(dword)) == 0)
		return Blp::Format::Blp2;

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

struct ReadData
{
	ReadData(Blp::MipMap &mipMap, unsigned char * const data, const std::size_t dataSize) : mipMap(mipMap), data(data), dataSize(dataSize), finished(false)
	{
	}

	Blp::MipMap &mipMap;
	unsigned char * const data; /// All required data from stream which has size \ref dataSize (does contain JPEG header as well). Note that this buffer has to be deleted/freed by the \ref readMipMapJpeg function! Usually this happens when decompressor is being destroyed. Therefore it cannot be a \ref boost::scoped_ptr since it wouldn't be freed by delete operator.
	const std::size_t dataSize;
	bool finished;
	std::string stateMessage;
};

struct WriteData
{
	WriteData(const Blp::MipMap &mipMap, bool isFirst, int quality) : mipMap(mipMap), dataSize(0), isFirst(isFirst), quality(quality), headerSize(0), finished(false)
	{
	}

	const Blp::MipMap &mipMap;
	boost::scoped_array<unsigned char> data; /// All required data from stream which has size \ref dataSize (does contain JPEG header as well). \todo Should be a scoped_array, check thread deletion bug!
	std::size_t dataSize;
	bool isFirst;
	int quality; /// Reaches from 0-100.
	std::size_t headerSize; /// Only useful if isFirst is true (header size in buffer \ref data).
	bool finished;
	std::string stateMessage;
};

/**
 * Function for multithreading. Has to be thread-safe!
 */
void readMipMapJpeg(ReadData *readData)
{
	JSAMPARRAY scanlines = 0; // will be filled later
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	try
	{
		jpeg_mem_src(&cinfo, readData->data, readData->dataSize);

		if (jpeg_read_header(&cinfo, true) != JPEG_HEADER_OK) // TODO segmentation fault, when it's not a JPEG file and doesn start with 0xff 0xd8!
			throw Exception(jpegError(_("Did not find header. Error: %1%.")));

		if (!jpeg_start_decompress(&cinfo))
			throw Exception(jpegError(_("Could not start decompress. Error: %1%.")));

		if (readData->mipMap.width() != cinfo.image_width)
			throw Exception(boost::format(_("Image width (%1%) is not equal to mip map width (%2%).")) % cinfo.image_width % readData->mipMap.width());

		if (readData->mipMap.height() != cinfo.image_height)
			throw Exception(boost::format(_("Warning: Image height (%1%) is not equal to mip map height (%2%).")) % cinfo.image_height % readData->mipMap.height());

		if (cinfo.out_color_space != JCS_RGB)
			std::cerr << boost::format(_("Warning: Image color space (%1%) is not equal to RGB (%2%).")) % cinfo.out_color_space % JCS_RGB << std::endl;

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

					readData->mipMap.setColor(width, height + currentScanline, argb); /// \todo Get alpha?!
					++width;
				}
			}
		}
	}
	catch (std::exception &exception)
	{
		// jpeg_abort_decompress is only used when cinfo has to be used again.
		jpeg_destroy_decompress(&cinfo); // discard object
		readData->stateMessage = exception.what();

		return;
	}
	catch (...)
	{
		// jpeg_abort_decompress is only used when cinfo has to be used again.
		jpeg_destroy_decompress(&cinfo); // discard object

		return;
	}

	jpeg_finish_decompress(&cinfo);

	if (!cinfo.saw_JFIF_marker)
		std::cerr << boost::format(_("Warning: Did not find JFIF marker. JFIF format is used by default!\nThis is the JFIF version of the image %1%.%2%")) % cinfo.JFIF_major_version % cinfo.JFIF_minor_version << std::endl;

	jpeg_destroy_decompress(&cinfo);
	readData->finished = true;
}

/**
 * Function for multithreading. Has to be thread-safe!
 */
void writeMipMapJpeg(WriteData *writeData)
{
	JSAMPARRAY scanlines = 0; // will be filled later

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	try
	{
		const JDIMENSION imageWidth = boost::numeric_cast<JDIMENSION>(writeData->mipMap.width());
		const JDIMENSION imageHeight = boost::numeric_cast<JDIMENSION>(writeData->mipMap.height());
		const int inputComponents = 3; // TODO ARGB, 4
		/// \todo Get as much required scanlines as possible (highest divident) to increase speed. Actually it could be equal to the MIP maps height which will lead to reading the whole MIP map with one single \ref jpeg_write_scanlines call
		const JDIMENSION requiredScanlines = imageHeight; // increase this value to read more scanlines in one step
		assert(requiredScanlines <= imageHeight && requiredScanlines > 0);
		const JDIMENSION scanlineSize = imageWidth * inputComponents; // JSAMPLEs per row in output buffer

		unsigned char *data = 0;
		unsigned long size = requiredScanlines * scanlineSize + 1024; // size of uncompressed scanlines + estimated header size, TODO jpeglib uses its own buffer size 4096 instead
		jpeg_mem_dest(&cinfo, &data, &size);
		cinfo.image_width = imageWidth;
		cinfo.image_height = imageHeight;
		cinfo.input_components = inputComponents;
		cinfo.in_color_space = JCS_RGB;
		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, writeData->quality, false);
		jpeg_start_compress(&cinfo, writeData->isFirst); // only write tables for the first MIP map
		// TODO also share JFIF header? cinfo.write_JFIF_header = writeData->isFirst;

		// get header size
		writeData->headerSize = boost::numeric_cast<std::size_t>(size - cinfo.dest->free_in_buffer);

		scanlines = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, scanlineSize, requiredScanlines);

		while (cinfo.next_scanline < cinfo.image_height)
		{
			for (dword height = cinfo.next_scanline; height < requiredScanlines; ++height)
			{
				dword width = 0;

				// cinfo.output_components should be 3 if RGB and 4 if RGBA
				for (int component = 0; component < scanlineSize; component += cinfo.input_components)
				{
					// TODO why is component 0 blue, component 1 green and component 2 red?
					// Red and Blue colors are swapped.
					// http://www.wc3c.net/showpost.php?p=1046264&postcount=2
					const color argb = writeData->mipMap.colorAt(width, height).argb();
					scanlines[height][component] = blue(argb);
					scanlines[height][component + 1] = green(argb);
					scanlines[height][component + 2] = red(argb);

					if (cinfo.input_components == 4) // we do have an alpha channel
						scanlines[height][component + 3] = 0xFF - alpha(argb);

					++width;
				}
			}

			const JDIMENSION dimension = jpeg_write_scanlines(&cinfo, scanlines, requiredScanlines);

			if (dimension != requiredScanlines)
				throw Exception(boost::format(_("Number of written scan lines is not equal to %1%. It is %2%.")) % requiredScanlines % dimension);
		}

		// output buffer data for further treatment
		writeData->dataSize = boost::numeric_cast<std::size_t>(size - cinfo.dest->free_in_buffer); // TODO free_in_buffer is larger than size
		writeData->data.reset(data);
		//writeData->data.reset(new unsigned char[writeData->dataSize]);
		//memcpy(writeData->data.get(), data, writeData->dataSize); // TODO memcpy seems to be necessary since data is freed after the thread is being deleted? Otherwise we could reset with data!
	}
	catch (std::exception &exception)
	{
		// jpeg_abort_compress is only used when cinfo has to be used again.
		jpeg_destroy_compress(&cinfo); // discard object
		writeData->stateMessage = exception.what();

		return;
	}
	catch (...)
	{
		// jpeg_abort_compress is only used when cinfo has to be used again.
		jpeg_destroy_compress(&cinfo); // discard object

		return;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	writeData->finished = true;
}

}

namespace
{

struct MipMapHeaderData
{
	dword offset, size;
};

}

std::streamsize Blp::read(InputStream &istream,  const std::size_t &mipMaps, const bool threads) throw (class Exception)
{
	this->clear();
	// header
	dword identifier;
	std::streamsize size = 0;
	wc3lib::read(istream, identifier, size);
	this->m_format = format((byte*)&identifier, sizeof(identifier));

	typedef boost::shared_ptr<MipMapHeaderData> MipMapHeaderDataPtr;
	std::vector<MipMapHeaderDataPtr> mipMapData(Blp::maxMipMaps);

	if (this->format() == Blp::Format::Blp0 || this->format() == Blp::Format::Blp1)
	{
		struct BlpHeader header;
		wc3lib::read(istream, header, size);
		this->m_compression = (BOOST_SCOPED_ENUM(Compression))(header.compression);
		this->m_flags = (BOOST_SCOPED_ENUM(Flags))(header.flags);
		this->m_width = header.width;
		this->m_height = header.height;
		this->m_pictureType = header.pictureType;
		this->m_pictureSubType = header.pictureSubType;

		for (std::size_t i = 0; i < Blp::maxMipMaps; ++i)
		{
			mipMapData[i].reset(new MipMapHeaderData);
			mipMapData[i]->offset = header.mipMapOffset[i],
			mipMapData[i]->size = header.mipMapSize[i];
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

		this->m_flags = static_cast<BOOST_SCOPED_ENUM(Flags)>(header.alphaDepth);
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
			mipMapData[i].reset(new MipMapHeaderData);
			mipMapData[i]->offset = header.mipMapOffset[i],
			mipMapData[i]->size = header.mipMapSize[i];
		}
	}

	std::size_t mipMapsCount = requiredMipMaps(this->m_width, this->m_height);

	if (mipMaps != 0 && mipMaps < mipMapsCount)
		mipMapsCount = mipMaps;

	if (mipMapsCount == 0)
		throw Exception(_("Detected 0 MIP maps (too little)."));

	//std::cout << "Required mip maps are " << mipMapsCount << " with width " << this->m_width << " and height " << this->m_height << std::endl; // TEST

	this->mipMaps().resize(mipMapsCount, 0);

	for (std::size_t i = 0; i < mipMapsCount; ++i)
	{
		this->mipMaps().replace(i, new MipMap(this->mipMapWidth(i), this->mipMapHeight(i)));

		if (this->compression() == Blp::Compression::Paletted)
		{
			if (this->flags() == Blp::Flags::NoAlpha && mipMapData[i]->size != this->mipMaps()[i].width() * this->mipMaps()[i].height() * sizeof(byte))
				std::cerr << boost::format(_("MIP map %1%: Size %2% is not equal to %3%.")) % i %  mipMapData[i]->size % (this->mipMaps()[i].width() * this->mipMaps()[i].height() * sizeof(color)) << std::endl;
			else if (this->flags() & Blp::Flags::Alpha && mipMapData[i]->size != this->mipMaps()[i].width() * this->mipMaps()[i].height() * 2 * sizeof(byte))
				std::cerr << boost::format(_("MIP map %1%: Size %2% is not equal to %3%.")) % i %  mipMapData[i]->size % (this->mipMaps()[i].width() * this->mipMaps()[i].height() * 2 * sizeof(color)) << std::endl;
		}
	}

	if (this->compression() == Blp::Compression::Jpeg)
	{
		if (BITS_IN_JSAMPLE > sizeof(byte) * 8)
			throw Exception(boost::format(_("Too many bits in one single sample (one single pixel color channel): %1%. BLP/wc3lib allows maximum sample size of %2%.")) % BITS_IN_JSAMPLE % (sizeof(byte) * 8));

		dword jpegHeaderSize;
		wc3lib::read(istream, jpegHeaderSize, size);

		if (jpegHeaderSize != 624) // usual size of headers of Blizzard BLPs
			std::cerr << boost::format(_("Warning: JPEG (JFIF) header size is not equal to 624 which is the usual size of Blizzard's JPEG compressed BLPs. It is %1%.")) % jpegHeaderSize << std::endl;

		boost::scoped_array<byte> jpegHeader(new byte[jpegHeaderSize]);
		wc3lib::read(istream, jpegHeader[0], size, jpegHeaderSize);

		/**
		 * For each MIP map a thread is added to thread group.
		 * All threads are started nearly at the same time after preparing all necessary buffer data etc.
		 * When MIP reading has finished (with or without any erros) current main thread checks for any errors and continues work.
		 */
		boost::ptr_vector<ReadData> readData(this->mipMaps().size());

		/// \todo Sort thread priorities by MIP map size Current thread gets low priority after starting all!!!
		for (std::size_t i = 0; i < this->mipMaps().size(); ++i)
		{
			const dword mipMapOffset = mipMapData[i]->offset;
			const dword mipMapSize = mipMapData[i]->size;
			// all mipmaps use the same header, jpeg header has been allocated before and is copied into each mip map buffer.
			const std::size_t bufferSize = boost::numeric_cast<std::size_t>(jpegHeaderSize) + boost::numeric_cast<std::size_t>(mipMapSize);
			unsigned char *buffer = new unsigned char[bufferSize]; // buffer has to be deleted by JPEG decompressor
			memcpy(reinterpret_cast<void*>(buffer), reinterpret_cast<const void*>(jpegHeader.get()), jpegHeaderSize); // copy header data
			// moving to offset, skipping null bytes
			const std::streampos position = istream.tellg();
			istream.seekg(mipMapOffset);

			if (!istream)
				throw Exception(boost::format(_("Invalid MIP map offset %1% for MIP map %2%")) % mipMapOffset % i);

			const std::size_t nullBytes = istream.tellg() - position;

			if (nullBytes > 0)
				std::cout << boost::format(_("Ignoring %1% 0 bytes.")) % nullBytes << std::endl;

			// read mip map data starting at header offset, header has already been copied into buffer
			wc3lib::read(istream, buffer[jpegHeaderSize], size, boost::numeric_cast<std::streamsize>(mipMapSize));
			readData.push_back(new ReadData(this->mipMaps()[i], buffer, bufferSize));
		}

		if (threads)
		{
			boost::thread_group threadGroup; // added threads are being destroyed automatically when group is being destroyed

			for (std::size_t i = 0; i < readData.size(); ++i)
				threadGroup.create_thread(boost::bind(&readMipMapJpeg, &readData[i]));

			threadGroup.join_all(); // wait for all threads have finished
		}
		else
		{
			for (std::size_t i = 0; i < readData.size(); ++i)
				readMipMapJpeg(&readData[i]);
		}

		// continue, check for errors during thread operations
		for (std::size_t i = 0; i < readData.size(); ++i)
		{
			if (!readData[i].finished)
				throw Exception(boost::format(_("Error during read process of MIP map %1%:\n%2%")) % i % readData[i].stateMessage);
		}
	}
	else if (this->compression() == Blp::Compression::Paletted)
	{
		ColorPtr palette(new color[Blp::compressedPaletteSize]); // uncompressed 1 and 2 only use 256 different colors.

		for (std::size_t i = 0; i < Blp::compressedPaletteSize; ++i)
			wc3lib::read(istream, palette[i], size);

		/// \todo Split up into threads
		for (std::size_t i = 0; i < this->mipMaps().size(); ++i)
		{
			const dword mipMapOffset = mipMapData[i]->offset;
			dword mipMapSize = mipMapData[i]->size;
			const std::streampos position = istream.tellg();
			istream.seekg(mipMapOffset);
			const std::size_t nullBytes = istream.tellg() - position;

			if (nullBytes > 0)
				std::cout << boost::format(_("Ignoring %1% 0 bytes.")) % nullBytes << std::endl;


			for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
			{
				for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
				{
					byte index;
					std::streamsize readSize = 0;
					wc3lib::read(istream, index, readSize);
					size += readSize;
					mipMapSize -= boost::numeric_cast<dword>(readSize);

					this->mipMaps()[i].setColorIndex(width, height, index);
				}
			}

			if (this->flags() & Blp::Flags::Alpha)
			{
				for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
				{
					for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
					{
						byte alpha;
						std::streamsize readSize = 0;
						wc3lib::read(istream, alpha, readSize);
						size += readSize;
						mipMapSize -= boost::numeric_cast<dword>(readSize);
						this->mipMaps()[i].setColorAlpha(width, height, alpha);
					}
				}
			}

			// skip unnecessary bytes
			if (mipMapSize != 0)
				istream.seekg(mipMapSize, std::ios_base::cur);
		}

		m_palette.swap(palette); // exception safe
	}
	else
		throw Exception(boost::format(_("Unknown compression mode: %1%.")) % this->compression());

	//std::cout << "Read " << size << " bytes." << std::endl;

	// check mip maps
	/*
	- A full mipmap chain must be present. The last mipmap must be 1x1 (no larger).
	If an image is 32x8 the mipmap chain must be 32x8, 16x4, 8x2, 4x1, 2x1, 1x1.
	Sizes not of powers of 2 seems to work fine too, the same rules for mipmaps
	still applies. Ex: 24x17, 12x8 (rounded down), 6x4, 3x2, 1x1 (rounded down).
	*/
	if (this->mipMaps().size() > 1 && (this->mipMaps().back().width() != 1 || this->mipMaps().back().height() != 1))
		std::cerr << boost::format(_("Last MIP map has not a size of 1x1 (%1%x%2%).")) % this->mipMaps().back().width() % this->mipMaps().back().height();

	return size;
}

namespace
{

/**
 * Searches for specified marker \p marker in buffer \p buffer. When found it writes marker data of size \p size (if \p variable is true it ignores \p size and reads size after marker from buffer) into output stream \p ostream.
 * \param variable If this value is true you do not have to define \p markerSize since marker size is read immediately after the marker's indicating byte (size of \ref word).
 * \param markerSize Includes size bytes!
 * \param marker Marker's indicating byte value (e. g. 0xD8 for start of image).
 * \return Returns true if marker was found and data has been written into output stream (no stream checks!).
 * \throw Exception Throws an exception if there is not enough data in buffer.
 * \todo (from Wikipedia) Within the entropy-coded data, after any 0xFF byte, a 0x00 byte is inserted by the encoder before the next byte, so that there does not appear to be a marker where none is intended, preventing framing errors. Decoders must skip this 0x00 byte. This technique, called byte stuffing (see JPEG specification section F.1.2.3), is only applied to the entropy-coded data, not to marker payload data.
 */
bool writeJpegMarker(Blp::OutputStream &ostream, std::streamsize &size, bool variable, word markerSize, const byte marker, const unsigned char *buffer, const std::size_t bufferSize) throw (Exception)
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

			if (variable || markerSize > 0)
			{
				++j;

				if (j + 1 > bufferSize) // we need 2 bytes for marker size (which could be 0 as well)
					throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data to get its owen data size.")) % marker);

				if (variable)
				{
					//memcpy(&markerSize, &(buffer[j]), sizeof(markerSize));
					// TEST
					markerSize = (word(buffer[j]) << 8) + word(buffer[j + 1]);
					std::cerr << "Marker size first byte: " << (byte)buffer[j] << std::endl;
					std::cerr << "Marker size second byte: " << (byte)buffer[j + 1] << std::endl;
					std::cerr << "Marker size: " << markerSize << std::endl;
					std::cerr << "Marker size test: " << (word(buffer[j]) << 8) + word(buffer[j + 1]) << std::endl;
					std::cerr << "Left value: " << (word(buffer[j]) << 8) << std::endl;
					std::cerr << "Right value: " << word(buffer[j + 1]) << std::endl;
				}
			}

			// 0xFF + marker + marker size
			if (i + 2 + markerSize > bufferSize)
				throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data with marker size %2% and data size %3%, starting at %4%.")) % marker % markerSize % bufferSize % (i + 2));

			// marker size is 2 bytes long and includes its own size!
			wc3lib::write(ostream, buffer[i], size, markerSize + 2); // + sizeof 0xFF and marker

			return true;
		}
	}

	return false;
}

}

std::streamsize Blp::write(OutputStream &ostream, const int &quality, const std::size_t &mipMaps, const bool threads) const throw (class Exception)
{
	if (quality < -1)
	{
		std::cerr << boost::format(_("Invalid quality %1%. Minimum value is -1 which indicates using default quality.")) % quality << std::endl;
		const_cast<int&>(quality) = -1;
	}
	else if (quality > 100)
	{
		std::cerr << boost::format(_("Invalid quality %1%. Maximum value is 100 which indicates using best quality.")) % quality << std::endl;
		const_cast<int&>(quality) = 100;
	}

	if (mipMaps > Blp::maxMipMaps)
	{
		std::cerr << boost::format(_("Invalid MIP maps number %1%. Maximum value is %2%.")) % mipMaps % Blp::maxMipMaps << std::endl;
		const_cast<std::size_t&>(mipMaps) = Blp::maxMipMaps;
	}

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


	dword startOffset = 0; /// Offset where MIP map offsets and sizes are written down at the end of the whole writing process.

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
				header.encoding = 1;
			else if (this->compression() == Blp::Compression::DirectXCompression)
				header.encoding = 2;
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
			memcpy(static_cast<void*>(header.palette), static_cast<const void*>(palette().get()), Blp::compressedPaletteSize);

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

	std::vector<dword> offsets(actualMipMaps, 0);
	std::vector<dword> sizes(actualMipMaps, 0);

	if (this->compression() == Blp::Compression::Jpeg)
	{
		/**
		 * For each MIP map a thread is added to thread group.
		 * All threads are started nearly at the same time after preparing all necessary buffer data etc.
		 * When MIP writing has finished (with or without any erros) current main thread checks for any errors and continues work.
		 */
		//typedef boost::shared_ptr<WriteData> WriteDataType;
		boost::ptr_vector<WriteData> writeData(actualMipMaps);

		/// \todo Sort thread priorities by MIP map size. Current thread gets low priority after starting all!!!
		for (std::size_t i = 0; i < actualMipMaps; ++i)
			writeData.push_back(new WriteData(this->mipMaps()[i], i == 0, (quality < 0 || quality > 100 ? Blp::defaultQuality : quality)));

		if (threads)
		{
			boost::thread_group threadGroup; // added threads are being destroyed automatically when group is being destroyed

			for (std::size_t i = 0; i < writeData.size(); ++i)
				threadGroup.create_thread(boost::bind(&writeMipMapJpeg, &writeData[i]));

			threadGroup.join_all(); // wait for all threads have finished
		}
		else
		{
			for (std::size_t i = 0; i < writeData.size(); ++i)
				writeMipMapJpeg(&writeData[i]);
		}

		// continue, check for errors during thread operations
		for (std::size_t i = 0; i < writeData.size(); ++i)
		{
			if (!writeData[i].finished)
				throw Exception(boost::format(_("Error during write process of MIP map %1%:\n%2%")) % i % writeData[i].stateMessage);
		}

		// skip shared header size
		std::streampos headerPosition = ostream.tellp();
		ostream.seekp(sizeof(dword), std::ios::cur);

		std::streamsize headerSize = 0;
		// NOTE marker reference: https://secure.wikimedia.org/wikipedia/en/wiki/JPEG#Syntax_and_structure
		// TODO would be much faster not to write unnecessary marker data for other MIP maps than first but seems to be impossible with jpeglib. Unfortunately, MIP maps still need some parts of their headers including their size information!
		if (!writeJpegMarker(ostream, headerSize, false, 0, 0xD8, &(writeData[0].data[0]), writeData[0].headerSize)) // image start
			throw Exception(_("Missing image start marker 0xD8."));

		// start after image start to increase performance
		// TODO huffman table marker data size seems to be too large (marker is found and size is read - 2 bytes - which include its own size of 2).
		if (!writeJpegMarker(ostream, headerSize, true, 0, 0xC4, &(writeData[0].data[headerSize]), writeData[0].headerSize - headerSize)) // huffman table
			throw Exception(_("Missing huffman table marker 0xC4."));

		// start after huffman table to increase performance
		if (!writeJpegMarker(ostream, headerSize, true, 0, 0xDB, &(writeData[0].data[headerSize]), writeData[0].headerSize - headerSize)) // quantization table
			throw Exception(_("Missing quantization table marker 0xDB."));

		// TODO support APPn marker which should be equal for all MIP maps (meta data)
		// TODO Are markers really stored end to end as written down on Wikipedia? If not we cannot start after already read markers!
		// start after quantization table to increase performance
		writeJpegMarker(ostream, headerSize, true, 0, 0xFE, &(writeData[0].data[headerSize]), writeData[0].headerSize - headerSize); // image comment

		if (headerSize != 624) // usual size of headers of Blizzard BLPs
			std::cerr << boost::format(_("Warning: JPEG (JFIF) header size is not equal to 624 which is the usual size of Blizzard's JPEG compressed BLPs. It is %1%.")) % headerSize << std::endl;

		// write shared header size and jump back again
		writeByteCount(ostream, boost::numeric_cast<dword>(headerSize), headerPosition, size);

		// write MIP map data
		for (std::size_t i = 0; i < writeData.size(); ++i)
		{
			offsets[i] = ostream.tellp();
			std::streamsize mipMapSize = 0;
			// write non-shared MIP map header data
			if (!writeJpegMarker(ostream, mipMapSize, true, 0, 0xC0, &(writeData[i].data[0]), writeData[i].headerSize)) // start of frame
				throw Exception(boost::format(_("Missing start of frame marker 0xC0 for MIP map %1%.")) % i);

			// start after start of frame to increase performance
			if (!writeJpegMarker(ostream, mipMapSize, true, 0, 0xDA, &(writeData[i].data[mipMapSize]), writeData[i].headerSize - mipMapSize)) // start of scan
				throw Exception(boost::format(_("Missing start of scan marker 0xDA for MIP map %1%.")) % i);

			wc3lib::write(ostream, writeData[i].data[writeData[i].headerSize], mipMapSize, writeData[i].dataSize - writeData[i].headerSize);
			sizes[i] = mipMapSize;
		}
	}
	else if (this->compression() == Blp::Compression::Paletted)
	{
		// write palette, palette has always size of Blp::compressedPaletteSize (remaining colors have value 0).
		if (format() != Blp::Format::Blp2)
			wc3lib::write(ostream, palette()[0], size, Blp::compressedPaletteSize * sizeof(color));

		// write mip maps
		for (std::size_t i = 0; i < actualMipMaps; ++i)
		{
			dword mipMapOffset = ostream.tellp();
			dword mipMapSize = size;

			for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
			{
				for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
				{
					byte index = this->mipMaps()[i].colorAt(width, height).paletteIndex();
					wc3lib::write(ostream, index, size);

					/*
					if (this->m_flags == Blp::Alpha)
					{
						byte alpha = mipMap->colorAt(width, height).alpha();
						wc3lib::write(ostream, alpha, size);
					}
					*/
				}
			}

			if (this->m_flags & Blp::Flags::Alpha)
			{
				for (dword height = 0; height < this->mipMaps()[i].height(); ++height)
				{
					for (dword width = 0; width < this->mipMaps()[i].width(); ++width)
					{
						byte alpha = this->mipMaps()[i].colorAt(width, height).alpha();
						wc3lib::write(ostream, alpha, size);
					}
				}
			}

			// set MIP map header data
			mipMapSize = size - mipMapSize;

			offsets[i] = mipMapOffset;
			sizes[i] = mipMapSize;
		}
	}
	else
		throw Exception(boost::format(_("Unknown compression mode: %1%.")) % (dword)this->compression());

	// write MIP map header data, jump to header
	std::streampos position = ostream.tellp();
	ostream.seekp(startOffset);

	for (std::size_t index = 0; index < actualMipMaps; ++index)
	{
		std::streamsize tmpSize = 0;
		// For BLP2 we would count size at twice since it has been in header already.
		wc3lib::write(ostream, offsets[index], format() == Blp::Format::Blp2 ? tmpSize : size);

	}

	// skip remaining offsets
	const std::size_t remaining = Blp::maxMipMaps - (mipMaps == 0 ? this->mipMaps().size() : mipMaps);
	ostream.seekp(remaining * sizeof(dword), std::ios::cur);

	for (std::size_t index = 0; index < actualMipMaps; ++index)
	{
		std::streamsize tmpSize = 0;
		// For BLP2 we would count size at twice since it has been in header already.
		wc3lib::write(ostream, sizes[index], format() == Blp::Format::Blp2 ? tmpSize : size);

	}

	// Add skipped MIP map header data size to written size. This data has been skipped at the beginning of writing process.
	if (format() != Blp::Format::Blp2 && remaining > 0)
		size += remaining * sizeof(dword) * 2;

	ostream.seekp(position); // jump back to the end of stream

	return size;
}

uint32_t Blp::version() const
{
	return (uint32_t)format();
}

int Blp::generateMipMaps(std::size_t number, bool regenerate) throw (class Exception)
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

const Blp::ColorPtr& Blp::palette() const throw (Exception)
{
	return const_cast<Blp*>(this)->palette();
}

Blp::ColorPtr& Blp::palette() throw (Exception)
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
