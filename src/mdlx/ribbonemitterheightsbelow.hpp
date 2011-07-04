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

#ifndef WC3LIB_MDLX_RIBBONEMITTERHEIGHTSBELOW_HPP
#define WC3LIB_MDLX_RIBBONEMITTERHEIGHTSBELOW_HPP

#include "mdlxalphas.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "KRHB".
 * MDL tag "HeightBelow".
 */
class RibbonEmitterHeightsBelow : public MdlxAlphas
{
	public:
		typedef std::list<class RibbonEmitterHeightBelow*> Heights;
		
		RibbonEmitterHeightsBelow(class RibbonEmitter *ribbonEmitter);

		class RibbonEmitter* ribbonEmitter() const;
		const Heights& heights() const;

	protected:
		virtual class MdlxAnimatedProperty* createAnimatedProperty();
		
		class RibbonEmitter *m_ribbonEmitter;
};

inline class RibbonEmitter* RibbonEmitterHeightsBelow::ribbonEmitter() const
{
	return this->m_ribbonEmitter;
}

inline const RibbonEmitterHeightsBelow::Heights& RibbonEmitterHeightsBelow::heights() const
{
	return reinterpret_cast<const Heights&>(this->mdlxAlphas());
}

}

}

#endif
