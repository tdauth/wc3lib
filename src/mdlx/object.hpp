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

#ifndef WC3LIB_MDLX_OBJECT_HPP
#define WC3LIB_MDLX_OBJECT_HPP

#include "node.hpp"

namespace wc3lib
{

namespace mdlx
{

class Object : public Node
{
	public:
		Object(class Mdlx *mdlx);
		virtual ~Object();

		void setVisibilties(class AttachmentVisibilities *visibilities);
		class AttachmentVisibilities* visibilities() const;

		virtual std::streamsize readMdl(istream &istream);
		virtual std::streamsize writeMdl(ostream &ostream) const;
		virtual std::streamsize readMdx(istream &istream);
		virtual std::streamsize writeMdx(ostream &ostream) const;

	protected:
		class AttachmentVisibilities *m_visibilities; //(KATV)
};

inline void Object::setVisibilties(class AttachmentVisibilities *visibilities)
{
	this->m_visibilities = visibilities;
}

inline class AttachmentVisibilities* Object::visibilities() const
{
	return this->m_visibilities;
}

}

}

#endif
