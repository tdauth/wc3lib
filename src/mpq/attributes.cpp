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

MD5 Attributes::md5(const byte* data, std::size_t dataSize)
{
	return mpq::md5(data, dataSize);
}

void Attributes::removeData()
{
	this->crc32s().clear();
	this->fileTimes().clear();
	this->md5s().clear();
	MpqFile::removeData();
}

void Attributes::refreshFile(const MpqFile *mpqFile)
{
	if (this->extendedAttributes() & ExtendedAttributes::FileTimeStamps)
		setFileTime(mpqFile->block(), FILETIME(time(NULL)));

	if (this->hasChecksums())
	{
		stringstream stream;
		std::streamsize size = mpqFile->writeData(stream);
		string data = stream.str(); // TODO expensive

		if (this->extendedAttributes() & ExtendedAttributes::FileCrc32s)
			setCrc32(mpqFile->block(), crc32(data.c_str(), data.size()));

		if (this->extendedAttributes() & ExtendedAttributes::FileMd5s)
			setMd5(mpqFile->block(), md5(data.c_str(), data.size()));
	}
}

void Attributes::refresh()
{
	if (this->extendedAttributes() == ExtendedAttributes::None)
		return;

	BOOST_FOREACH(const Mpq::FilePtr &file, this->mpq()->files())
		refreshFile(file.get());
}

std::streamsize Attributes::readData()
{
	/*
	boost::scoped_array<byte> data(new byte[this->size()]); // TODO works with buffered stream stringstream but not with unbuffered?
	arraystream stream(data.get(), this->size());
	*/
	stringstream stream;
	std::streamsize size = 0;
	MpqFile::writeData(stream);

	struct ExtendedAttributesHeader extendedAttributesHeader;
	wc3lib::read(stream, extendedAttributesHeader, size);
	setVersion(extendedAttributesHeader.version);
	setExtendedAttributes(static_cast<ExtendedAttributes>(extendedAttributesHeader.attributesPresent));

	if (this->extendedAttributes() & ExtendedAttributes::FileCrc32s)
	{
		crc32s().resize(this->mpq()->blocks().size());

		BOOST_FOREACH(const Mpq::BlockPtr &block, this->mpq()->blocks())
		{
			CRC32 crc32;
			wc3lib::read(stream, crc32, size);
			setCrc32(block.get(), crc32);
		}
	}

	if (this->extendedAttributes() & ExtendedAttributes::FileTimeStamps)
	{
		fileTimes().resize(this->mpq()->blocks().size());

		BOOST_FOREACH(const Mpq::BlockPtr &block, this->mpq()->blocks())
		{
			FILETIME fileTime;
			wc3lib::read(stream, fileTime, size);
			setFileTime(block.get(), fileTime);
		}
	}

	if (this->extendedAttributes() & ExtendedAttributes::FileMd5s)
	{
		md5s().resize(this->mpq()->blocks().size());

		BOOST_FOREACH(const Mpq::BlockPtr &block, this->mpq()->blocks())
		{
			MD5 md5;
			wc3lib::read(stream, md5, size);
			setMd5(block.get(), md5);
		}
	}

	return size;
}

std::streamsize Attributes::writeData()
{
	struct ExtendedAttributesHeader extendedAttributesHeader;
	extendedAttributesHeader.version = latestVersion;
	extendedAttributesHeader.attributesPresent = static_cast<uint32>(extendedAttributes());
	arraystream stream;
	std::streamsize size = 0;
	wc3lib::write(stream, extendedAttributesHeader, size);

	if (this->extendedAttributes() & ExtendedAttributes::FileCrc32s)
	{
		BOOST_FOREACH(Crc32s::const_reference ref, this->crc32s())
			wc3lib::write(stream, ref, size);
	}

	if (this->extendedAttributes() & ExtendedAttributes::FileTimeStamps)
	{
		BOOST_FOREACH(FileTimes::const_reference ref, this->fileTimes())
			wc3lib::write(stream, ref, size);
	}

	if (this->extendedAttributes() & ExtendedAttributes::FileMd5s)
	{
		BOOST_FOREACH(Md5s::const_reference ref, this->md5s())
			wc3lib::write(stream, ref, size);
	}

	return MpqFile::readData(stream);
	//return this->addFile("(attributes)", MpqFile::Locale::Neutral, MpqFile::Platform::Default, &stream);
}

bool Attributes::check(const MpqFile *mpqFile) const
{
	if (!hasChecksums())
		return true;

	stringstream stream;
	std::streamsize size = mpqFile->writeData(stream);
	string data = stream.str(); // TODO expensive

	if (extendedAttributes() & ExtendedAttributes::FileCrc32s)
	{
		// FIXME fix the lines below
		//if (boost::crc<boost::crc_32_type>(buffer, size) != this->crc32(mpqFile))
			//return false;
	}

	if (extendedAttributes() & ExtendedAttributes::FileMd5s)
	{
		if (mpq::md5(data.c_str(), data.size()) != this->md5(mpqFile))
			return false;
	}

	return true;
}

bool Attributes::check() const
{
	if (!hasChecksums())
		return true;

	BOOST_FOREACH(const Mpq::FilePtr &file, this->mpq()->files())
	{
		if (!check(file.get()))
			return false;
	}

	return true;
}

Attributes::Attributes(Mpq *mpq, Hash *hash, ExtendedAttributes extendedAttributes, uint32 version) : m_extendedAttributes(extendedAttributes), m_version(version), MpqFile(mpq, hash)
{
	this->m_path = fileName();
}

}

}
