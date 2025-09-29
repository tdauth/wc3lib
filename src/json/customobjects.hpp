/***************************************************************************
 *   Copyright (C) 2025 by Tamino Dauth                                    *
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

#ifndef WC3LIB_JSON_CUSTOMOBJECTS_HPP
#define WC3LIB_JSON_CUSTOMOBJECTS_HPP

#include "../format.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace json
{

class CustomObjects : public Format
{
	public:
		CustomObjects(map::CustomObjects *customObjects);
		virtual ~CustomObjects();

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		map::CustomObjects *m_customObjects;
};

}

}

#endif
