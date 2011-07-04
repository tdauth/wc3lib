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

#ifndef WC3LIB_MDLX_PRIMITIVESIZE_HPP
#define WC3LIB_MDLX_PRIMITIVESIZE_HPP

#include "groupmdxblockmember.hpp"
#include "primitivesizes.hpp"

namespace wc3lib
{

namespace mdlx
{

class PrimitiveSize : public GroupMdxBlockMember
{
	public:
		PrimitiveSize(class PrimitiveSizes *primitiveSizes);
		virtual ~PrimitiveSize();

		class PrimitiveSizes* primitiveSizes() const;
		long32 value() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		long32 m_value;
};

inline class PrimitiveSizes* PrimitiveSize::primitiveSizes() const
{
	return boost::polymorphic_cast<class PrimitiveSizes*>(this->parent());
}

inline long32 PrimitiveSize::value() const
{
	return this->m_value;
}

}

}

#endif
