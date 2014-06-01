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
 */
template<typename T, typename std::size_t N>
class BasicVertex : public std::array<T, N>, public Format
{
	public:
		typedef std::array<T, N> Base;

		BasicVertex() : Base()
		{
		}

		BasicVertex(const BasicVertex<T, N> &other) : Base(other) {
		}

		virtual std::streamsize read(InputStream &istream) throw (Exception)
		{
			std::streamsize size = 0;

			for (std::size_t i = 0; i < Base::size(); ++i)
			{
				wc3lib::read(istream, (*this)[i], size);
			}

			return size;
		}

		virtual std::streamsize write(OutputStream &ostream) const throw (Exception)
		{
			std::streamsize size = 0;

			for (std::size_t i = 0; i < Base::size(); ++i)
			{
				wc3lib::write(ostream, (*this)[i], size);
			}

			return size;
		}
};

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
			(*this)[0] = x;
			(*this)[1] = y;
		}

		Vertex2d(const Base &other) : Base(other) {
		}

		T x() const
		{
			return (*this)[0];
		}

		T y() const
		{
			return (*this)[1];
		}
};

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
			(*this)[0] = x;
			(*this)[1] = y;
			(*this)[2] = z;
		}

		Vertex3d(const Base &other) : Base(other) {
		}

		T x() const
		{
			return (*this)[0];
		}

		T y() const
		{
			return (*this)[1];
		}

		T z() const
		{
			return (*this)[2];
		}
};

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
			(*this)[0] = a;
			(*this)[1] = b;
			(*this)[2] = c;
			(*this)[3] = d;
		}

		Quaternion(const Base &other) : Base(other) {
		}

		T a() const
		{
			return (*this)[0];
		}

		T b() const
		{
			return (*this)[1];
		}

		T c() const
		{
			return (*this)[2];
		}

		T d() const
		{
			return (*this)[2];
		}
};

}

#endif
