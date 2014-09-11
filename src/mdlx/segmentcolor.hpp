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

#ifndef WC3LIB_MDLX_SEGMENTCOLOR_HPP
#define WC3LIB_MDLX_SEGMENTCOLOR_HPP

#include "platform.hpp"
#include "mdlxproperty.hpp"

namespace wc3lib
{

namespace mdlx
{

class SegmentColor : public MdlxProperty
{
	public:
		SegmentColor(class ParticleEmitter2 *particleEmitter);
		virtual ~SegmentColor();

		class ParticleEmitter2* particleEmitter() const;
		float32 red() const;
		float32 green() const;
		float32 blue() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		class ParticleEmitter2 *m_particleEmitter;
		// SegmentColor usually 3 segments
		// Inverse order from MDL
		float32 m_red, m_green, m_blue;
};

inline class ParticleEmitter2* SegmentColor::particleEmitter() const
{
	return this->m_particleEmitter;
}

inline float32 SegmentColor::red() const
{
	return this->m_red;
}

inline float32 SegmentColor::green() const
{
	return this->m_green;
}

inline float32 SegmentColor::blue() const
{
	return this->m_blue;
}

}

}

#endif
