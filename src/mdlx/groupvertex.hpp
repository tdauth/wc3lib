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

#ifndef WC3LIB_MDLX_GROUPVERTEX_HPP
#define WC3LIB_MDLX_GROUPVERTEX_HPP

#include "groupmdxblockmember.hpp"
#include "groupvertices.hpp"

namespace wc3lib
{

namespace mdlx
{

class GroupVertex : public GroupMdxBlockMember
{
	public:
		GroupVertex(class GroupVertices *groupVertices);
		virtual ~GroupVertex();

		class GroupVertices* groupVertices() const;
		byte data() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

	protected:
		byte m_data;
};

inline class GroupVertices* GroupVertex::groupVertices() const
{
	return boost::polymorphic_cast<class GroupVertices*>(this->m_parent);
}

inline byte GroupVertex::data() const
{
	return this->m_data;
}

}

}

#endif
