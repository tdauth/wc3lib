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
	size += Object::writeMdl(ostream);
	// Observe properties of an Object.
	// Path only appears if its length is greater than 0.
	// Maximum size is 256 characters (0x100 bytes)
	// I am unsure as to how it is determined that AttachmentID be shown...
	// NightElfCampaign3D and UndeadCampaign3D.mdl are the only two MDLs
	// that utilize this attribute. Their only exclusive similarity is the
	// underscore prefixing their name string. "_Blah"
	/*
	ostream
	<< "Attachment " << this->name() << " {\n"
	<< "\tObjectId " << this->objectId() << ",\n"
	;

	if (this->parent() != 0xFFFFFFFF)
		ostream << "\tParent " << this->parent() << ",\n";

	if (this->type() & Object::BillboardedLockZ)
		ostream << "\tBillboardedLockZ,\n";

	if (this->type() & Object::BillboardedLockY)
		ostream << "\tBillboardedLockY,\n";

	if (this->type() & Object::BillboardedLockX)
		ostream << "\tBillboardedLockX,\n";

	if (this->type() & Object::Billboarded)
		ostream << "\tBillboarded,\n";

	if (this->type() & Object::CameraAnchored)
		ostream << "\tCameraAnchored,\n";

	if (this->type() & Object::DontInheritRotation)
		ostream << "\tDontInherit { Rotation },\n";
	else if (this->type() & Object::DontInheritTranslation)
		ostream << "\tDontInherit { Translation },\n";
	else if (this->type() & Object::DontInheritScaling)
		ostream << "\tDontInherit { Scaling },\n";

	ostream << "\tAttachmentID " << this->attachmentId() << ",\n";

	if (strlen(this->path()) > 0)
		ostream << "\tPath " << this->path() << ",\n";

	//fstream << "\tTranslation { " << this->translations()->x() << ", " << this->translations()->y() << ", " << this->translations()->z() << " }\n";
	//fstream << "\tRotation { " << this->rotations()->a() << ", " << this->rotations()->b() << ", " << this->rotations()->c() << ", " << this->rotations()->d() << " }\n";
	//fstream << "\tScaling { " << this->scalings()->x() << ", " << this->scalings()->y() << ", " << this->scalings()->z() << " }\n";
	//fstream << "\tVisibility " << this->visibilities()->value() << '\n';
	ostream << "}\n";
	*/
	return size;
}


std::streamsize Attachment::readMdx(istream &istream) throw (class Exception)
{
	long32 nbytesi = 0;
	std::streamsize size = 0;
	wc3lib::read(istream, nbytesi, size);
	std::cout << "Attachments " << this->attachments()->attachments().size() << std::endl;
	size += Object::readMdx(istream);
	wc3lib::read(istream, this->m_path, size);
	wc3lib::read(istream, this->m_unknown0, size);
	wc3lib::read(istream, this->m_attachmentId, size);
	size += this->m_visibilities->readMdx(istream);

	if (size != nbytesi)
		throw Exception(boost::str(boost::format(_("Attachment: File byte count is not equal to real byte count.\nFile byte count: %1%.\nReal byte count: %2%.\n")) % nbytesi % size));

	return size;
}

std::streamsize Attachment::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<mdlx::ostream>(ostream, position);

	std::streamsize size = 0;
	size += Object::writeMdx(ostream);
	wc3lib::write(ostream, this->m_path, size);
	wc3lib::write(ostream, this->m_unknown0, size);
	wc3lib::write(ostream, this->m_attachmentId, size);
	size += this->m_visibilities->writeMdx(ostream);
	
	long32 nbytesi = size;
	writeByteCount(ostream, nbytesi, position, size, true);
	
	return size;
}

}

}
