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

#ifndef WC3LIB_MDLX_MDLXALPHA_HPP
#define WC3LIB_MDLX_MDLXALPHA_HPP

#include "mdlxanimatedproperty.hpp"
#include "mdlxalphas.hpp"

namespace wc3lib
{

namespace mdlx
{

class MdlxAlpha : public MdlxAnimatedProperty
{
	public:
		MdlxAlpha(class MdlxAlphas *alphas);
		virtual ~MdlxAlpha();
		
		class MdlxAlphas* mdlxAlphas() const;
		float32 alpha() const;
		float32 inTanAlpha() const;
		float32 outTanAlpha() const;

	protected:
		class MdxAlphas *m_alphas;
		long32 m_frame;
		float32 m_state;
		//if (LineType > 1) {
		float32 m_inTan;
		float32 m_outTan;
		//}
};

inline class MdlxAlphas* MdlxAlpha::mdlxAlphas() const
{
	return dynamic_cast<class MdlxAlphas*>(properties());
}

inline float32 MdlxAlpha::alpha() const
{
	return values()[0];
}

inline float32 MdlxAlpha::inTanAlpha() const
{
	return inTan()[0];
}

inline float32 MdlxAlpha::outTanAlpha() const
{
	return outTan()[0];
}

}

}

#endif
