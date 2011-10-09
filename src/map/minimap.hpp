/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_MINIMAP_HPP
#define WC3LIB_MAP_MINIMAP_HPP

#include "platform.hpp"
#include "../blp.hpp"

namespace wc3lib
{

namespace map
{

class Minimap : public FileFormat, public blp::Blp
{
	public:
		virtual std::streamsize read(InputStream& istream) throw (Exception)
		{
			return blp::Blp::read(istream);
		}

		virtual std::streamsize write(OutputStream& ostream) const throw (Exception)
		{
			return blp::Blp::write(ostream);
		}

		virtual const char8* fileTextId() const;
		virtual const wc3lib::map::char8* fileName() const;
		virtual int32 latestFileVersion() const;
};

inline const char8* Minimap::fileTextId() const
{
	return "";
}

inline const wc3lib::map::char8* Minimap::fileName() const
{
	return "war3mapMap.blp";
}

inline int32 Minimap::latestFileVersion() const
{
	return 0;
}

}

}

#endif
