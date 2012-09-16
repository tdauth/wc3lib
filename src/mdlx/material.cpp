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

#include <boost/format.hpp>

#include "material.hpp"
#include "layers.hpp"
#include "../internationalisation.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

Material::Material(class Materials *materials) : m_layers(new Layers(this)), GroupMdxBlockMember(materials, "Material")
{
}

Material::~Material()
{
	delete this->m_layers;
}

std::streamsize Material::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Material::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Material");

	if (this->renderMode() & RenderMode::ConstantColor)
		writeMdlProperty(ostream, size, "ConstantColor");

	if (this->renderMode() & RenderMode::SortPrimitivesNearZ)
		writeMdlProperty(ostream, size, "SortPrimsNearZ");

	if (this->renderMode() & RenderMode::SortPrimitivesFarZ)
		writeMdlProperty(ostream, size, "SortPrimsFarZ");

	if (this->renderMode() & RenderMode::FullResolution)
		writeMdlProperty(ostream, size, "FullResolution");

	if (this->priorityPlane() != 0)
		writeMdlValueProperty(ostream, size, "PriorityPlane", this->priorityPlane());

	size += this->layers()->writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Material::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	long32 includingSize; // including size itself!
	wc3lib::read(istream, includingSize, size);

	if (size <= 0)
		throw Exception(boost::format(_("Material: Small byte count.\nBytes %d.\n")) % size);

	wc3lib::read(istream, this->m_priorityPlane, size);
	wc3lib::read(istream, reinterpret_cast<long32&>(this->m_renderMode), size);
	size += this->layers()->readMdx(istream);

	checkBytesIncluding(size, includingSize);

	return size;
}

std::streamsize Material::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize size = 0;
	wc3lib::write(ostream, this->priorityPlane(), size);
	wc3lib::write(ostream, (long32)(this->renderMode()), size);
	size += this->layers()->writeMdx(ostream);

	long32 includingSize = boost::numeric_cast<long32>(size);
	writeByteCount(ostream, includingSize, position, size, true);

	return size;
}

}

}

