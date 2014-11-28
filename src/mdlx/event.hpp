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

#ifndef WC3LIB_MDLX_EVENT_HPP
#define WC3LIB_MDLX_EVENT_HPP

#include "object.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "EventObject".
 */
class Event : public Object
{
	public:
		typedef std::vector<long32> Tracks;

		Event();

		long32 globalSequenceId() const;
		const Tracks& tracks() const;

	protected:
		//OBJ
		//byte *bla; //ASCII "KEVT" // Actually a separate object
		//long32 ntrks; // usually (1)
		//0xFFFFFFFF!!!
		long32 m_globalSequenceId; /// Event tracks global sequence ID.
		Tracks m_tracks;
};

inline long32 Event::globalSequenceId() const
{
	return this->m_globalSequenceId;
}

inline const Event::Tracks& Event::tracks() const
{
	return this->m_tracks;
}

}

}

#endif
