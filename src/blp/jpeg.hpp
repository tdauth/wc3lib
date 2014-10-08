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

#ifndef WC3LIB_BLP_JPEG_HPP
#define WC3LIB_BLP_JPEG_HPP

#include "platform.hpp"
#include "blp.hpp"

namespace wc3lib
{

namespace blp
{

/**
 * Searches for \p marker in \p buffer with size \p bufferSize.
 *
 * \param marker The one byte long JPEG marker (see JPEG specification for more information).
 * \param buffer The buffer in which is searched for the marker.
 * \param bufferSize The size of \p buffer.
 *
 * \return Returns true if \p marker has been found in \p buffer.
 */
bool hasJpegMarker(const byte marker, const unsigned char *buffer, const std::size_t bufferSize);

/**
 * Searches for specified marker \p marker in buffer \p buffer. When found it writes marker data of size \p markerSize (if \p variableMarkerSize is true it ignores \p markerSize and reads size after marker from buffer) into output stream \p ostream.
 * \param variableMarkerSize If this value is true you do not have to define \p markerSize since marker size is read immediately after the marker's indicating byte (size of \ref word).
 * \param markerSize Includes size bytes!
 * \param marker Marker's indicating byte value (e. g. 0xD8 for start of image).
 * \param multiple If this value is true there can be multiple markers and all are written into \p ostream. For example markers C4 and DB occur multiple times.
 * \return Returns true if marker was found and data has been written into output stream (no stream checks!).
 * \throw Exception Throws an exception if there is not enough data in buffer.
 * <a href="https://secure.wikimedia.org/wikipedia/en/wiki/JPEG#Syntax_and_structure">Source</a>
 * \todo (from Wikipedia) Within the entropy-coded data, after any 0xFF byte, a 0x00 byte is inserted by the encoder before the next byte, so that there does not appear to be a marker where none is intended, preventing framing errors. Decoders must skip this 0x00 byte. This technique, called byte stuffing (see JPEG specification section F.1.2.3), is only applied to the entropy-coded data, not to marker payload data.
 */
bool writeJpegMarkerFromBufferToStream(ostream &ostream, std::streamsize &size, bool variableMarkerSize, word markerSize, const byte marker, const unsigned char *buffer, const std::size_t bufferSize, bool multiple);

/**
 * Searches for a start of scan (0xDA) and writes data until an end of image (0xD9) is found into \p ostream.
 */
bool writeJpegScanFromBufferToStream(ostream &ostream, std::streamsize &size, const unsigned char *buffer, const std::size_t bufferSize);

/**
 * Searches for any SOF and writes all data which is NOT shared between MIP maps without the SOF marker into \p ostream.
 * This data usually is everything except for the 0xFF + marker.
 * Writes the actual 0xFF + marker into \p marker.
 *
 * \return Returns true if the marker has been found and the data has been written into \p ostream. Otherwise it returns false.
 */
bool writeJpegStartOfFrameDataFromBufferToStream(ostream &ostream, std::streamsize &size, const unsigned char *buffer, const std::size_t bufferSize, word &marker);


/**
 * Reads JPEG MIP map data from \p buffer of size \p bufferSize into \p mipMap.
 *
 * \param mipMap The MIP map which is filled with the data.
 * \param buffer The buffer which is read from.
 * \param bufferSize The size of the buffer.
 *
 * \throws Exception Throws an exception if any error occurs.
 */
void readMipMapJpeg(Blp::MipMap &mipMap, byte *buffer, dword bufferSize);

/**
 * See section "Abbreviated datastreams and multiple images" in
 * https://www.opensource.apple.com/source/tcl/tcl-87/tcl_ext/tkimg/tkimg/libjpeg/libjpeg.doc
 * or
 * /usr/share/doc/libjpeg-turbo-1.3.0-r3/libjpeg.txt.bz2
 * All MIP maps are abbreviated. The shared header is placed before the MIP map JPEG images in the data stream.
 *
 * \param mipMap The MIP map which is written into the buffer.
 * \param buffer The buffer in which is written into. It is allocated during the function. Therefore a reference must be passed which is then set to the allocated buffer.
 * \param bufferSize The size of the buffer which is set during the function. Therefore a reference must be passed which is then set to the size of the allocated buffer.
 * \param quality The JPEG image quality for the written JPEG data. Must be a value from 0 to 100.
 *
 * \throws Exception Throws an exception if any error occurs.
 */
void writeMipMapJpeg(const Blp::MipMap &mipMap, unsigned char *&buffer, unsigned long &bufferSize, int quality);

}

}

#endif
