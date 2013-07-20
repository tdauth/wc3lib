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
		arraystream stream;
		std::streamsize size = mpqFile->writeData(stream);
		byte *buffer;
		stream >> buffer; // TODO don't copy (check if this makes a copy)!

		if (this->extendedAttributes() & ExtendedAttributes::FileCrc32s)
			;
			// FIXME fix the line below
			//setCrc32(mpqFile, boost::crc<boost::crc_32_type>((void*)buffer, size));

		if (this->extendedAttributes() & ExtendedAttributes::FileMd5s)
			setMd5(mpqFile->block(), mpq::md5(buffer));
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
	setExtendedAttributes(BOOST_SCOPED_ENUM(ExtendedAttributes)(extendedAttributesHeader.attributesPresent));

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
	extendedAttributesHeader.attributesPresent = extendedAttributes();
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

	arraystream stream;
	std::streamsize size = mpqFile->writeData(stream);
	byte *buffer;
	stream >> buffer; // TODO don't copy (check if this makes a copy)!

	if (extendedAttributes() & ExtendedAttributes::FileCrc32s)
	{
		// FIXME fix the lines below
		//if (boost::crc<boost::crc_32_type>(buffer, size) != this->crc32(mpqFile))
			//return false;
	}

	if (extendedAttributes() & ExtendedAttributes::FileMd5s)
	{
		if (mpq::md5(buffer) != this->md5(mpqFile))
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

Attributes::Attributes(Mpq *mpq, Hash *hash, BOOST_SCOPED_ENUM(ExtendedAttributes) extendedAttributes, uint32 version) : m_extendedAttributes(extendedAttributes), m_version(version), MpqFile(mpq, hash)
{
	this->m_path = fileName();
}

}

}
