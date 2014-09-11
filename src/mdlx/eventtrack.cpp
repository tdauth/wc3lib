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

#include "eventtrack.hpp"
#include "eventtracks.hpp"

namespace wc3lib
{

namespace mdlx
{

EventTrack::EventTrack(class EventTracks *eventTracks) : GroupMdxBlockMember(eventTracks, "")
{
}

EventTrack::~EventTrack()
{
}

std::streamsize EventTrack::readMdl(istream &istream)
{
	return 0;
}

std::streamsize EventTrack::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;
	writeMdlValueProperty(ostream, size, "", this->frames());

	return size;
}

std::streamsize EventTrack::readMdx(istream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_frames, size);

	return size;
}

std::streamsize EventTrack::writeMdx(ostream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->frames(), size);

	return size;
}

}

}
