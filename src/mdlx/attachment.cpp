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

#include <sstream>
#include <cstring>

#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

#include "attachment.hpp"
#include "attachmentvisibilities.hpp"
#include "../internationalisation.hpp"
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
	/*
	std::string line;
	std::getline(istream, line);
	boost::tokenizer<> tokenizer(line);
	boost::tokenizer<>::iterator iterator = tokenizer.begin();

	if ((*iterator) != "Attachment")
		throw Exception("");

	++iterator;

	if (iterator == tokenizer.end() || sizeof((*iterator).c_str()) > 0x100) /// @todo Name size is bigger than in object?! path length?
		throw Exception("");

	for (std::size_t i = 0; i < sizeof(this->m_name); ++i)
		this->m_name[i] = (*iterator).c_str()[i];

	++iterator;

	if ((*iterator) != "{")
		throw Exception("");

	std::getline(istream, line);
	tokenizer.assign(line);
	iterator = tokenizer.begin();

	if ((*iterator) != "ObjectId")
		throw Exception("");

	++iterator;
	std::stringstream sstream;
	sstream << (*iterator);
	sstream >> this->m_objectId;
	*/
	return 0;
}

std::streamsize Attachment::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Attachment", this->name());

	size += Object::writeMdl(ostream);

	writeMdlValueProperty(ostream, size, "AttachmentID", this->attachmentId());

	if (strlen(this->path()) > 0)
		writeMdlValueProperty(ostream, size, "Path", this->path());

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
	size += this->m_visibilities->readMdx(istream);

	checkBytesIncluding(size, nbytesi);

	return size;
}

std::streamsize Attachment::writeMdx(ostream &out) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<ostream>(out, position);

	std::streamsize size = 0;
	size += Object::writeMdx(out);
	wc3lib::write(out, this->m_path, size);
	wc3lib::write(out, this->m_unknown0, size);
	wc3lib::write(out, this->m_attachmentId, size);
	size += this->m_visibilities->writeMdx(out);

	long32 nbytesi = size;
	writeByteCount(out, nbytesi, position, size, true);

	return size;
}

}

}
