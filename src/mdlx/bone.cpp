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

#include "bone.hpp"

namespace wc3lib
{

namespace mdlx
{

Bone::Bone(class Bones *bones) : Object(bones->mdlx()), GroupMdxBlockMember(bones, "Bone")
{
}

std::streamsize Bone::readMdl(istream &istream)
{
	return 0;
}

std::streamsize Bone::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "Bone", this->name(), 0, true);

	if (this->geosetId() == noneId)
		writeMdlValueProperty(ostream, size, "GeosetId", "Multiple");
	else
		writeMdlValueProperty(ostream, size, "GeosetId", this->geosetId());

	if (this->geosetAnimationId() == noneId)
		writeMdlValueProperty(ostream, size, "GeosetAnimId", "None");
	else
		writeMdlValueProperty(ostream, size, "GeosetAnimId", this->geosetAnimationId());

	size += Object::writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Bone::readMdx(istream &istream)
{
	std::streamsize size = Object::readMdx(istream);
	wc3lib::read(istream, this->m_geosetId, size);
	wc3lib::read(istream, this->m_geosetAnimationId, size);

	return size;
}

std::streamsize Bone::writeMdx(ostream &ostream) const
{
	std::streamsize size = Object::writeMdx(ostream);
	wc3lib::write(ostream, this->geosetId(), size);
	wc3lib::write(ostream, this->geosetAnimationId(), size);

	return size;
}

}

}
