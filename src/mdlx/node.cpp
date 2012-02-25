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
#include "mdlx.hpp"

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

std::streamsize Node::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Node::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize Node::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	long32 nbytesi;
	wc3lib::read(istream, nbytesi, size);
	wc3lib::read(istream, this->m_name, size);
	wc3lib::read(istream, this->m_id, size);
	// register!
	//std::cout << "Name " << this->name() << std::endl;
	//std::cout << "ID " << this->id() << std::endl;
	this->m_mdlx->addNode(this->id(), this);
	wc3lib::read(istream, this->m_parentId, size);
	wc3lib::read(istream, *reinterpret_cast<long32*>(&this->m_type), size);

	//if (!this->inheritsTranslation())
		size += this->m_translations->readMdx(istream);

	//if (!this->inheritsRotation())
		size += this->m_rotations->readMdx(istream);

	//if (!this->inheritsScaling())
		size += this->m_scalings->readMdx(istream);

	//print(std::cout);

	return size;
}

std::streamsize Node::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);
	std::streamsize size = 0;
	wc3lib::write(ostream, this->m_name, size);
	wc3lib::write(ostream, this->m_id, size);
	wc3lib::write(ostream, this->m_parentId, size);
	wc3lib::write(ostream, *reinterpret_cast<const long32*>(&this->m_type), size);

	//if (!this->inheritsTranslation())
		size += this->m_translations->writeMdx(ostream);

	//if (!this->inheritsRotation())
		size += this->m_rotations->writeMdx(ostream);

	//if (!this->inheritsScaling())
		size += this->m_scalings->writeMdx(ostream);

	writeByteCount(ostream, *reinterpret_cast<const long32*>(&size), position, size, true);

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
