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

#include <array>

#include "platform.hpp"
#include "format.hpp"

namespace wc3lib
{

/**
 * \brief Basic type for N-dimensional vertex which can be serialized and deserialized.
 *
 * This class allows you to easily store an array like vertex and to use \ref read() and \ref write() to deserialize and serialize it.
 * Vertices are used for 3D data or map coordinates, for example.
 * This class builds the template based base for all other vertex classes.
 *
 * It allows basic array operations like index access.
 *
 * \param T The type of each value hold by the vertex.
 * \param N The fixed size of the vertex.
 *
 * \internal std::array<T, N> is not designed to be inherited so it is aggregated.
 */
template<typename T, typename std::size_t N>
class BasicVertex : public Format
{
	public:
		typedef std::array<T, N> Array;
		static const constexpr std::streamsize dataSize = N * sizeof(T) * sizeof(byte);

		BasicVertex()
		{
		}

		BasicVertex(const BasicVertex<T, N> &other) : m_array(other.m_array)
		{
		}

		virtual std::streamsize read(InputStream &istream) override
		{
			std::streamsize size = 0;

			typename Array::pointer data = this->m_array.data();
			wc3lib::read(istream, data[0], size, dataSize);

			return size;
		}

		virtual std::streamsize write(OutputStream &ostream) const override
		{
			std::streamsize size = 0;
			wc3lib::write(ostream, this->m_array.data(), size, dataSize);

			return size;
		}

		std::size_t size() const
		{
			return this->m_array.size();
		}

		typename Array::const_reference operator[](std::size_t index) const
		{
			return this->m_array[index];
		}

		typename Array::reference operator[](std::size_t index)
		{
			return this->m_array[index];
		}

		bool operator==(const BasicVertex &other)
		{
			return this->m_array == other.m_array;
		}

		void fill(typename Array::const_reference value)
		{
			this->m_array.fill(value);
		}

	protected:
		Array m_array;
};

/**
 * \brief Provides serialization for a two dimensional vertex.
 */
template<typename T = float32>
class Vertex2d : public BasicVertex<T, 2>
{
	public:
		typedef BasicVertex<T, 2> Base;

		Vertex2d() : Base()
		{
		}

		Vertex2d(T x, T y)
		{
			this->m_array[0] = x;
			this->m_array[1] = y;
		}

		Vertex2d(const Base &other) : Base(other)
		{
		}

		T x() const
		{
			return this->m_array[0];
		}

		T y() const
		{
			return this->m_array[1];
		}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vertex2d<T>& v)
{
	return os << "Vertex2d[" << v.x() << "," << v.y() << "]";
}

template<typename T>
bool operator==(const Vertex2d<T>& lhs, const Vertex2d<T>& rhs)
{
    return lhs.x() && rhs.x() && lhs.y() == rhs.y();
}

template<typename T>
bool operator !=(const Vertex2d<T>& lhs, const Vertex2d<T>& rhs)
{
    return !(lhs == rhs);
}

/**
 * \brief Provides serialization for a three dimensional vertex.
 */
template<typename T = float32>
class Vertex3d : public BasicVertex<T, 3>
{
	public:
		typedef BasicVertex<T, 3> Base;

		Vertex3d() : Base()
		{
		}

		Vertex3d(T x, T y, T z)
		{
			this->m_array[0] = x;
			this->m_array[1] = y;
			this->m_array[2] = z;
		}

		Vertex3d(const Base &other) : Base(other)
		{
		}

		void setX(T x)
		{
			this->m_array[0] = x;
		}

		T x() const
		{
			return this->m_array[0];
		}

		void setY(T y)
		{
			this->m_array[1] = y;
		}

		T y() const
		{
			return this->m_array[1];
		}

		void setZ(T z)
		{
			this->m_array[2] = z;
		}

		T z() const
		{
			return this->m_array[2];
		}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vertex3d<T>& v)
{
	return os << "Vertex3d[" << v.x() << "," << v.y() << "," << v.z() << "]";
}

template<typename T>
bool operator==(const Vertex3d<T>& lhs, const Vertex3d<T>& rhs)
{
    return lhs.x() && rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
}

template<typename T>
bool operator !=(const Vertex3d<T>& lhs, const Vertex3d<T>& rhs)
{
    return !(lhs == rhs);
}

/**
 * \brief Provides serialization for a quaternion.
 */
template<typename T = float32>
class Quaternion : public BasicVertex<T, 4>
{
	public:
		typedef BasicVertex<T, 4> Base;

		Quaternion() : Base()
		{
		}

		Quaternion(T a, T b, T c, T d)
		{
			this->m_array[0] = a;
			this->m_array[1] = b;
			this->m_array[2] = c;
			this->m_array[3] = d;
		}

		Quaternion(const Base &other) : Base(other)
		{
		}

		T a() const
		{
			return this->m_array[0];
		}

		T b() const
		{
			return this->m_array[1];
		}

		T c() const
		{
			return this->m_array[2];
		}

		T d() const
		{
			return this->m_array[3];
		}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Quaternion<T>& v)
{
	return os << "Quaternion[" << v.a() << "," << v.b() << "," << v.c() << "," << v.d() << "]";
}

template<typename T>
bool operator==(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
{
    return lhs.a() && rhs.a() && lhs.b() == rhs.b() && lhs.c() == rhs.c() && lhs.d() == rhs.d();
}

template<typename T>
bool operator !=(const Quaternion<T>& lhs, const Quaternion<T>& rhs)
{
    return !(lhs == rhs);
}

}

#endif
