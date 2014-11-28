/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef WC3LIB_MDLX_FACES_HPP
#define WC3LIB_MDLX_FACES_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

class Faces
{
	public:
		enum class Type : long32
		{
			Triangles = 4
		};

		typedef std::vector<short16> Vertices;

		Faces();

		void setType(Type type);
		Type type() const;
		void setVertices(const Vertices &vertices);
		const Vertices& vertices() const;

	private:
		Type m_type;
		Vertices m_vertices;
};

inline void Faces::setType(Faces::Type type)
{
	this->m_type = type;
}

inline Faces::Type Faces::type() const
{
	return this->m_type;
}

inline void Faces::setVertices(const Faces::Vertices &vertices)
{
	this->m_vertices = vertices;
}

inline const Faces::Vertices& Faces::vertices() const
{
	return this->m_vertices;
}

}

}

#endif // WC3LIB_MDLX_FACES_HPP
