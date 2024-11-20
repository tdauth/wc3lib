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

#ifndef WC3LIB_JSON_PLATFORM_HPP
#define WC3LIB_JSON_PLATFORM_HPP

#include <iostream>

#include "../platform.hpp"
#include "../mdlx.hpp"

namespace wc3lib
{

namespace json
{

inline void writeJsonKeyObjectStart(Format::OutputStream &ostream)
{
	ostream << "{" << std::endl;
}

inline void writeJsonKeyObjectStart(Format::OutputStream &ostream, const string &key)
{
	ostream << '\"' << key << "\": {" << std::endl;
}

inline void writeJsonKeyObjectEnd(Format::OutputStream &ostream)
{
	ostream << "}" << std::endl;
}

inline void writeJsonKeyValue(Format::OutputStream &ostream, const string &key, const string &value)
{
	ostream << '\"' << key << "\": " << "\"" << value << "\"," << std::endl;
}

inline void writeJsonKeyValue(Format::OutputStream &ostream, const string &key, const mdlx::long32 &value)
{
	ostream << '\"' << key << "\": " << " " << value << "," << std::endl;
}


inline void writeJsonValue(Format::OutputStream &ostream, const float32 &value)
{
	ostream << value << "," << std::endl;
}

inline void writeJsonKeyValue(Format::OutputStream &ostream, const mdlx::Bounds &bounds)
{
    writeJsonKeyValue(ostream, "BoundsRadius", bounds.boundsRadius());
    writeJsonKeyObjectStart(ostream, "MinimumExtent");
    writeJsonValue(ostream, bounds.minimumExtent().x());
    writeJsonValue(ostream, bounds.minimumExtent().y());
    writeJsonValue(ostream, bounds.minimumExtent().z());
    writeJsonKeyObjectEnd(ostream);
    writeJsonKeyObjectStart(ostream, "MaximumExtent");
    writeJsonValue(ostream, bounds.maximumExtent().x());
    writeJsonValue(ostream, bounds.maximumExtent().y());
    writeJsonValue(ostream, bounds.maximumExtent().z());
    writeJsonKeyObjectEnd(ostream);
}


}

}

#endif
