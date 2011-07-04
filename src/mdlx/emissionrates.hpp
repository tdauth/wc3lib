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

#ifndef WC3LIB_MDLX_EMISSIONRATES_HPP
#define WC3LIB_MDLX_EMISSIONRATES_HPP

#include "mdlxalphas.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDX tag "KP2E".
 * MDL tag "EmissionRate".
 */
class EmissionRates : public MdlxAlphas
{
	public:
		typedef std::list<class EmissionRate*> Rates;
		
		EmissionRates(class Mdlx *mdlx);

		const Rates& emissionRates() const;

	protected:
		virtual class MdlxAnimatedProperty* createAnimatedProperty();
		
		class Mdlx *m_mdlx;
};

inline const EmissionRates::Rates& EmissionRates::emissionRates() const
{
	return reinterpret_cast<const Rates&>(this->mdlxAlphas());
}

}

}

#endif
