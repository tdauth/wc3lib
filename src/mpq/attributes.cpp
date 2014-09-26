/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include <boost/crc.hpp>

#include "attributes.hpp"
#include "mpq.hpp"
#include "block.hpp"
#include "platform.hpp"
#include "algorithm.hpp"

namespace wc3lib
{

namespace mpq
{

CRC32 Attributes::crc32(const byte* data, std::size_t dataSize)
{
	boost::crc_32_type result; // TODO get correct CRC paramaters for MPQ (specification?)

	result.process_bytes((const void*)data, dataSize);

	return result.checksum();
}

MD5Checksum Attributes::md5(const byte* data, std::size_t dataSize)
{
	return mpq::md5(data, dataSize);
}

bool Attributes::checkCrc(const byte *data, std::size_t dataSize, CRC32 crc)
{
	// TODO implement CRC check

	return false;
}

bool Attributes::checkMd5(const byte *data, std::size_t dataSize, MD5Checksum md5)
{
	return mpq::md5(data, dataSize) == md5;
}

Attributes::Attributes() : File()
{
}

void Attributes::removeData()
{
	this->writeAttributes(latestVersion, ExtendedAttributes::None); // write empty data
}

std::streamsize Attributes::attributes(int32 &version, ExtendedAttributes &extendedAttributes, Crc32s &crcs, FileTimes &fileTimes, Md5s &md5s)
{
	/*
	boost::scoped_array<byte> data(new byte[this->size()]); // TODO works with buffered stream stringstream but not with unbuffered?
	arraystream stream(data.get(), this->size());
	*/
	stringstream stream;
	std::streamsize size = 0;
	size = File::writeData(stream);

	ExtendedAttributesHeader extendedAttributesHeader;
	wc3lib::read(stream, extendedAttributesHeader, size);
	version = extendedAttributesHeader.version;
	extendedAttributes = static_cast<ExtendedAttributes>(extendedAttributesHeader.attributesPresent);

	if (extendedAttributes & ExtendedAttributes::FileCrc32s)
	{
		crcs.resize(this->mpq()->blocks().size());

		BOOST_FOREACH(const Block &block, this->mpq()->blocks())
		{
			CRC32 crc32;
			wc3lib::read(stream, crc32, size);
			crcs[block.index()] = crc32;
		}
	}

	if (extendedAttributes & ExtendedAttributes::FileTimeStamps)
	{
		fileTimes.resize(this->mpq()->blocks().size());

		BOOST_FOREACH(const Block &block, this->mpq()->blocks())
		{
			FILETIME fileTime;
			wc3lib::read(stream, fileTime, size);
			fileTimes[block.index()] = fileTime;
		}
	}

	if (extendedAttributes & ExtendedAttributes::FileMd5s)
	{
		md5s.resize(this->mpq()->blocks().size());

		BOOST_FOREACH(const Block &block, this->mpq()->blocks())
		{
			MD5Checksum md5;
			wc3lib::read(stream, md5, size);
			md5s[block.index()] = md5;
		}
	}

	return size;
}

std::streamsize Attributes::writeAttributes(int32 version, ExtendedAttributes extenedAttributes, const Crc32s &crcs, const FileTimes &fileTimes, const Md5s &md5s)
{
	struct ExtendedAttributesHeader extendedAttributesHeader;
	extendedAttributesHeader.version = version;
	extendedAttributesHeader.attributesPresent = static_cast<uint32>(extenedAttributes);
	arraystream stream;
	std::streamsize size = 0;
	wc3lib::write(stream, extendedAttributesHeader, size);

	if (extenedAttributes & ExtendedAttributes::FileCrc32s)
	{
		BOOST_FOREACH(Crc32s::const_reference ref, crcs)
		{
			wc3lib::write(stream, ref, size);
		}
	}

	if (extenedAttributes & ExtendedAttributes::FileTimeStamps)
	{
		BOOST_FOREACH(FileTimes::const_reference ref, fileTimes)
		{
			wc3lib::write(stream, ref, size);
		}
	}

	if (extenedAttributes & ExtendedAttributes::FileMd5s)
	{
		BOOST_FOREACH(Md5s::const_reference ref, md5s)
		{
			wc3lib::write(stream, ref, size);
		}
	}

	return File::readData(stream);
}

Attributes::Attributes(Archive *mpq, Hash *hash) : File(mpq, hash, "(attributes)")
{
}

}

}
