/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include "jpeg.hpp"

namespace wc3lib
{

namespace blp
{

bool hasJpegMarker(const blp::byte marker, const unsigned char* buffer, const std::size_t bufferSize)
{
	for (std::size_t i = 0; i < bufferSize; ++i)
	{
		if (buffer[i] == 0xFF)
		{
			std::size_t j = i + 1;

			if (j >= bufferSize)
			{
				return false;
			}

			if (buffer[j] == marker)
			{
				return true;
			}
		}
	}

	return false;
}

bool writeJpegMarkerFromBufferToStream(ostream &ostream, std::streamsize &size, bool variableMarkerSize, word markerSize, const byte marker, const unsigned char *buffer, const std::size_t bufferSize, bool multiple)
{
	if (marker == 0x00)
	{
		throw Exception(_("0x00 marker is invalid! This value must be safed for 0xFF bytes in encoded data!"));
	}

	bool found = false;

	for (std::size_t i = 0; i < bufferSize && (!found || multiple); ++i)
	{
		if (buffer[i] == 0xFF)
		{
			std::size_t j = i + 1;

			if (j < bufferSize && buffer[j] == marker)
			{
				if (variableMarkerSize || markerSize > 0)
				{
					++j;

					if (j + 1 > bufferSize) // we need 2 bytes for marker size (which could be 0 as well)
					{
						throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data to get its own data size.")) % hexValue(marker));
					}

					if (variableMarkerSize)
					{
						//memcpy(&markerSize, &(buffer[j]), sizeof(markerSize));
						markerSize = (word(buffer[j]) << 8) + word(buffer[j + 1]); // stored as little endian
					}
				}

				// 0xFF + marker + marker size
				if (i + 2 + markerSize > bufferSize)
				{
					throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data with marker size %2% and data size %3%, starting at %4%.")) % hexValue(marker) % markerSize % bufferSize % (i + 2));
				}

				// marker size is 2 bytes long and includes its own size!
				wc3lib::write(ostream, buffer[i], size, markerSize + 2); // + sizeof 0xFF and marker

				found = true;
			}
		}
	}

	return found;
}

bool writeJpegScanFromBufferToStream(ostream& ostream, std::streamsize& size, const unsigned char* buffer, const std::size_t bufferSize)
{
	bool found = false;
	const byte marker = 0xDA;

	for (std::size_t i = 0; i < bufferSize && !found; ++i)
	{
		if (buffer[i] == 0xFF)
		{
			std::size_t j = i + 1;

			if (j < bufferSize && buffer[j] == marker)
			{
				++j;

				if (j + 1 > bufferSize) // we need 2 bytes for marker size (which could be 0 as well)
				{
					throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data to get its own data size.")) % hexValue(marker));
				}
				//memcpy(&markerSize, &(buffer[j]), sizeof(markerSize));
				const word markerSize = (word(buffer[j]) << 8) + word(buffer[j + 1]); // stored as little endian

				// 0xFF + marker + marker size
				if (i + 2 + markerSize > bufferSize)
				{
					throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data with marker size %2% and data size %3%, starting at %4%.")) % hexValue(marker) % markerSize % bufferSize % (i + 2));
				}


				/*
				 * Now find ending marker 0xD9.
				 */
				std::size_t dataSize = 0;
				bool foundEndingMarker = false;

				for (std::size_t k = j + 1; k < bufferSize && !foundEndingMarker; ++k)
				{
					if (buffer[k] == 0xFF)
					{
						if (k + 1 >= bufferSize)
						{
							throw Exception();
						}

						k++;

						if (buffer[k] == 0xD9)
						{
							dataSize = (k - i) + 1; // + sizeof 0xFF and marker + 2 bytes variable size
							foundEndingMarker = true;
						}
					}
				}

				if (foundEndingMarker)
				{
					wc3lib::write(ostream, buffer[i], size, dataSize);

					found = true;
				}
			}
		}
	}

	return found;
}

bool writeJpegStartOfFrameDataFromBufferToStream(ostream& ostream, std::streamsize &size, const unsigned char *buffer, const std::size_t bufferSize, word &marker)
{
	bool found = false;
	/*
	 * From the JPEG specification:
	 * B.2.2 Frame header syntax
	 */
	const byte markers[] =
	{
		0xC0,
		0xC1,
		0xC2,
		0xC3,
		0xC9,
		0xCA,
		0xCB

	};

	for (std::size_t i = 0; i < bufferSize && !found; ++i)
	{
		if (buffer[i] == 0xFF)
		{
			std::size_t j = i + 1;

			if (j < bufferSize)
			{
				bool isMarker = false;

				for (std::size_t index = 0; index < sizeof(markers) && !isMarker; ++index)
				{
					if (buffer[j] == markers[index])
					{
						isMarker = true;
					}
				}

				if (isMarker)
				{
					// stored as little endian
					const word actualMarker = (0xFF << 8) | buffer[j];

					++j;

					if (j + 1 > bufferSize) // we need 2 bytes for marker size (which could be 0 as well)
					{
						throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data to get its own data size.")) % hexValue(marker));
					}
					//memcpy(&markerSize, &(buffer[j]), sizeof(markerSize));
					const word markerSize = (word(buffer[j]) << 8) + word(buffer[j + 1]); // stored as little endian

					// 0xFF + marker + marker size
					if (i + 2 + markerSize > bufferSize)
					{
						throw Exception(boost::format(_("JPEG marker \"%1%\" needs more data with marker size %2% and data size %3%, starting at %4%.")) % hexValue(marker) % markerSize % bufferSize % (i + 2));
					}

					// marker size is 2 bytes long and includes its own size!
					// write marker size + data and leave marker away
					wc3lib::write(ostream, buffer[j], size, markerSize); // write data without marker since marker is stored into marker
					marker = actualMarker;

					found = true;
				}
			}
		}
	}

	return found;
}

std::string jpegError(const std::string &message)
{
	return boost::str(boost::format(message) % (jpeg_std_error(0)->jpeg_message_table != 0 ? jpeg_std_error(0)->jpeg_message_table[jpeg_std_error(0)->last_jpeg_message] : _("No error")));
}

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

		// BLP uses color space 4 but actually it is bgra
		if (cinfo.out_color_space != JCS_CMYK)
		{
			std::cerr << boost::format(_("Warning: Image color space (%1%) is not equal to CMYK (%2%) - actually BGRA.")) % cinfo.out_color_space % JCS_CMYK << std::endl;
		}

		/// \todo Get as much required scanlines as possible (highest divident) to increase speed. Actually it could be equal to the MIP maps height which will lead to reading the whole MIP map with one single jpeg_read_scanlines call
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
			{
				// TODO occurs all the time, too much output
				//std::cerr << boost::format(_("Number of scanned lines is not equal to %1%. It is %2%.")) % requiredScanlines % dimension << std::endl;
			}

			for (dword height = 0; height < dimension; ++height)
			{
				dword width = 0;

				// cinfo.output_components should be 3 if RGB and 4 if RGBA
				for (JDIMENSION component = 0; component < scanlineSize; component += cinfo.output_components)
				{
					// convert BGRA to RGBA
					// Red and Blue colors are swapped.
					// http://www.wc3c.net/showpost.php?p=1046264&postcount=2
					color rgba = ((color)scanlines[height][component] << 8) | ((color)scanlines[height][component + 1] << 16) | ((color)scanlines[height][component + 2] << 24);

					if (cinfo.output_components == 4) // we do have an alpha channel
					{
						rgba |= ((color)(scanlines[height][component + 3]));
					}

					mipMap.setColor(width, height + currentScanline, rgba);
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

		throw;
	}

	jpeg_finish_decompress(&cinfo);

	if (!cinfo.saw_JFIF_marker)
	{
		std::cerr << boost::format(_("Warning: Did not find JFIF marker. JFIF format is used by default!\nThis is the JFIF version of the image %1%.%2%")) % cinfo.JFIF_major_version % cinfo.JFIF_minor_version << std::endl;
	}

	jpeg_destroy_decompress(&cinfo);
}

void writeMipMapJpeg(const Blp::MipMap &mipMap, unsigned char *&buffer, unsigned long &bufferSize, int quality)
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
		const int inputComponents = 4; // actually stored as ABGR, 4 components
		/// \todo Get as much required scanlines as possible (highest divident) to increase speed. Actually it could be equal to the MIP maps height which will lead to reading the whole MIP map with one single \ref jpeg_write_scanlines call
		const JDIMENSION requiredScanlines = imageHeight; // increase this value to read more scanlines in one step
		assert(requiredScanlines <= imageHeight && requiredScanlines > 0);
		const JDIMENSION scanlineSize = imageWidth * inputComponents; // JSAMPLEs per row in output buffer

		jpeg_mem_dest(&cinfo, &buffer, &bufferSize);
		cinfo.image_width = imageWidth;
		cinfo.image_height = imageHeight;
		cinfo.input_components = inputComponents;
		cinfo.in_color_space = JCS_CMYK; // BLP uses color space 4 but actually it is bgra

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, quality, false);
		//jpeg_write_tables(&cinfo);

		jpeg_start_compress(&cinfo, true);


		// TODO also share JFIF header? cinfo.write_JFIF_header = writeData->isFirst;

		scanlines = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, scanlineSize, requiredScanlines);

		while (cinfo.next_scanline < cinfo.image_height)
		{
			for (dword height = cinfo.next_scanline; height < requiredScanlines; ++height)
			{
				dword width = 0;

				// cinfo.output_components should be 3 if RGB and 4 if RGBA
				for (JDIMENSION component = 0; component < scanlineSize; component += cinfo.input_components)
				{
					// Convert RGBA to BGRA
					// Red and Blue colors are swapped.
					// http://www.wc3c.net/showpost.php?p=1046264&postcount=2
					const color rgba = mipMap.colorAt(width, height).rgba();
					scanlines[height][component] = blue(rgba);
					scanlines[height][component + 1] = green(rgba);
					scanlines[height][component + 2] = red(rgba);

					if (cinfo.input_components == 4) // we do have an alpha channel
					{
						scanlines[height][component + 3] = alpha(rgba);
					}

					++width;
				}
			}

			const JDIMENSION dimension = jpeg_write_scanlines(&cinfo, scanlines, requiredScanlines);

			if (dimension != requiredScanlines)
			{
				throw Exception(boost::format(_("Number of written scan lines is not equal to %1%. It is %2%.")) % requiredScanlines % dimension);
			}
		}
	}
	/*
	 * Cleanup before throwing exception.
	 */
	catch (std::exception &exception)
	{
		// jpeg_abort_compress is only used when cinfo has to be used again.
		jpeg_destroy_compress(&cinfo); // discard object

		throw;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
}

}

}
