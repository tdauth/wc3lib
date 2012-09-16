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

#include "event.hpp"
#include "events.hpp"
#include "eventtracks.hpp"

namespace wc3lib
{

namespace mdlx
{

Event::Event(class Events *events) : Object(events->mdlx()), GroupMdxBlockMember(events, "EventObject"), m_tracks(new EventTracks(this))
{
}

Event::~Event()
{
	delete this->m_tracks;
}

std::streamsize Event::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Event::writeMdl(ostream &ostream) const throw (Exception)
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "EventObject", this->name(), 0, true);

	size += Object::writeMdl(ostream);
	size += this->tracks()->writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}

std::streamsize Event::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = Object::readMdx(istream);
	size += this->tracks()->readMdx(istream);

	return size;
}

std::streamsize Event::writeMdx(ostream &ostream) const throw (Exception)
{
	std::streamsize size = Object::writeMdx(ostream);
	size += this->tracks()->writeMdx(ostream);

	return size;
}

}

}
