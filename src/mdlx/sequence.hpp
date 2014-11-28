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

#ifndef WC3LIB_MDLX_SEQUENCE_HPP
#define WC3LIB_MDLX_SEQUENCE_HPP

#include <cstring>

#include "bounds.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Anim".
 */
class Sequence : public Bounds
{
	public:
		static const std::size_t nameSize = 0x50;

		Sequence();

		void setName(const string &name);
		/**
		 * Comments: All text after the "-" and still inside the quotes is ignored.
		 * \return Returns name of sequence with constant length of \ref nameSize.
		 */
		const byte* name() const;
		void setIntervalStart(long32 intervalStart);
		long32 intervalStart() const;
		void setIntervalEnd(long32 intervalEnd);
		long32 intervalEnd() const;
		void setMoveSpeed(float32 moveSpeed);
		float32 moveSpeed() const;
		void setNoLooping(long32 noLooping);
		long32 noLooping() const;
		void setRarity(float32 rarity);
		float32 rarity() const;
		void setUnknown0(long32 unknown0);
		long32 unknown0() const;

		/**
		 * \return Returns the length of the sequence calculated by the interval.
		 */
		long32 length() const;

	protected:
		byte m_name[nameSize]; //(0x50 bytes)
		long32 m_intervalStart, m_intervalEnd;
		float32 m_moveSpeed;
		long32 m_noLooping; //(0:loop; 1:no loop)
		float32 m_rarity;
		long32 m_unknown0; //(0)

};

inline void Sequence::setName(const string &name)
{
	if (name.size() + 1 > nameSize)
	{
		throw std::out_of_range("");
	}

	std::memcpy(this->m_name, name.c_str(), name.size() + 1);
}

inline const byte* Sequence::name() const
{
	return this->m_name;
}

inline void Sequence::setIntervalStart(long32 intervalStart)
{
	this->m_intervalStart = intervalStart;
}

inline long32 Sequence::intervalStart() const
{
	return this->m_intervalStart;
}

inline void Sequence::setIntervalEnd(long32 intervalEnd)
{
	this->m_intervalEnd = intervalEnd;
}

inline long32 Sequence::intervalEnd() const
{
	return this->m_intervalEnd;
}

inline void Sequence::setMoveSpeed(float32 moveSpeed)
{
	this->m_moveSpeed = moveSpeed;
}

inline float32 Sequence::moveSpeed() const
{
	return this->m_moveSpeed;
}

inline void Sequence::setNoLooping(long32 noLooping)
{
	this->m_noLooping = noLooping;
}

inline long32 Sequence::noLooping() const
{
	return this->m_noLooping;
}

inline void Sequence::setRarity(float32 rarity)
{
	this->m_rarity = rarity;
}

inline float32 Sequence::rarity() const
{
	return this->m_rarity;
}

inline void Sequence::setUnknown0(long32 unknown0)
{
	this->m_unknown0 = unknown0;
}

inline long32 Sequence::unknown0() const
{
	return this->m_unknown0;
}

inline long32 Sequence::length() const
{
	return this->intervalEnd() - this->intervalStart();
}

}

}

#endif
