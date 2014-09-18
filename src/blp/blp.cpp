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

#include <cstdlib>
#include <cstdio>

#include <jpeglib.h>

#include "blp.hpp"
#include "jpeg.hpp"

namespace wc3lib
{

namespace blp
{

const std::size_t Blp::maxMipMaps = 16;
const std::size_t Blp::compressedPaletteSize = 256;
const int Blp::defaultQuality = 100;
const std::size_t Blp::defaultMipMaps = 0;
const bool Blp::defaultSharedHeader = true;

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
	{
		throw Exception(boost::format(_("Error while reading BLP file. BLP identifier is too short: %1%. Expected size of %2%.")) % bufferSize % sizeof(dword));
	}

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
	{
		return false;
	}

	/*
	 * TODO FIXME
	static const dword identifier0 = Blp::Format::Blp0;
	static const dword identifier1 = Blp::Format::Blp1;
	static const dword identifier2 = Blp::Format::Blp2;
	*/

	if (memcmp(buffer, "BLP0", sizeof(dword)) == 0)
	{
		return true;
	}
	else if (memcmp(buffer, "BLP1", sizeof(dword)) == 0)
	{
		return true;
	}
	else if (memcmp(buffer, "BLP2", sizeof(dword)) == 0)
	{
		return true;
	}

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
		{
			this->m_compression = Blp::Compression::Jpeg;
		}
		else if (header.type == 1)
		{
			if (header.encoding == 1)
			{
				this->m_compression = Blp::Compression::Uncompressed;
			}
			else if (header.encoding == 2)
			{
				this->m_compression = Blp::Compression::DirectXCompression;
			}
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
		{
			//header.palette;
			;
		}

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

			boost::scoped_array<byte> jpegHeader;

			/*
			 * There might be no shared header.
			 * In this case the whole header is stored in the MIP map data.
			 */
			if (jpegHeaderSize > 0)
			{
				jpegHeader.reset(new byte[jpegHeaderSize]);
				wc3lib::read(istream, jpegHeader[0], size, jpegHeaderSize * sizeof(byte));
			}

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

				if (jpegHeaderSize > 0)
				{
					memcpy(buffer.get(), jpegHeader.get(), jpegHeaderSize);
				}

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

std::streamsize Blp::write(OutputStream &ostream, int quality, std::size_t mipMaps, bool sharedHeader) const
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
		{
			wc3lib::write(ostream, "BLP0", size, 4);

			break;
		}

		case Blp::Format::Blp1:
		{
			wc3lib::write(ostream, "BLP1", size, 4);

			break;
		}

		case Blp::Format::Blp2:
		{
			wc3lib::write(ostream, "BLP2", size, 4);

			break;
		}
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
		{
			header.type = 0;
		}
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

				/*
				 * Write the JPEG MIP map into a temporary buffer and store the header size of the buffer.
				 * The header should only be written for the first MIP map.
				 */
				writeMipMapJpeg(this->mipMaps()[i], tmpBuffer, bufferSize, quality);

				/*
				 * Make sure buffer is freed on leaving the scope to avoid memory leaks.
				 */
				boost::scoped_array<unsigned char> buffer;
				buffer.reset(tmpBuffer);

				std::streamsize mipMapSize = 0;

				if (sharedHeader)
				{
					std::streamsize sstreamSize = 0;
					stringstream sstream;
					// stores the 0xFFC<number> marker which indicates the start of a frame
					word startOfFrameMarker = 0;

					// TODO image is bigger than from Warcraft III it self (more data in shared header?!)
					// start of frame must be in the header as well
					// but only the 0xFFC<number> marker since the rest cannot be shared and contains information such as the size etc.
					if (!writeJpegStartOfFrameDataFromBufferToStream(sstream, sstreamSize, buffer.get(), bufferSize, startOfFrameMarker))
					{
						throw Exception(_("Missing image start marker 0xC0."));
					}

					/*
					 * Validate the format of the marker.
					 */
					if ((startOfFrameMarker >> 4) != 0xFFC)
					{
						throw Exception(boost::format(_("Invalid start of frame marker %1%.")) % startOfFrameMarker);
					}

					/*
					 * Write start of frame into buffer which can be used later for the MIP map data.
					 */
					const string startOfFrameString = sstream.str();

					const std::size_t startOfFrameSize = endPosition(sstream); // - (std::streamoff)(startOfFramePerMipMapSize);
					boost::scoped_array<byte> startOfFrame(new byte[startOfFrameSize]);
					memcpy(startOfFrame.get(), startOfFrameString.c_str(), startOfFrameSize);

					/*
					 * Write header once for the first MIP map.
					 */
					if (i == 0)
					{
						// skip shared header size
						std::streamsize headerSize = 0;
						const std::streampos headerPosition = ostream.tellp();
						ostream.seekp(sizeof(dword), std::ios::cur);

						if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, false, 0, 0xD8, buffer.get(), bufferSize, false))
						{
							throw Exception(_("Missing image start marker 0xD8."));
						}

						// huffman table
						if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xC4, buffer.get(), bufferSize, true))
						{
							throw Exception(_("Missing image start marker 0xC4."));
						}

						// quantization table
						if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xDB, buffer.get(), bufferSize, true))
						{
							throw Exception(_("Missing image start marker 0xDB."));
						}

						// optional image comment
						writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xFE, buffer.get(), bufferSize, false);
						// JFIF marker
						writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xE0, buffer.get(), bufferSize, false);

						/*
						 * Make sure the order of the two bytes is correct.
						 */
						wc3lib::write<byte>(ostream, (startOfFrameMarker >> 8), headerSize);
						wc3lib::write<byte>(ostream, startOfFrameMarker, headerSize);
						std::cerr << "Header size after: " << headerSize << std::endl;

						// write shared header size and jump back again
						writeByteCount(ostream, boost::numeric_cast<dword>(headerSize), headerPosition, size);
					}

					/*
					 * Set the MIP map offset to the start of the MIP map which starts either after the last MIP map or after the shared header.
					 */
					mipMapHeaders[i].offset = boost::numeric_cast<dword>(OutputStream::off_type(ostream.tellp()));

					/*
					 * Now write the full start of frame except for the marker which can be shared.
					 */
					wc3lib::write(ostream, startOfFrame[0], mipMapSize, startOfFrameSize);

					// Start of Scan
					if (!writeJpegScanFromBufferToStream(ostream, mipMapSize, buffer.get(), bufferSize))
					{
						throw Exception(_("Missing image start marker 0xDA."));
					}
				}
				else
				{
					/*
					 * Shared header size is 0.
					 */
					if (i == 0)
					{
						std::cerr << "Offset before: " << boost::numeric_cast<dword>(OutputStream::off_type(ostream.tellp())) << std::endl;
						wc3lib::write<dword>(ostream, 0, size);
						std::cerr << "Offset after: " << boost::numeric_cast<dword>(OutputStream::off_type(ostream.tellp())) << std::endl;
					}

					mipMapHeaders[i].offset = boost::numeric_cast<dword>(OutputStream::off_type(ostream.tellp()));

					/*
					 * Write the whole buffer into the stream.
					 * The MIP map has its own JPEG header now.
					 */
					wc3lib::write(ostream, buffer[0], mipMapSize, bufferSize);
				}

				mipMapHeaders[i].size = boost::numeric_cast<dword>(mipMapSize);
				size += mipMapSize;
			}

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
	{
		std::cerr << boost::format(_("MIP map number %1% is bigger than maximum %2%")) % number % Blp::maxMipMaps << std::endl;
	}

	number = std::max<std::size_t>(number, 1);
	number = std::min<std::size_t>(number, Blp::maxMipMaps);

	if (regenerate)
	{
		m_mipMaps.clear();
	}

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
	{
		throw Exception(_("BLP has no palette."));
	}

	return m_palette;
}

dword Blp::mipMapWidth(std::size_t index) const
{
	dword width = this->m_width;

	for (std::size_t i = 0; i < index; ++i)
	{
		width /= 2;
	}

	return width;
}

dword Blp::mipMapHeight(std::size_t index) const
{
	dword height = this->m_height;

	for (std::size_t i = 0; i < index; ++i)
	{
		height /= 2;
	}

	return height;
}

}

}
