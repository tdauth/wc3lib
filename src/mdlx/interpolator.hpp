/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MDLX_INTERPOLATOR_HPP
#define WC3LIB_MDLX_INTERPOLATOR_HPP

#include "platform.hpp"
#include "mdlxanimatedproperties.hpp"
#include "mdlxanimatedproperty.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \brief Allows to calculate interpolation data required by Warcraft III animations.
 *
 * See \ref mdlx::LineType for all possible interpolation types of Warcraft III animations.
 *
 * \ingroup animations
 */
template<typename std::size_t N = 3, typename _ValueType = float32>
class Interpolator
{
	public:
		typedef MdlxAnimatedProperties<N, _ValueType> AnimatedProperties;
		typedef MdlxAnimatedProperty<N, _ValueType> AnimatedProperty;
		typedef typename AnimatedProperty::Values Values;

		void boundaryValues(Values &bound1, Values &bound2);

		/**
		 * Calculates the interpolated values at the given time frame \p frame depending
		 * of the interpolation type of \ref Interpolator::animatedProperties().
		 */
		Values calculate(long32 frame);


		AnimatedProperties* animatedProperties() const;


	private:
		AnimatedProperties *m_animatedProperties;
};

template<typename std::size_t N, typename _ValueType>
inline typename Interpolator<N, _ValueType>::AnimatedProperties* Interpolator<N, _ValueType>::animatedProperties() const
{
	return this->m_animatedProperties;
}

template<typename std::size_t N, typename _ValueType>
inline typename Interpolator<N, _ValueType>::Values Interpolator<N, _ValueType>::calculate(long32 frame)
{

}

}

}

#endif
