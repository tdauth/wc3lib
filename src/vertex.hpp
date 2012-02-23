/***************************************************************************
 *   Copyright (C) 2012 by Tamino Dauth                                    *
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

#ifndef WC3LIB_VERTEX_HPP
#define WC3LIB_VERTEX_HPP

#include <vector>

#include "platform.hpp"

namespace wc3lib
{

/**
 * \todo Use some boost::geometry class to inherit which provides much more functionality.
 */
template<typename T>
class BasicVertex
{
	public:
		BasicVertex() : m_x(0), m_y(0), m_z(0) { }
		BasicVertex(T x, T y, T z) : m_x(x), m_y(y), m_z(z)
		{
		}

		BasicVertex(const std::vector<T> &values) : m_x(values[0]), m_y(values[1]), m_z(values[2])
		{
		}

		T operator[](uint8_t index) const throw (std::out_of_range)
		{
			if (index == 0)
				return m_x;
			else if (index == 1)
				return m_y;
			else if (index == 2)
				return m_z;

			throw std::out_of_range();
		}

		T x() const
		{
			return m_x;
		}

		T y() const
		{
			return m_y;
		}

		T z() const
		{
			return m_z;
		}

	private:
		T m_x, m_y, m_z;
};

typedef BasicVertex<float32> Vertex;

}

#endif
