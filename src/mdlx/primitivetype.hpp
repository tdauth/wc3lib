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

#ifndef WC3LIB_MDLX_PRIMITIVETYPE_HPP
#define WC3LIB_MDLX_PRIMITIVETYPE_HPP

#include "groupmdxblockmember.hpp"
#include "primitivetypes.hpp"
#include "groupmdxblock.hpp"

namespace wc3lib
{

namespace mdlx
{

class PrimitiveType : public GroupMdxBlockMember
{
	public:
		//4   - Triangles
		//??? - Triangle fan
		//??? - Triangle strip
		//??? - Quads
		//??? - Quad strip
		BOOST_SCOPED_ENUM_START(Type) // : long32
		{
			Triangles = 4
		};
		BOOST_SCOPED_ENUM_END

		PrimitiveType(class PrimitiveTypes *primitiveTypes);
		virtual ~PrimitiveType();

		class PrimitiveTypes* primitiveTypes() const;
		BOOST_SCOPED_ENUM(Type) type() const;

		virtual std::streamsize readMdl(std::istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(std::ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(std::istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(std::ostream &ostream) const throw (class Exception);

	protected:
		BOOST_SCOPED_ENUM(Type) m_type;
};

inline class PrimitiveTypes* PrimitiveType::primitiveTypes() const
{
	return dynamic_cast<class PrimitiveTypes*>(this->m_parent);
}

inline BOOST_SCOPED_ENUM(PrimitiveType::Type) PrimitiveType::type() const
{
	return this->m_type;
}

}

}

#endif
