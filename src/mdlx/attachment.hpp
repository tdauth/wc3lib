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

#ifndef WC3LIB_MDLX_ATTACHMENT_HPP
#define WC3LIB_MDLX_ATTACHMENT_HPP

#include "object.hpp"
#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Attachment".
 */
class Attachment : public Object
{
	public:
		static const std::size_t pathSize = 0x100;

		Attachment();

		/**
		 * \return Returns path with length of \ref pathSize.
		 */
		const byte* path() const;
		long32 unknown0() const; // TODO maybe static visibility?
		long32 attachmentId() const;
		const Alphas& visibilities() const;

	protected:
		byte m_path[pathSize];
		long32 m_unknown0;
		long32 m_attachmentId;
		Alphas m_visibilities; //(KATV)
};

inline const byte* Attachment::path() const
{
	return this->m_path;
}

inline long32 Attachment::unknown0() const
{
	return this->m_unknown0;
}

inline long32 Attachment::attachmentId() const
{
	return this->m_attachmentId;
}

inline const Alphas& Attachment::visibilities() const
{
	return this->m_visibilities;
}

}

}

#endif
