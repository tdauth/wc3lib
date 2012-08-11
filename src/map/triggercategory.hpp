/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_TRIGGERCATEGORY_HPP
#define WC3LIB_MAP_TRIGGERCATEGORY_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \todo Add derived class TriggerCategoryX.
 * \sa TriggerCategoryX
 */
class TriggerCategory : public Format
{
	public:
		TriggerCategory();

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		int32 index() const;
		const string& name() const;

	protected:
		int32 m_index;
		string m_name;
};

inline const string& TriggerCategory::name() const
{
	return this->m_name;
}

inline int32 TriggerCategory::index() const
{
	return this->m_index;
}

}

}

#endif
