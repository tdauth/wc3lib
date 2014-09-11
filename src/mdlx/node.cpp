/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include "node.hpp"
#include "mdlxtranslations.hpp"
#include "mdlxrotations.hpp"
#include "mdlxscalings.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

Node::Node(class Mdlx *mdlx) : m_mdlx(mdlx), m_translations(new MdlxTranslations(mdlx)), m_rotations(new MdlxRotations(mdlx)), m_scalings(new MdlxScalings(mdlx))
{
}

Node::~Node()
{
	delete this->m_translations;
	delete this->m_rotations;
	delete this->m_scalings;
}

std::streamsize Node::readMdl(istream &istream)
{
	return 0;
}

std::streamsize Node::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;

	writeMdlValueProperty(ostream, size, "ObjectId", id());

	if (parentId() != noneId)
		writeMdlValueProperty(ostream, size, "Parent", parentId());

	if (type() & Type::DontInheritTranslation)
		writeMdlProperty(ostream, size, "DontInheritTranslation");

	if (type() & Type::DontInheritRotation)
		writeMdlProperty(ostream, size, "DontInheritRotation");

	if (type() & Type::DontInheritScaling)
		writeMdlProperty(ostream, size, "DontInheritScaling");

	if (type() & Type::Billboarded)
		writeMdlProperty(ostream, size, "Billboarded");

	if (type() & Type::BillboardedLockX)
		writeMdlProperty(ostream, size, "BillboardedLockX");

	if (type() & Type::BillboardedLockY)
		writeMdlProperty(ostream, size, "BillboardedLockY");

	if (type() & Type::BillboardedLockZ)
		writeMdlProperty(ostream, size, "BillboardedLockZ");

	if (type() & Type::CameraAnchored)
		writeMdlProperty(ostream, size, "CameraAnchored");

	if (type() & Type::LineEmitter)
		writeMdlProperty(ostream, size, "LineEmitter");

	if (type() & Type::Unfogged)
		writeMdlProperty(ostream, size, "Unfogged");

	if (type() & Type::ModelSpace)
		writeMdlProperty(ostream, size, "ModelSpace");

	if (type() & Type::XYQuad)
		writeMdlProperty(ostream, size, "XYQuad");

	if (!translations()->properties().empty())
		size += translations()->writeMdl(ostream);

	if (!rotations()->properties().empty())
		size += rotations()->writeMdl(ostream);

	if (!scalings()->properties().empty())
		size += scalings()->writeMdl(ostream);

	return size;
}

std::streamsize Node::readMdx(istream &istream)
{
	std::streamsize size = 0;
	long32 nbytesi;
	wc3lib::read(istream, nbytesi, size);

	if (nbytesi < sizeof(nbytesi))
		throw Exception(boost::format(_("Node inclusive byte count %1% is smaller than its own size %2%!")) % nbytesi % sizeof(nbytesi));

	wc3lib::read(istream, this->m_name, size, nameSize);
	wc3lib::read(istream, this->m_id, size);
	wc3lib::read(istream, this->m_parentId, size);
	wc3lib::read(istream, *reinterpret_cast<long32*>(&this->m_type), size);

	//if (!this->inheritsTranslation())
	size += this->translations()->readMdx(istream);

	//if (!this->inheritsRotation())
	size += this->rotations()->readMdx(istream);

	//if (!this->inheritsScaling())
	size += this->scalings()->readMdx(istream);

	if (size != nbytesi)
		throw Exception(boost::format(_("Node \"%1%\" has inclusive byte count %2% but read %3% bytes.")) % this->name() % nbytesi % size);

	//print(std::cout);

	return size;
}

std::streamsize Node::writeMdx(ostream &ostream) const
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);
	std::streamsize size = 0;
	wc3lib::write(ostream, this->name(), size, nameSize);
	wc3lib::write(ostream, this->id(), size);
	wc3lib::write(ostream, this->parentId(), size);
	wc3lib::write<long32>(ostream, static_cast<long32>(this->type()), size);

	//if (!this->inheritsTranslation())
	size += this->translations()->writeMdx(ostream);

	//if (!this->inheritsRotation())
	size += this->rotations()->writeMdx(ostream);

	//if (!this->inheritsScaling())
	size += this->scalings()->writeMdx(ostream);

	const long32 nbytesi = boost::numeric_cast<long32>(size);
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

std::ostream& Node::print(ostream &ostream) const
{
	ostream << "Node: " << std::endl
	<< "Name: " << this->name() << std::endl
	<< "ID: " << this->id() << std::endl
	<< "Parent ID: " << this->parentId() << std::endl
	<< "Translations: " << this->translations()->properties().size() << std::endl
	<< "Rotations: " << this->rotations()->properties().size() << std::endl
	<< "Scalings: " << this->scalings()->properties().size() << std::endl
	<< std::endl;

	return ostream;
}

}

}
