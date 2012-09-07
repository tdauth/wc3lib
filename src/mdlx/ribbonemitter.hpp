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

#ifndef WC3LIB_MDLX_RIBBONEMITTER_HPP
#define WC3LIB_MDLX_RIBBONEMITTER_HPP

#include "node.hpp"
#include "groupmdxblockmember.hpp"
#include "ribbonemitters.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "RibbonEmitter".
 */
class RibbonEmitter : public Node, public GroupMdxBlockMember
{
	public:
		RibbonEmitter(class RibbonEmitters *ribbonEmitters);
		virtual ~RibbonEmitter();

		class RibbonEmitters* ribbonEmitters() const;
		float32 heightAboveValue() const;
		float32 heightBelowValue() const;
		float32 alpha() const;
		float32 colorRed() const;
		float32 colorGreen() const;
		float32 colorBlue() const;
		float32 lifeSpan() const;
		long32 unknown0() const;
		long32 emissionRate() const;
		long32 rows() const;
		long32 columns() const;
		long32 materialId() const;
		float32 gravity() const;
		class RibbonEmitterVisibilities* visibilities() const;
		class RibbonEmitterHeightsAbove* heightsAbove() const;
		class RibbonEmitterHeightsBelow* heightsBelow() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		//long nbytesi;
		//long nbytesikg; // inclusive bytecount including KGXXs
		float32 m_heightAboveValue;
		float32 m_heightBelowValue;
		float32 m_alpha;
		float32 m_colorRed, m_colorGreen, m_colorBlue;
		float32 m_lifeSpan;
		long32 m_unknown0; //(0)
		long32 m_emissionRate;
		long32 m_rows;
		long32 m_columns;
		long32 m_materialId;
		float32 m_gravity;
		class RibbonEmitterVisibilities *m_visibilities; //(KRVS)
		class RibbonEmitterHeightsAbove *m_heightsAbove; //(KRHA)
		class RibbonEmitterHeightsBelow *m_heightsBelow; //(KRHB)
};

inline class RibbonEmitters* RibbonEmitter::ribbonEmitters() const
{
	return boost::polymorphic_cast<class RibbonEmitters*>(this->parent());
}

inline float32 RibbonEmitter::heightAboveValue() const
{
	return m_heightAboveValue;
}

inline float32 RibbonEmitter::heightBelowValue() const
{
	return m_heightBelowValue;
}

inline float32 RibbonEmitter::alpha() const
{
	return m_alpha;
}

inline float32 RibbonEmitter::colorRed() const
{
	return m_colorRed;
}

inline float32 RibbonEmitter::colorGreen() const
{
	return m_colorGreen;
}

inline float32 RibbonEmitter::colorBlue() const
{
	return m_colorBlue;
}

inline float32 RibbonEmitter::lifeSpan() const
{
	return m_lifeSpan;
}

inline long32 RibbonEmitter::unknown0() const
{
	return m_unknown0;
}

inline long32 RibbonEmitter::emissionRate() const
{
	return m_emissionRate;
}

inline long32 RibbonEmitter::rows() const
{
	return m_rows;
}

inline long32 RibbonEmitter::columns() const
{
	return m_columns;
}

inline long32 RibbonEmitter::materialId() const
{
	return m_materialId;
}

inline float32 RibbonEmitter::gravity() const
{
	return m_gravity;
}

inline class RibbonEmitterVisibilities* RibbonEmitter::visibilities() const
{
	return m_visibilities;
}

inline class RibbonEmitterHeightsAbove* RibbonEmitter::heightsAbove() const
{
	return m_heightsAbove;
}

inline class RibbonEmitterHeightsBelow* RibbonEmitter::heightsBelow() const
{
	return m_heightsBelow;
}

}

}

#endif
