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

#include "eventtracks.hpp"
#include "eventtrack.hpp"

namespace wc3lib
{

namespace mdlx
{

EventTracks::EventTracks(class Event *event) : GroupMdxBlock("KEVT", "EventTrack", true, true, true), m_event(event)
{
}

EventTracks::~EventTracks()
{
}

std::streamsize EventTracks::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = MdxBlock::readMdx(istream);
	long32 number;
	wc3lib::read(istream, number, size);
	wc3lib::read(istream, this->m_globalSequenceId, size);

	for ( ; number > 0; --number)
	{
		class EventTrack *track = new EventTrack(this);
		size += track->readMdx(istream);
		this->m_members.push_back(track);
	}

	return size;
}

std::streamsize EventTracks::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = MdxBlock::writeMdx(ostream);
	const long32 number = boost::numeric_cast<long32>(this->members().size());
	wc3lib::write(ostream, number, size);
	wc3lib::write(ostream, this->m_globalSequenceId, size);

	BOOST_FOREACH(Members::const_reference track, this->members())
		size += track.writeMdx(ostream);

	return size;
}

class GroupMdxBlockMember* EventTracks::createNewMember()
{
	return new EventTrack(this);
}

}

}
