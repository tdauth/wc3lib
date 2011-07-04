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

#ifndef WC3LIB_MDLX_EMISSIONRATE_HPP
#define WC3LIB_MDLX_EMISSIONRATE_HPP

#include "mdlxalpha.hpp"
#include "emissionrates.hpp"

namespace wc3lib
{

namespace mdlx
{

class EmissionRate : public MdlxAlpha
{
	public:
		EmissionRate(class EmissionRates *emissionRates);
	
		class EmissionRates* emissionRates() const;
};

inline class EmissionRates* EmissionRate::emissionRates() const
{
	return boost::polymorphic_cast<class EmissionRates*>(this->mdlxAlphas());

}

}

}

#endif
