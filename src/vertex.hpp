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

#include <boost/array.hpp>

#include "platform.hpp"
#include "format.hpp"

namespace wc3lib
{

/**
 * \todo Use some boost::geometry class to inherit which provides much more functionality.
 */
template<typename T, typename std::size_t N>
class BasicVertex : public boost::array<T, N>, public Format
{
	public:
		typedef boost::array<T, N> Base;

		BasicVertex(const Base &base)
		{
			*this = base;
		}

		BasicVertex()
		{
			for (std::size_t i = 0; i < Base::size(); ++i)
				(*this)[i] = 0;
		}

		BasicVertex(T x, T y, T z)
		{
			(*this)[0] = x;
			(*this)[1] = y;
			(*this)[2] = z;
		}

		BasicVertex(T x, T y)
		{
			(*this)[0] = x;
			(*this)[1] = y;
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

		virtual std::streamsize read(InputStream &istream) throw (Exception)
		{
			std::streamsize size = 0;

			for (std::size_t i = 0; i < Base::size(); ++i)
				wc3lib::read(istream, (*this)[i], size);

			return size;
		}

		virtual std::streamsize write(OutputStream &ostream) const throw (Exception)
		{
			std::streamsize size = 0;

			for (std::size_t i = 0; i < Base::size(); ++i)
				wc3lib::write(ostream, (*this)[i], size);

			return size;
		}
};

typedef BasicVertex<float32, 3> Vertex;

}

#endif
