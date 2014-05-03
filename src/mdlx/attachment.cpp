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

#include "attachment.hpp"
#include "attachmentvisibilities.hpp"
#include "../i18n.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

Attachment::Attachment(class Attachments *attachments) : Object(attachments->mdlx()), GroupMdxBlockMember(attachments, "Attachment"), m_visibilities(new AttachmentVisibilities(attachments->mdlx()))
{
}

Attachment::~Attachment()
{
	delete this->m_visibilities;
}

std::streamsize Attachment::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Attachment::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Attachment", this->name(), 0, true);

	size += Object::writeMdl(ostream);

	writeMdlValueProperty(ostream, size, "AttachmentID", this->attachmentId());

	if (strlen(this->path()) > 0)
	{
		writeMdlValueProperty(ostream, size, "Path", this->path());
	}

	size += visibilities()->writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}


std::streamsize Attachment::readMdx(istream &istream) throw (class Exception)
{
	long32 nbytesi = 0;
	std::streamsize size = 0;
	wc3lib::read(istream, nbytesi, size);
	size += Object::readMdx(istream);
	wc3lib::read(istream, this->m_path, size, pathSize);
	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, this->m_attachmentId, size);
	size += this->visibilities()->readMdx(istream);

	checkBytesIncluding(size, nbytesi);

	return size;
}

std::streamsize Attachment::writeMdx(ostream &out) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(out, position);

	std::streamsize size = Object::writeMdx(out);
	wc3lib::write(out, this->path(), size, pathSize);
	wc3lib::write(out, this->unknown0(), size);
	wc3lib::write(out, this->attachmentId(), size);
	size += this->visibilities()->writeMdx(out);

	const long32 nbytesi = boost::numeric_cast<long32>(size);
	writeByteCount(out, nbytesi, position, size, true);

	return size;
}

}

}
