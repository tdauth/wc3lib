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

namespace wc3lib
{

namespace blp
{

const std::size_t Blp::maxMipMaps = 16;
const std::size_t Blp::compressedPaletteSize = 256;
const int Blp::defaultQuality = 100;

//#ifndef STATIC
// jpeg read functions
typedef struct jpeg_error_mgr* (*jpeg_std_errorType)(struct jpeg_error_mgr*);
typedef void (*jpeg_CreateDecompressType)(j_decompress_ptr, int, size_t);
typedef void (*jpeg_mem_srcType)(j_decompress_ptr, unsigned char*, unsigned long);
typedef int (*jpeg_read_headerType)(j_decompress_ptr, boolean);
typedef boolean (*jpeg_start_decompressType)(j_decompress_ptr);
typedef void (*jpeg_abort_decompressType)(j_decompress_ptr);
typedef void (*jpeg_destroy_decompressType)(j_decompress_ptr);
typedef JDIMENSION (*jpeg_read_scanlinesType)(j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines);
typedef boolean (*jpeg_finish_decompressType)(j_decompress_ptr cinfo);

typedef void (*jpeg_CreateCompressType)(j_compress_ptr cinfo, int version, size_t structsize);
typedef void (*jpeg_mem_destType)(j_compress_ptr cinfo, unsigned char **outbuffer, unsigned long *outsize);
typedef void (*jpeg_set_defaultsType)(j_compress_ptr cinfo);
typedef void (*jpeg_set_colorspaceType)(j_compress_ptr cinfo, J_COLOR_SPACE colorspace);
typedef void (*jpeg_default_colorspaceType)(j_compress_ptr cinfo);
typedef void (*jpeg_set_qualityType)(j_compress_ptr cinfo, int quality, boolean force_baseline);
typedef void (*jpeg_set_linear_qualityType)(j_compress_ptr cinfo, int scale_factor, boolean force_baseline);
typedef void (*jpeg_start_compressType)(j_compress_ptr cinfo, boolean write_all_tables);
typedef void (*jpeg_abort_compressType)(j_compress_ptr cinfo);
typedef void (*jpeg_destroy_compressType)(j_compress_ptr cinfo);
typedef JDIMENSION (*jpeg_write_scanlinesType)(j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines);
typedef void (*jpeg_finish_compressType)(j_compress_ptr cinfo);
//#endif

bool Blp::MipMap::Color::operator==(const class Color &other) const
{
	/*
	color argb0 = this->argb();
	byte alpha0 = 0;
	color argb1 = other.argb();
	byte alpha1 = 0;

	if (this->m_mipMap->m_blp->flags() & Alpha)
		alpha0 = this->alpha();

	if (other.m_mipMap->m_blp->flags() & Alpha)
		alpha1 = other.alpha();
	*/

	return this->argb() == other.argb() && this->alpha() == other.alpha();
}

bool Blp::MipMap::Color::operator!=(const class Color &other) const
{
	return !(*this == other);
}

Blp::MipMap::Color::Color() : m_mipMap(0), m_argb(0), m_alpha(0), m_paletteIndex(0)
{
}

Blp::MipMap::Color::~Color()
{
}

Blp::MipMap::Color::Color(class Blp::MipMap *mipMap, color argb, byte alpha, byte paletteIndex) : m_mipMap(mipMap), m_argb(argb), m_alpha(alpha), m_paletteIndex(paletteIndex)
{
}

/// @todo Implement.
void Blp::MipMap::scale(dword newWidth, dword newHeight) throw (class Exception)
{
	throw Exception(_("MipMap::scale - Not implemented yet!"));
}

Blp::MipMap::MipMap(class Blp *blp, dword width, dword height) : m_blp(blp), m_width(width), m_height(height)
{
}

Blp::MipMap::~MipMap()
{
}

BOOST_SCOPED_ENUM(Blp::Format) Blp::format(const byte *buffer, const std::size_t bufferSize) throw (Exception)
{
	if (bufferSize < sizeof(dword))
		throw Exception(boost::format(_("Error while reading BLP file. BLP identifier is too short: %1%. Expected size of %2%.")) % bufferSize % sizeof(dword));
	
	static const dword identifier0 = Blp::Format::Blp0;
	static const dword identifier1 = Blp::Format::Blp1;
	static const dword identifier2 = Blp::Format::Blp2;
	
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
	
	static const dword identifier0 = Blp::Format::Blp0;
	static const dword identifier1 = Blp::Format::Blp1;
	static const dword identifier2 = Blp::Format::Blp2;
	
	if (memcmp(buffer, &identifier0, sizeof(dword)) == 0)
		return true;
	else if (memcmp(buffer, &identifier1, sizeof(dword)) == 0)
		return true;
	else if (memcmp(buffer, &identifier2, sizeof(dword)) == 0)
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

	BOOST_FOREACH(MipMapPtr mipMap, this->m_mipMaps)
		mipMap.reset();
		
	m_palette.reset();
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

std::string jpegError(jpeg_std_errorType jpeg_std_error, const std::string &message)
{
	return boost::str(boost::format(message) % (jpeg_std_error(0)->jpeg_message_table != 0 ? jpeg_std_error(0)->jpeg_message_table[jpeg_std_error(0)->last_jpeg_message] : _("No error")));
}

struct JpegLoader
{
	class LibraryLoader::Handle *libraryHandle;
	jpeg_std_errorType jpeg_std_error;
	jpeg_CreateDecompressType jpeg_CreateDecompress;
	jpeg_mem_srcType jpeg_mem_src;
	jpeg_read_headerType jpeg_read_header;
	jpeg_start_decompressType jpeg_start_decompress;
	jpeg_abort_decompressType jpeg_abort_decompress;
	jpeg_destroy_decompressType jpeg_destroy_decompress;
	jpeg_read_scanlinesType jpeg_read_scanlines;
	jpeg_finish_decompressType jpeg_finish_decompress;
	
	~JpegLoader()
	{
		unload();
	}
	
	void unload() throw (class Exception)
	{
		if (libraryHandle != 0)
		{
			LibraryLoader::unloadLibrary(libraryHandle);
			libraryHandle = 0;
		}
	}

	void load() throw (class Exception)
	{
		try
		{
			libraryHandle = LibraryLoader::loadLibrary("jpeg");
			void *symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_std_error");
			jpeg_std_error = *((jpeg_std_errorType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_CreateDecompress");
			jpeg_CreateDecompress = *((jpeg_CreateDecompressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_mem_src");
			jpeg_mem_src = *((jpeg_mem_srcType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_read_header");
			jpeg_read_header = *((jpeg_read_headerType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_start_decompress");
			jpeg_start_decompress = *((jpeg_start_decompressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_abort_decompress");
			jpeg_abort_decompress = *((jpeg_abort_decompressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_destroy_decompress");
			jpeg_destroy_decompress = *((jpeg_destroy_decompressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_read_scanlines");
			jpeg_read_scanlines = *((jpeg_read_scanlinesType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_finish_decompress");
			jpeg_finish_decompress = *((jpeg_finish_decompressType*)(&symbol));
		}
		catch (class Exception &exception)
		{
			exception.what().append("\nRequired for BLP reading.");

			throw exception;
		}
	}
};

struct JpegWriter
{
	class LibraryLoader::Handle *libraryHandle;
	jpeg_std_errorType jpeg_std_error;
	jpeg_CreateCompressType jpeg_CreateCompress;
	jpeg_mem_destType jpeg_mem_dest;
	jpeg_set_defaultsType jpeg_set_defaults;
	jpeg_set_colorspaceType jpeg_set_colorspace;
	jpeg_default_colorspaceType jpeg_default_colorspace;
	jpeg_set_qualityType jpeg_set_quality;
	jpeg_set_linear_qualityType jpeg_set_linear_quality;
	jpeg_start_compressType jpeg_start_compress;
	jpeg_abort_compressType jpeg_abort_compress;
	jpeg_destroy_compressType jpeg_destroy_compress;
	jpeg_write_scanlinesType jpeg_write_scanlines;
	jpeg_finish_compressType jpeg_finish_compress;
	
	~JpegWriter()
	{
		unload();
	}
	
	void unload() throw (class Exception)
	{
		if (libraryHandle != 0)
		{
			LibraryLoader::unloadLibrary(libraryHandle);
			libraryHandle = 0;
		}
	}

	void load() throw (class Exception)
	{
		try
		{
			libraryHandle = LibraryLoader::loadLibrary("jpeg");
			void *symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_std_error");
			//jpeg_std_error = (jpeg_std_error)(symbol);
			jpeg_std_error = *((jpeg_std_errorType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_CreateCompress");
			jpeg_CreateCompress = *((jpeg_CreateCompressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_mem_dest");
			jpeg_mem_dest = *((jpeg_mem_destType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_set_defaults");
			jpeg_set_defaults = *((jpeg_set_defaultsType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_set_colorspace");
			jpeg_set_colorspace = *((jpeg_set_colorspaceType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_default_colorspace");
			jpeg_default_colorspace = *((jpeg_default_colorspaceType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_set_quality");
			jpeg_set_quality = *((jpeg_set_qualityType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_start_compress");
			jpeg_start_compress = *((jpeg_start_compressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_abort_compress");
			jpeg_abort_compress = *((jpeg_abort_compressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_destroy_compress");
			jpeg_destroy_compress = *((jpeg_destroy_compressType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_write_scanlines");
			jpeg_write_scanlines = *((jpeg_write_scanlinesType*)(&symbol));
			symbol = LibraryLoader::librarySymbol(*libraryHandle, "jpeg_finish_compress");
			jpeg_finish_compress = *((jpeg_finish_compressType*)(&symbol));
		}
		catch (class Exception &exception)
		{
			exception.what().append("\nRequired for BLP reading.");

			throw exception;
		}
	}
};

BOOST_SCOPED_ENUM_START(State)
{
	None,
	Ready,
	Failed
};
BOOST_SCOPED_ENUM_END

struct ReadData
{
	ReadData(Blp::MipMap &mipMap, unsigned char * const data, const std::size_t dataSize, const JpegLoader &loader) : mipMap(mipMap), data(data), dataSize(dataSize), loader(loader), state(State::None)
	{
	}
	
	~ReadData()
	{
		//delete[] data;
	}
	
	Blp::MipMap &mipMap;
	unsigned char * const data; /// All required data from stream which has size \ref dataSize (does contain JPEG header as well). Note that this buffer has to be deleted/freed by the \ref readMipMapJpeg function! Usually this happens when decompressor is being destroyed. Therefore it cannot be a \ref boost::scoped_ptr since it wouldn't be freed by delete operator.
	const std::size_t dataSize;
	const JpegLoader &loader;
	BOOST_SCOPED_ENUM(State) state;
};

struct WriteData
{
	WriteData(const Blp::MipMap &mipMap, const JpegWriter &writer, bool isFirst, int quality) : mipMap(mipMap), dataSize(0), writer(writer), isFirst(isFirst), quality(quality), headerSize(0), state(State::None)
	{
	}
	
	~WriteData()
	{
	}
	
	const Blp::MipMap &mipMap;
	boost::shared_array<unsigned char> data; /// All required data from stream which has size \ref dataSize (does contain JPEG header as well).
	std::size_t dataSize;
	const JpegWriter &writer;
	const bool isFirst; /// If this value is true JPEG header has to be written for this MIP map (usually this is the first one!).
	int quality; /// Reaches from 0-100.
	std::size_t headerSize; /// Only useful if isFirst is true (header size in buffer \ref data).
	BOOST_SCOPED_ENUM(State) state;
};


/**
 * Function for multithreading. Has to be thread-safe!
 * \todo MIP map has to be locked until everything finished.
 * \todo Is data cleared when dempress is aborted due to failure.
 */
void readMipMapJpeg(struct ReadData &readData)
{
	JSAMPARRAY scanlines = 0; // will be filled later

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = readData.loader.jpeg_std_error(&jerr);
	readData.loader.jpeg_create_decompress(&cinfo);


	try
	{
		readData.loader.jpeg_mem_src(&cinfo, readData.data, readData.dataSize);
		//std::cout << "Buffer size is " << dataSize << std::endl;
		//jpeg_mem_dest(cinfo, &buffer, &bufferSize);

		if (readData.loader.jpeg_read_header(&cinfo, true) != JPEG_HEADER_OK)
			throw Exception(jpegError(readData.loader.jpeg_std_error, _("Did not find header. Error: %1%.")));
		
		if (!readData.loader.jpeg_start_decompress(&cinfo))
			throw Exception(jpegError(readData.loader.jpeg_std_error, _("Could not start decompress. Error: %1%.")));

		if (readData.mipMap.width() != cinfo.image_width)
			std::cerr << boost::format(_("Warning: Image width (%1%) is not equal to mip map width (%2%).")) % cinfo.image_width % readData.mipMap.width() << std::endl;

		if (readData.mipMap.height() != cinfo.image_height)
			std::cerr << boost::format(_("Warning: Image height (%1%) is not equal to mip map height (%2%).")) % cinfo.image_height % readData.mipMap.height() << std::endl;

		if (cinfo.out_color_space != JCS_RGB)
			std::cerr << boost::format(_("Warning: Image color space (%1%) is not equal to RGB (%2%).")) % cinfo.out_color_space % JCS_RGB << std::endl;

		//std::cout << "JPEG image has width " << cinfo.image_width << " and height " << cinfo.image_height << std::endl;
		//std::cout << "JPEG image has scaled width " << cinfo.output_width << " and scaled height " << cinfo.output_height << std::endl;
		//std::cout << "Color map has size " << cinfo.actual_number_of_colors << std::endl;

		/// \todo Get as much required scanlines as possible (highest divident) to increase speed. Actually it could be equal to the MIP maps height which will lead to reading the whole MIP map with one single \ref jpeg_read_scanlines call
		static const JDIMENSION requiredScanlines = 1; // increase this value to read more scanlines in one step
		JDIMENSION scanlineSize = cinfo.output_width * cinfo.output_components; // JSAMPLEs per row in output buffer
		scanlines = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, scanlineSize, requiredScanlines); // TODO allocate via boost scoped pointer please!
		//std::cout << "Required scanlines " << requiredScanlines << std::endl;
		//std::cout << "Size of scanline " << scanlineSize << " (width * components)" << std::endl;
		//std::cout << "we do have " << cinfo.output_components << " components/color channels." << std::endl;

		// per scanline
		while (cinfo.output_scanline < cinfo.output_height)
		{
			//std::cout << "Output scanline is " << cinfo.output_scanline << " and buffer size is " << scanlinesSize << " and height is " <<  cinfo.output_height << std::endl;

			//scanlines[cinfo.output_scanline] = new JSAMPLE[cinfo.output_height];
			JDIMENSION width = 0;
			JDIMENSION height = cinfo.output_scanline; // cinfo.output_scanline is increased by one after calling jpeg_read_scanlines
			JDIMENSION dimension = readData.loader.jpeg_read_scanlines(&cinfo, scanlines, requiredScanlines); // scanlinesSize
			//std::cout << "Read " << dimension << " scanlines" << std::endl;

			if (dimension != requiredScanlines)
				std::cerr << boost::format(_("Warning: Number of scanned lines is not equal to %1%. It is %2%.")) % requiredScanlines % dimension << std::endl;

			for (JDIMENSION scanline = 0; scanline < dimension; ++scanline, ++height)
			{
				// cinfo.output_components should be 3 if RGB and 4 if RGBA
				for (int component = 0; component < scanlineSize; component += cinfo.output_components)
				{
					// store as ARGB (BLP)
					// TODO why is component 0 blue, component 1 green and component 2 red?
					// Red and Blue colors are swapped.
					// http://www.wc3c.net/showpost.php?p=1046264&postcount=2
					color argb = ((color)scanlines[scanline][component]) | ((color)scanlines[scanline][component + 1] << 8) | ((color)scanlines[scanline][component + 2] << 16);

					if (cinfo.output_components == 4) // we do have an alpha channel
						argb |= ((color)(0xFF - scanlines[scanline][component + 3]) << 24);
					//std::cout << "ARGB " << std::ios::hex << argb << std::endl;

					readData.mipMap.setColor(width, height, argb, 0); /// \todo Get alpha?!
					++width;
				}
			}

			//std::cout << "read " << i << " blas" << std::endl;


			//put_scanline_someplace(scanlines[0], scanlinesSize);
			//std::cout << "I is " << i << " and read lines " << dimension << std::endl;
		}
	}
	catch (...)
	{
		// jpeg_abort_decompress is only used when cinfo has to be used again.
		readData.loader.jpeg_destroy_decompress(&cinfo); // discard object
		readData.state = State::Failed;
		
		return;
	}

	readData.loader.jpeg_finish_decompress(&cinfo);

	if (!cinfo.saw_JFIF_marker)
		std::cerr << boost::format(_("Warning: Did not find JFIF marker. JFIF format is used by default!\nThis is the JFIF version of the image %1%.%2%")) % cinfo.JFIF_major_version % cinfo.JFIF_minor_version << std::endl;

	readData.loader.jpeg_destroy_decompress(&cinfo);
	readData.state = State::Ready;
	//std::cout << "Finished decompress of MIP map and updated state." << std::endl;
}

/**
 * Function for multithreading. Has to be thread-safe!
 * \todo MIP map has to be locked until everything finished.
 */
void writeMipMapJpeg(struct WriteData &writeData)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = writeData.writer.jpeg_std_error(&jerr);
	writeData.writer.jpeg_create_compress(&cinfo);

	try
	{
		unsigned char *data = 0;
		unsigned long size = 0;
		writeData.writer.jpeg_mem_dest(&cinfo, &data, &size);
		cinfo.image_width = writeData.mipMap.width();
		cinfo.image_height = writeData.mipMap.height();
		cinfo.input_components = 3; // ARGB, 4
		cinfo.in_color_space = JCS_RGB;
		cinfo.write_JFIF_header = writeData.isFirst; // we're sharing our header
		writeData.writer.jpeg_set_defaults(&cinfo);
		writeData.writer.jpeg_set_quality(&cinfo, writeData.quality, false);
		writeData.writer.jpeg_start_compress(&cinfo, TRUE);
		
		// get header size
		if (writeData.isFirst)
			writeData.headerSize = boost::numeric_cast<std::size_t>(size);
		
		boost::scoped_array<JSAMPLE> buffer(new JSAMPLE[cinfo.input_components]);
		
		BOOST_FOREACH(const Blp::MipMap::MapEntryType entry, writeData.mipMap.colors())
		{
			const color argb = entry.second.argb();
			// TODO why is component 0 blue, component 1 green and component 2 red?
			// Red and Blue colors are swapped.
			// http://www.wc3c.net/showpost.php?p=1046264&postcount=2
			buffer[0] = blue(argb);
			buffer[1] = green(argb);
			buffer[2] = red(argb);
			
			if (cinfo.input_components == 4) // we do have an alpha channel
				buffer[3] = 0xFF - alpha(argb);
			
			JSAMPROW row = buffer.get();
			writeData.writer.jpeg_write_scanlines(&cinfo, &row, 1);
		}
		
		// output buffer data for further treatment
		writeData.data.reset(data);
		writeData.dataSize = boost::numeric_cast<std::size_t>(size);
	}
	catch (...)
	{
		// jpeg_abort_decompress is only used when cinfo has to be used again.
		writeData.writer.jpeg_destroy_compress(&cinfo); // discard object
		writeData.state = State::Failed;
		
		return;
	}

	writeData.writer.jpeg_finish_compress(&cinfo);
	writeData.writer.jpeg_destroy_compress(&cinfo);
	writeData.state = State::Ready;
}

}

namespace
{

struct MipMapHeaderData
{
	dword offset, size;
};

}

std::streamsize Blp::read(InputStream &istream,  const std::size_t &mipMaps) throw (class Exception)
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

	this->m_mipMaps.resize(mipMapsCount);

	for (std::size_t i = 0; i < mipMapsCount; ++i)
	{
		this->m_mipMaps[i].reset(new MipMap(this, this->mipMapWidth(i), this->mipMapHeight(i)));

		if (this->compression() == Blp::Compression::Paletted)
		{
			if (this->flags() == Blp::Flags::NoAlpha && mipMapData[i]->size != m_mipMaps[i]->width() * m_mipMaps[i]->height() * sizeof(byte))
				std::cerr << boost::format(_("MIP map %1%: Size %2% is not equal to %3%.")) % i %  mipMapData[i]->size % (m_mipMaps[i]->width() * m_mipMaps[i]->height() * sizeof(color)) << std::endl;
			else if (this->flags() & Blp::Flags::Alpha && mipMapData[i]->size != m_mipMaps[i]->width() * m_mipMaps[i]->height() * 2 * sizeof(byte))
				std::cerr << boost::format(_("MIP map %1%: Size %2% is not equal to %3%.")) % i %  mipMapData[i]->size % (m_mipMaps[i]->width() * m_mipMaps[i]->height() * 2 * sizeof(color)) << std::endl;
		}
	}

	if (this->compression() == Blp::Compression::Jpeg)
	{
		//std::cout << "Detected JPEG compression mode." << std::endl;
		
		struct JpegLoader loader;
		loader.load();
		
		//std::cout << "Loaded shared object." << std::endl;
		
		if (BITS_IN_JSAMPLE > sizeof(byte) * 8)
			throw Exception(boost::format(_("Too many bits in one single sample (one single pixel color channel): %1%. BLP/wc3lib allows maximum sample size of %2%.")) % BITS_IN_JSAMPLE % (sizeof(byte) * 8));

		dword jpegHeaderSize;
		wc3lib::read(istream, jpegHeaderSize, size);

		if (jpegHeaderSize != 624) // usual size of headers of Blizzard BLPs
			std::cerr << boost::format(_("Warning: JPEG (JFIF) header size is not equal to 624 which is the usual size of Blizzard's JPEG compressed BLPs. It is %1%.")) % jpegHeaderSize << std::endl;

		boost::scoped_array<byte> jpegHeader(new byte[jpegHeaderSize]);
		
		boost::timer operationTimer; // TEST
		//std::cout << "JPEG header size " << jpegHeaderSize << std::endl;
		wc3lib::read(istream, *jpegHeader.get(), size, jpegHeaderSize);
		//std::cout << "JPEG header size " << jpegHeaderSize << std::endl;
		//std::cout << "-- Reading MIP maps --" << std::endl;
		
		/**
		 * For each MIP map a thread is added to thread group.
		 * All threads are started nearly at the same time after preparing all necessary buffer data etc.
		 * When MIP reading has finished (with or without any erros) current main thread checks for any errors and continues work.
		 */
		typedef boost::shared_ptr<ReadData> ReadDataType;
		std::vector<ReadDataType> readData(this->m_mipMaps.size());
		std::size_t i = 0;
		
		/// \todo Sort thread priorities by MIP map size Current thread gets low priority after starting all!!!
		BOOST_FOREACH(MipMaps::const_reference mipMap, this->m_mipMaps)
		{
			const dword mipMapOffset = mipMapData[i]->offset;
			const dword mipMapSize = mipMapData[i]->size;
			// all mipmaps use the same header, jpeg header has been allocated before and is copied into each mip map buffer.
			const std::size_t bufferSize = boost::numeric_cast<std::size_t>(jpegHeaderSize) + boost::numeric_cast<std::size_t>(mipMapSize);
			unsigned char *buffer = 0;
			
			try
			{
				buffer = new unsigned char[bufferSize]; // buffer has to be deleted by JPEG decompressor
			}
			catch (std::bad_alloc &exception)
			{
				throw Exception(boost::format(_("Error when allocating MIP map buffer of MIP map %1% with size %2%: \"%3%\".")) % i % bufferSize % exception.what());
			}
			
			memcpy(reinterpret_cast<void*>(buffer), reinterpret_cast<const void*>(jpegHeader.get()), jpegHeaderSize); // copy header data
			// moving to offset, skipping null bytes
			const std::streampos position = istream.tellg();
			istream.seekg(mipMapOffset);
			const std::size_t nullBytes = istream.tellg() - position;

			if (nullBytes > 0)
				std::cout << boost::format(_("Ignoring %1% 0 bytes.")) % nullBytes << std::endl;

			// read mip map data starting at header offset, header has already been copied into buffer
			wc3lib::read(istream, buffer[jpegHeaderSize], size, boost::numeric_cast<std::streamsize>(mipMapSize));
			readData[i].reset(new ReadData(*mipMap, buffer, bufferSize, loader));
			
			++i;
		}
		
		typedef boost::thread* Thread;
		std::vector<Thread> threads(this->m_mipMaps.size(), 0);
		boost::thread_group threadGroup; // added threads are being destroyed automatically when group is being destroyed
		
		for (i = 0; i < readData.size(); ++i)
			threads[i] = threadGroup.create_thread(boost::bind(&readMipMapJpeg, boost::ref(*readData[i])));
		
		threadGroup.join_all(); // wait for all threads have finished
		
		// TEST
		std::cout << "---- Operation has finished with time: " << operationTimer.elapsed() << "s." << std::endl;
		
		// continue, check for errors during thread operations
		i = 0;
		
		BOOST_FOREACH(const ReadDataType &reader, readData)
		{
			if (reader->state == State::Failed)
				throw Exception(boost::format(_("Error during read process of MIP map %1%.")) % i);
			
			++i;
		}
	}
	else if (this->compression() == Blp::Compression::Paletted)
	{
		ColorPtr palette(new color[Blp::compressedPaletteSize]); // uncompressed 1 and 2 only use 256 different colors.

		for (std::size_t i = 0; i < Blp::compressedPaletteSize; ++i)
			wc3lib::read(istream, palette[i], size);

		std::size_t i = 0;
		
		/// \todo Split up into threads
		BOOST_FOREACH(MipMapPtr mipMap, this->m_mipMaps)
		{
			const dword mipMapOffset = mipMapData[i]->offset;
			dword mipMapSize = mipMapData[i]->size;
			const std::streampos position = istream.tellg();
			istream.seekg(mipMapOffset);
			const std::size_t nullBytes = istream.tellg() - position;

			if (nullBytes > 0)
				std::cout << boost::format(_("Ignoring %1% 0 bytes.")) % nullBytes << std::endl;


			for (dword height = 0; height < mipMap->height(); ++height)
			{
				for (dword width = 0; width < mipMap->width(); ++width)
				{
					byte index;
					std::streamsize readSize = 0;
					wc3lib::read(istream, index, readSize);
					size += readSize;
					mipMapSize -= boost::numeric_cast<dword>(readSize);
					/*
					byte alpha = 0;

					if (this->m_flags == Blp::Alpha)
					{
						readSize = 0;
						wc3lib::read(istream, alpha, readSize);
						size += readSize;
						mipMapSize -= readSize;
					}
					*/

					mipMap->setColor(width, height, palette[index], 0, index);
				}
			}

			if (this->flags() & Blp::Flags::Alpha)
			{
				for (dword height = 0; height < mipMap->height(); ++height)
				{
					for (dword width = 0; width < mipMap->width(); ++width)
					{
						byte alpha;
						std::streamsize readSize = 0;
						wc3lib::read(istream, alpha, readSize);
						size += readSize;
						mipMapSize -= boost::numeric_cast<dword>(readSize);
						mipMap->setColorAlpha(width, height, alpha);
					}
				}
			}

			if (mipMapSize != 0)
			{
				istream.seekg(mipMapSize, std::ios_base::cur);
				//std::cout << "Skipping " << mipMapSize << " unnecessary bytes." << std::endl;
			}

			//std::cout << "Mip map colors map size " << mipMap->m_colors.size() << std::endl;
			++i;
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
	if (this->mipMaps().size() > 1 && (this->mipMaps().back()->width() != 1 || this->mipMaps().back()->height() != 1))
		std::cerr << boost::format(_("Last MIP map has not a size of 1x1 (%1%x%2%).")) % this->mipMaps().back()->width() % this->mipMaps().back()->height();

	return size;
}

std::streamsize Blp::write(OutputStream &ostream, const int &quality, const std::size_t &mipMaps) const throw (class Exception)
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
				header.encoding = 1;
			else if (this->compression() == Blp::Compression::DirectXCompression)
				header.encoding = 2;
		}

		header.alphaDepth = static_cast<byte>(this->flags());
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

		header.hasMips = (mipMaps == 0 && this->mipMaps().size() > 1) || (mipMaps > 1);

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
	
	std::vector<dword> offsets(mipMaps == 0 ? this->mipMaps().size() : mipMaps, 0);
	std::vector<dword> sizes(mipMaps == 0 ? this->mipMaps().size() : mipMaps, 0);

	if (this->compression() == Blp::Compression::Jpeg)
	{
		struct JpegWriter writer;
		writer.load();
		
		//std::cout << "Loaded shared object." << std::endl;
		
		if (BITS_IN_JSAMPLE > sizeof(byte) * 8)
			throw Exception(boost::format(_("Too many bits in one single sample (one single pixel color channel): %1%. BLP/wc3lib allows maximum sample size of %2%.")) % BITS_IN_JSAMPLE % (sizeof(byte) * 8));
		
		boost::timer operationTimer; // TEST
		
		/**
		 * For each MIP map a thread is added to thread group.
		 * All threads are started nearly at the same time after preparing all necessary buffer data etc.
		 * When MIP writing has finished (with or without any erros) current main thread checks for any errors and continues work.
		 */
		typedef boost::shared_ptr<WriteData> WriteDataType;
		std::vector<WriteDataType> writeData(mipMaps == 0 ? this->mipMaps().size() : mipMaps);
		std::size_t i = 0;
		
		/// \todo Sort thread priorities by MIP map size. Current thread gets low priority after starting all!!!
		BOOST_FOREACH(MipMaps::const_reference mipMap, this->mipMaps())
		{
			writeData[i].reset(new WriteData(*mipMap, writer, i == 0, (quality < 0 || quality > 100 ? Blp::defaultQuality : quality)));
			
			++i;
			
			if (mipMaps != 0 && i >= mipMaps)
				break;
		}
		
		typedef boost::thread* Thread;
		std::vector<Thread> threads(this->m_mipMaps.size(), 0);
		boost::thread_group threadGroup; // added threads are being destroyed automatically when group is being destroyed
		
		for (i = 0; i < writeData.size(); ++i)
			threads[i] = threadGroup.create_thread(boost::bind(&writeMipMapJpeg, boost::ref(*writeData[i])));
		
		threadGroup.join_all(); // wait for all threads have finished
		
		// TEST
		std::cout << "---- Operation has finished with time: " << operationTimer.elapsed() << "s." << std::endl;
		
		// continue, check for errors during thread operations
		i = 0;
		
		BOOST_FOREACH(const WriteDataType &writer, writeData)
		{
			if (writer->state == State::Failed)
				throw Exception(boost::format(_("Error during write process of MIP map %1%.")) % i);
			
			++i;
		}
		
		if (writeData[0]->headerSize == 0)
			throw Exception("Header size is 0.");
		
		// Get header size from the first MIP map by JPEG markers.
		const dword jpegHeaderSize = boost::numeric_cast<dword>(writeData[0]->headerSize);
		
		if (jpegHeaderSize != 624) // usual size of headers of Blizzard BLPs
			std::cerr << boost::format(_("Warning: JPEG (JFIF) header size is not equal to 624 which is the usual size of Blizzard's JPEG compressed BLPs. It is %1%.")) % jpegHeaderSize << std::endl;
		
		i = 0;
		dword offset = startOffset + jpegHeaderSize + sizeof(jpegHeaderSize);
		
		BOOST_FOREACH(const WriteDataType &writer, writeData)
		{
			// assign MIP map header data for later writing
			const dword mSize = writer->dataSize - writer->headerSize;
			offsets[i] = offset;
			sizes[i] = mSize;
			offset += mSize;
			++i;
		}
		
		wc3lib::write(ostream, jpegHeaderSize, size);		
		wc3lib::write(ostream, writeData[0]->data[0], size, writeData[0]->headerSize);
		
		i = 0;

		// write MIP map data
		BOOST_FOREACH(const WriteDataType &writer, writeData)
		{
			wc3lib::write(ostream, writer->data[writer->headerSize], size, writer->dataSize - writer->headerSize);
			
			++i;
		}
	}
	else if (this->compression() == Blp::Compression::Paletted)
	{
		// write palette, palette has always size of Blp::compressedPaletteSize (remaining colors have value 0).
		if (format() != Blp::Format::Blp2)
			wc3lib::write(ostream, palette()[0], size, Blp::compressedPaletteSize);

		std::size_t index = 0;

		// write mip maps
		BOOST_FOREACH(const MipMapPtr mipMap, this->mipMaps())
		{
			dword mipMapOffset = ostream.tellp();
			dword mipMapSize = size;

			for (dword height = 0; height < mipMap->height(); ++height)
			{
				for (dword width = 0; width < mipMap->width(); ++width)
				{
					byte index = mipMap->colorAt(width, height).paletteIndex();
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
				for (dword height = 0; height < mipMap->height(); ++height)
				{
					for (dword width = 0; width < mipMap->width(); ++width)
					{
						byte alpha = mipMap->colorAt(width, height).alpha();
						wc3lib::write(ostream, alpha, size);
					}
				}
			}

			// set MIP map header data
			mipMapSize = size - mipMapSize;

			offsets[index] = mipMapOffset;
			sizes[index] = mipMapSize;

			++index;
			
			if (mipMaps != 0 && index >= mipMaps)
				break;
		}
	}
	else
		throw Exception(boost::format(_("Unknown compression mode: %1%.")) % (dword)this->compression());

	// write MIP map header data, jump to header
	std::streampos position = ostream.tellp();
	ostream.seekp(startOffset);
	
	for (std::size_t index = 0; index < (mipMaps == 0 ? this->mipMaps().size() : mipMaps); ++index)
	{
		std::streamsize tmpSize = 0;
		// For BLP2 we would count size at twice since it has been in header already.
		wc3lib::write(ostream, offsets[index], format() == Blp::Format::Blp2 ? tmpSize : size);
		
	}
	
	// skip remaining offsets
	const std::size_t remaining = Blp::maxMipMaps - (mipMaps == 0 ? this->mipMaps().size() : mipMaps);
	ostream.seekp(remaining * sizeof(dword), std::ios::cur);
	
	for (std::size_t index = 0; index < (mipMaps == 0 ? this->mipMaps().size() : mipMaps); ++index)
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
	number = std::max<std::size_t>(number, 1);
	number = std::min<std::size_t>(number, Blp::maxMipMaps);
	
	if (number < mipMaps().size())
	{
		const int result = number - mipMaps().size();
		std::size_t i = mipMaps().size() - number;
		
		do
		{
			m_mipMaps.back().reset();
			m_mipMaps.erase(--m_mipMaps.end());
			--i;
		}
		while (i > 0);
		
		return result;
	}
	else if (number > mipMaps().size())
	{
		dword width = mipMaps().front()->width();
		dword height = mipMaps().front()->height();
		//std::list<byte> indexList = initialMipMap->m_indexList;
		//std::list<byte> alphaList = initialMipMap->m_alphaList;
		const int result = number - mipMaps().size();

		for (std::size_t i = mipMaps().size(); i < number; ++i)
		{
			
			MipMapPtr mipMap(new MipMap(this, width, height));
			/// @todo Generate new scaled index and alpha list.


			this->m_mipMaps[i] = mipMap;
			width /= 2;
			height /= 2;
		}
		
		return result;
	}

	return 0;
}

Blp::ColorPtr Blp::generatePalette(std::size_t number) throw (Exception)
{
	if (!hasPalette())
		throw Exception(_("BLP has no palette."));
	
	if (number > Blp::compressedPaletteSize)
	{
		std::cerr << boost::format(_("Invalid number of palette entries %1%. Maximum number can be %2%.")) % number % Blp::compressedPaletteSize << std::endl;
		number = Blp::compressedPaletteSize;
	}
	else if (number == 0)
		number = Blp::compressedPaletteSize;
	
	ColorPtr palette(new color[number]);
	memset(reinterpret_cast<void*>(palette.get()), 0, number);

	BOOST_FOREACH(Blp::MipMap::MapEntryType entry, this->mipMaps().front()->colors())
	{
		if (entry.second.paletteIndex() < number)
			palette[entry.second.paletteIndex()] = entry.second.argb();
	}
	
	return palette;
}

const Blp::ColorPtr& Blp::palette() const throw (Exception)
{
	return const_cast<Blp*>(this)->palette();
}

Blp::ColorPtr& Blp::palette() throw (Exception)
{
	if (!hasPalette())
		throw Exception(_("BLP has no palette."));
	
	if (m_palette.get() == 0)
	{
		ColorPtr palette = this->generatePalette();
		this->m_palette.swap(palette);
	}
	
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
