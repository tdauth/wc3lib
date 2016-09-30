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

#define BOOST_TEST_MODULE JpegTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <jpeglib.h>

#include "../blp.hpp"
#include "../jpeg.hpp"

#if not defined(BOOST_TEST_DYN_LINK) and not defined(WINDOWS)
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::blp;

/*
 * To create a tables-only file, perform the same parameter setup that you
normally would, but instead of calling jpeg_start_compress() and so on, call
jpeg_write_tables(&cinfo).  This will write an abbreviated datastream
containing only SOI, DQT and/or DHT markers, and EOI.  All the quantization
and Huffman tables that are currently defined in the compression object will
be emitted unless their sent_tables flag is already TRUE, and then all the
sent_tables flags will be set TRUE.
*/
BOOST_AUTO_TEST_CASE(WriteHeader)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	unsigned char *buffer = 0;
	unsigned long int bufferSize = 0;
	bool success = true;

	try
	{
		const JDIMENSION imageWidth = boost::numeric_cast<JDIMENSION>(64);
		const JDIMENSION imageHeight = boost::numeric_cast<JDIMENSION>(64);
		const int inputComponents = 3; // TODO ARGB, 4

		jpeg_mem_dest(&cinfo, &buffer, &bufferSize);
		cinfo.image_width = imageWidth;
		cinfo.image_height = imageHeight;
		cinfo.input_components = inputComponents;
		cinfo.in_color_space = JCS_RGB;
		cinfo.write_JFIF_header = true;

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, 100, false);
		//  This will write an abbreviated datastream containing only SOI, DQT and/or DHT markers, and EOI
		jpeg_write_tables(&cinfo);
		//jpeg_start_compress(&cinfo, true);
	}
	catch (std::exception &e)
	{
		success = false;
		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(success);
	std::cerr << "Header size: " << bufferSize << std::endl;
	BOOST_CHECK(hasJpegMarker(0xD8, buffer, bufferSize));
	// no JFIF marker and no comment!
	//BOOST_CHECK(hasJpegMarker(0xE0, buffer, bufferSize));
	//BOOST_CHECK(hasJpegMarker(0xFE, buffer, bufferSize));
	BOOST_CHECK(hasJpegMarker(0xDB, buffer, bufferSize));
	BOOST_CHECK(hasJpegMarker(0xC4, buffer, bufferSize));
	BOOST_CHECK(bufferSize == 624);
}

BOOST_AUTO_TEST_CASE(WriteHeader2)
{
	std::cerr << "Size of BLP1 header:" << sizeof(BlpHeader) << std::endl;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	unsigned char *buffer = 0;
	unsigned long int bufferSize = 0;
	bool success = true;

	try
	{
		const JDIMENSION imageWidth = boost::numeric_cast<JDIMENSION>(64);
		const JDIMENSION imageHeight = boost::numeric_cast<JDIMENSION>(64);
		const int inputComponents = 3; // TODO ARGB, 4

		jpeg_mem_dest(&cinfo, &buffer, &bufferSize);
		cinfo.image_width = imageWidth;
		cinfo.image_height = imageHeight;
		cinfo.input_components = inputComponents;
		cinfo.in_color_space = JCS_RGB;

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, 100, false);
		jpeg_write_tables(&cinfo);
		//jpeg_start_compress(&cinfo, true);
		//jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
	}
	catch (std::exception &e)
	{
		success = false;
		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(success);
	std::cerr << "Header size 1: " << bufferSize << std::endl;

	ofstream jpegheaderout("jpegheaderout", std::ios::binary | std::ios::out);
	jpegheaderout.write((const char*)buffer, bufferSize);
	jpegheaderout.close();

	ofstream ostream("tmp", std::ios::binary | std::ios::out);
	std::streamsize headerSize = 0;

	try
	{
		// skip shared header size
		std::streampos headerPosition = ostream.tellp();
		ostream.seekp(sizeof(dword), std::ios::cur);

		// NOTE marker reference: https://secure.wikimedia.org/wikipedia/en/wiki/JPEG#Syntax_and_structure
		if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, false, 0, 0xD8, buffer, bufferSize, false)) // image start
		{
			throw Exception(_("Missing image start marker 0xD8."));
		}

		if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xC4, buffer, bufferSize, true)) // huffman table
		{
			throw Exception(_("Missing huffman table marker 0xC4."));
		}

		if (!writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xDB, buffer, bufferSize, true)) // quantization table
		{
			throw Exception(_("Missing quantization table marker 0xDB."));
		}

		writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xFE, buffer, bufferSize, false); // image comment
		writeJpegMarkerFromBufferToStream(ostream, headerSize, true, 0, 0xE0, buffer, bufferSize, false); // JFIF marker

		// write shared header size and jump back again
		std::streamsize size = 0;
		writeByteCount(ostream, boost::numeric_cast<dword>(headerSize), headerPosition, size);
	}
	catch (Exception &e)
	{
		success = false;
		std::cerr << e.what() << std::endl;
	}

	BOOST_REQUIRE(success);
	std::cerr << "Header size 2: " << headerSize << std::endl;
	BOOST_CHECK(headerSize == 624);
	ostream.close();

	/*
	 * Now compare the header from the jpeglib to the customly created header.
	 */
	ifstream istream("tmp", std::ios::binary | std::ios::in);

	BOOST_REQUIRE(istream);

	unsigned char buffer2[headerSize - 2];
	memset(buffer2, 0, sizeof(buffer2));
	istream.seekg(sizeof(dword), std::ios::cur); // skip header count
	istream.read((char*)buffer2, headerSize);
	istream.close();

	/*
	 * Compare all bytes. If one is not in the other one print in in the error stream in hex format.
	 */
	const std::size_t max = std::max(sizeof(buffer2), bufferSize);

	for (std::size_t i = 0; i < max; ++i)
	{
		if (i >= sizeof(buffer2))
		{
			std::cerr << std::hex << "Byte in jpeglib header but not in custom: " << (int)buffer[i] << std::endl;
		}

		if (i >= bufferSize)
		{
			std::cerr << std::hex << "Byte in custom header but not in jpeg: " << (unsigned int)buffer2[i] << std::endl;
		}
	}

	std::cerr.unsetf(std::ios::hex);
}
