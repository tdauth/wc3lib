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

		Interpolator();

		/**
		 * Stores the boundary values of \p frame into \p bound1 and \p bound2 where \p bound1 is the left boundary
		 * and \p bound2 is the right boundary.
		 * It searches in \ref MdlxAnimatedProperties::properties() of the corresponding instance \p animatedProperties() for the boundary values.
		 *
		 * Boundaries are required for a successful interpolation of a specific frame value.
		 *
		 * \return Returns true if both boundaries has been found. Otherwise it returns false. For example if the \p frame value is invalid.
		 */
		bool boundaryValues(long32 frame, AnimatedProperty &bound1, AnimatedProperty &bound2);

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


		void setAnimatedProperties(const AnimatedProperties *properties);
		const AnimatedProperties* animatedProperties() const;


	private:
		const AnimatedProperties *m_animatedProperties;
};

template<typename std::size_t N, typename _ValueType>
Interpolator<N, _ValueType>::Interpolator() : m_animatedProperties(0)
{
}

template<typename std::size_t N, typename _ValueType>
bool Interpolator<N, _ValueType>::boundaryValues(long32 frame, AnimatedProperty &bound1, AnimatedProperty &bound2)
{
	std::size_t i = 0;

	BOOST_FOREACH(typename AnimatedProperties::Properties::const_reference ref, this->animatedProperties()->properties())
	{
		if (ref.frame() >= frame)
		{
			bound2 = ref;

			if (ref.frame() != frame && i > 0)
			{
				bound1 = this->animatedProperties()->properties()[i - 1];
			}
			else
			{
				bound1 = ref;
			}

			return true;
		}

		++i;
	}

	return false;
}

template<typename std::size_t N, typename _ValueType>
void Interpolator<N, _ValueType>::setAnimatedProperties(const AnimatedProperties *properties)
{
	this->m_animatedProperties = properties;
}

template<typename std::size_t N, typename _ValueType>
inline const typename Interpolator<N, _ValueType>::AnimatedProperties* Interpolator<N, _ValueType>::animatedProperties() const
{
	return this->m_animatedProperties;
}

namespace
{

template<typename T, typename std::size_t N>
BasicVertex<T, N> multiply(const BasicVertex<T, N> &vertex, T factor)
{
	BasicVertex<T, N> result;

	for (std::size_t i = 0; i < N; ++i)
	{
		result[i] = vertex[i] * factor;
	}

	return result;
}

template<typename T, typename std::size_t N>
BasicVertex<T, N> operator*(const BasicVertex<T, N> &vertex, T factor)
{
	return multiply(vertex, factor);
}

template<typename T, typename std::size_t N>
BasicVertex<T, N> operator*(T factor, const BasicVertex<T, N> &vertex)
{
	return multiply(vertex, factor);
}

template<typename T, typename std::size_t N>
BasicVertex<T, N> operator+(const BasicVertex<T, N> &vertex1, const BasicVertex<T, N> &vertex2)
{
	BasicVertex<T, N> result;

	for (std::size_t i = 0; i < N; ++i)
	{
		result[i] = vertex1[i] + vertex2[i];
	}

	return result;
}

}

template<typename std::size_t N, typename _ValueType>
inline typename Interpolator<N, _ValueType>::Values Interpolator<N, _ValueType>::calculate(long32 frame)
{
	AnimatedProperty bound1;
	AnimatedProperty bound2;

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
				return bound1.values();
			}

			case LineType::Linear:
			{
				// http://de.wikipedia.org/wiki/Interpolation_%28Mathematik%29#Lineare_Interpolation
				return bound1.values() * static_cast<_ValueType>((bound2.frame() - frame) / (bound2.frame() - bound1.frame())) + bound2.values() * static_cast<_ValueType>((frame - bound1.frame()) / (bound2.frame() - bound1.frame()));
			}

			// TODO optimize formula
			case LineType::Hermite:
			{
				// http://cubic.org/docs/hermite.htm
				// says that 3Ds max uses Kochanek-Bartels Splines




				// http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Representations
				const long32 t = (frame - bound1.frame()) / (bound2.frame() - bound1.frame());

				// Hermite basis functions: h00(t) ...
				const _ValueType h00 = 2 * std::pow(t, 3) - 3 * std::pow(t, 2) + 1;
				const _ValueType h10 = std::pow(t, 3) - std::pow(t, 2) + t;
				const _ValueType h01 = -2 * std::pow(t, 3) + 3 * std::pow(t, 2);
				const _ValueType h11 = std::pow(t, 3) - std::pow(t, 2);

				return h00 * bound1.values() + h10 * bound1.outTan() + h01 * bound2.values() + h11 * bound2.inTan();
			}

			case LineType::Bezier:
			{
			}
		}
	}

	return Values();
}

}

}

#endif
