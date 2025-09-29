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

#include <boost/json.hpp>

#include "customobjects.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace map
{

CustomObjects tag_invoke(const boost::json::value_to_tag<CustomObjects>&, const boost::json::value &v) {
    auto& o = v.as_object();

    CustomObjects r(CustomObjects::Type::Units);
    r.setType(static_cast<CustomObjects::Type>(value_to<int32>(o.at("type"))));

    return r;
}

void tag_invoke(const boost::json::value_from_tag&, boost::json::value& v, const CustomObjects &x)
{
    v = boost::json::object{
        {"type", static_cast<int32>(x.type())},
    };
}

}

}

namespace wc3lib
{

namespace json
{

CustomObjects::CustomObjects(map::CustomObjects *customObjects) : m_customObjects(customObjects)
{
}

CustomObjects::~CustomObjects()
{
}

std::streamsize CustomObjects::read(InputStream &istream)
{
	auto v = boost::json::parse(istream);

	return 0;
}

std::streamsize CustomObjects::write(OutputStream &ostream) const
{
    ostream << boost::json::value_from(*m_customObjects) << std::endl;

    return 0;
}

}

}