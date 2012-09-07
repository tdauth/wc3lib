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

#include "object.hpp"
#include "mdlxtranslations.hpp"
#include "mdlxrotations.hpp"
#include "mdlxscalings.hpp"
#include "attachmentvisibilities.hpp"
#include "mdlx.hpp"

namespace wc3lib
{

namespace mdlx
{

Object::Object(class Mdlx *mdlx) : Node(mdlx), m_visibilities(new AttachmentVisibilities(mdlx))
{
}

Object::~Object()
{
	delete this->m_visibilities;
}

std::streamsize Object::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Object::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = Node::writeMdl(ostream);

	if (!visibilities()->properties().empty())
		size += visibilities()->writeMdl(ostream);

	return size;
}

std::streamsize Object::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = Node::readMdx(istream);
	size += this->m_visibilities->readMdx(istream);

	return size;
}

std::streamsize Object::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = Node::writeMdx(ostream);
	size += this->m_visibilities->writeMdx(ostream);

	return size;
}

}

}
