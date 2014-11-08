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
 * \param N The dimension of the interpolated values. Usually 3 is used for three dimensional coordinates. 4 might be used for Quaternions. 1 might be used for alpha values.
 * \param _ValueType The data type for the interpolated values.
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

		/**
		 * Stores the boundary values of \p frame into \p bound1 and \p bound2 where \p bound1 is the left boundary
		 * and \p bound2 is the right boundary.
		 * It searches in \ref MdlxAnimatedProperties::properties() of the corresponding instance \p animatedProperties() for the boundary values.
		 *
		 * Boundaries are required for a successful interpolation of a specific frame value.
		 *
		 * \return Returns true if both boundaries has been found. Otherwise it returns false. For example if the \p frame value is invalid.
		 */
		bool boundaryValues(long32 frame, Values &bound1, Values &bound2);

		/**
		 * Calculates the interpolated values at the given time frame \p frame depending
		 * on the interpolation type of \ref Interpolator::animatedProperties().
		 *
		 * It uses \ref boundaryValues() to get the existing boundary values of \p frame.
		 * The existing boundary values are stored in the corresponding animated properties instance (\ref MdlxAnimatedProperties::properties()).
		 *
		 * \return Returns the newly interpolated values.
		 */
		Values calculate(long32 frame);


		void setAnimatedProperties(AnimatedProperties *properties);
		AnimatedProperties* animatedProperties() const;


	private:
		AnimatedProperties *m_animatedProperties;
};

template<typename std::size_t N, typename _ValueType>
bool Interpolator<N, _ValueType>::boundaryValues(long32 frame, Values &bound1, Values &bound2)
{
	std::size_t i = 0;

	BOOST_FOREACH(typename AnimatedProperties::Properties::reference ref, this->animatedProperties()->properties())
	{
		if (ref.frame() > frame)
		{
			if (i > 0)
			{
				bound2 = ref.values();
				bound1 = this->animatedProperties()->properties()[i - 1].values();

				return true;
			}

			break;
		}

		++i;
	}

	return false;
}

template<typename std::size_t N, typename _ValueType>
void Interpolator<N, _ValueType>::setAnimatedProperties(AnimatedProperties *properties)
{
	this->m_animatedProperties = properties;
}

template<typename std::size_t N, typename _ValueType>
inline typename Interpolator<N, _ValueType>::AnimatedProperties* Interpolator<N, _ValueType>::animatedProperties() const
{
	return this->m_animatedProperties;
}

template<typename std::size_t N, typename _ValueType>
inline typename Interpolator<N, _ValueType>::Values Interpolator<N, _ValueType>::calculate(long32 frame)
{
	Values &bound1;
	Values &bound2;

	if (boundaryValues(frame, bound1, bound2))
	{
		switch (animatedProperties()->lineType())
		{
			/*
			 * If no interpolation method is used simply take the first available value.
			 * No calculation has to be done.
			 */
			case LineType::DontInterpolate:
			{
				return bound1;
			}
		}
	}

	return Values();
}

}

}

#endif
