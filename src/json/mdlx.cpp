/***************************************************************************
 *   Copyright (C) 2024 by Tamino Dauth                                    *
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
#include <boost/json/src.hpp>

#include "mdlx.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

wc3lib::mdlx::Model tag_invoke(const boost::json::value_to_tag<wc3lib::mdlx::Model>&, const boost::json::value &v) {
    auto& o = v.as_object();

    wc3lib::mdlx::Model r;
    r.setName(value_to<std::string>(o.at("Name")));
    r.setAnimationFileName(value_to<std::string>(o.at("AnimationFileName")));
    r.setBlendTime(value_to<long32>(o.at("BlendTime")));

    return r;
}

void tag_invoke(const boost::json::value_from_tag&, boost::json::value& v, const wc3lib::mdlx::Model &x)
{
    v = boost::json::object{
        {"Name", x.name()},
        {"AnimationFileName", x.animationFileName()},
        {"BlendTime", x.blendTime()},
    };
}

}

}

namespace wc3lib
{

namespace json
{

Mdlx::Mdlx(mdlx::Mdlx *model) : m_model(model)
{
}

Mdlx::~Mdlx()
{
}

std::streamsize Mdlx::read(InputStream &istream)
{
	auto v = boost::json::parse(istream);

	return 0;
}

std::streamsize Mdlx::write(OutputStream &ostream) const
{
    ostream << boost::json::value_from(m_model->model()) << std::endl;

    return 0;
}

}

}