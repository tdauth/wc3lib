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

#ifndef WC3LIB_EDITOR_MAP_HPP
#define WC3LIB_EDITOR_MAP_HPP

#include "resource.hpp"
#include "platform.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

class Map : public Resource
{
	public:
		typedef boost::scoped_ptr<map::W3m> MapPtr;

		Map(const KUrl &url);

		virtual void load() throw (Exception);
		virtual void reload() throw (Exception);
		virtual void save(const KUrl &url) const throw (Exception);

		bool isW3x() const;

		const MapPtr& map() const;
		MapPtr& map();

	private:
		MapPtr m_map;
};

inline bool Map::isW3x() const
{
	if (m_map.get() == 0)
		return false;

	return typeid(*m_map.get()) == typeid(map::W3x);
}

inline const Map::MapPtr& Map::map() const
{
	return m_map;
}

inline Map::MapPtr& Map::map()
{
	return m_map;
}

}

}

#endif
