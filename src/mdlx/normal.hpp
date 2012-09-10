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

#ifndef WC3LIB_MDLX_NORMAL_HPP
#define WC3LIB_MDLX_NORMAL_HPP

#include <boost/cast.hpp>

#include "groupmdxblockmember.hpp"
#include "normals.hpp"

namespace wc3lib
{

namespace mdlx
{

class Normal : public GroupMdxBlockMember
{
	public:
		Normal(class Normals *normals);

		class Normals* normals() const;
		VertexData& vertexData();
		const VertexData& vertexData() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		VertexData m_vertexData;
};

inline class Normals* Normal::normals() const
{
	return boost::polymorphic_cast<class Normals*>(this->parent());
}

inline VertexData& Normal::vertexData()
{
	return this->m_vertexData;
}

inline const VertexData& Normal::vertexData() const
{
	return this->m_vertexData;
}

}

}

#endif
