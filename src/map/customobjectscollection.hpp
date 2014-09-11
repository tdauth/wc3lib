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

#ifndef WC3LIB_MAP_CUSTOMOBJECTSCOLLECTION_HPP
#define WC3LIB_MAP_CUSTOMOBJECTSCOLLECTION_HPP

#include <boost/scoped_ptr.hpp>

#include "platform.hpp"
#include "customobjects.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Collection of various customized object tables.
 * Usually used to export all custom map objects.
 * \sa CustomObjects
 */
class CustomObjectsCollection : public Format
{
	public:
		typedef boost::scoped_ptr<CustomObjects> CustomObjectsPtr;

		CustomObjectsCollection();

		std::streamsize read(InputStream &istream);
		std::streamsize write(OutputStream &ostream) const;
		
		void clear();

		virtual uint32 latestFileVersion() const;
		virtual const byte* fileExtension() const;

		virtual uint32 version() const;
		bool hasUnits() const;
		CustomObjectsPtr& units();
		const CustomObjectsPtr& units() const;
		bool hasItems() const;
		CustomObjectsPtr& items();
		const CustomObjectsPtr& items() const;
		bool hasDestructibles() const;
		CustomObjectsPtr& destructibles();
		const CustomObjectsPtr& destructibles() const;
		bool hasDoodads() const;
		CustomObjectsPtr& doodads();
		const CustomObjectsPtr& doodads() const;
		bool hasAbilities() const;
		CustomObjectsPtr& abilities();
		const CustomObjectsPtr& abilities() const;
		bool hasBuffs() const;
		CustomObjectsPtr& buffs();
		const CustomObjectsPtr& buffs() const;
		bool hasUpgrades() const;
		CustomObjectsPtr& upgrades();
		const CustomObjectsPtr& upgrades() const;

	protected:
		uint32 m_version;
		CustomObjectsPtr m_units;
		CustomObjectsPtr m_items;
		CustomObjectsPtr m_destructibles;
		CustomObjectsPtr m_doodads;
		CustomObjectsPtr m_abilities;
		CustomObjectsPtr m_buffs;
		CustomObjectsPtr m_upgrades;
};

inline uint32 CustomObjectsCollection::latestFileVersion() const
{
	return 1;
}

inline const byte* CustomObjectsCollection::fileExtension() const
{
	return ".w3o";
}

inline uint32 CustomObjectsCollection::version() const
{
	return this->m_version;
}

inline bool CustomObjectsCollection::hasUnits() const
{
	return this->units().get() != 0;
}

inline CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::units()
{
	return this->m_units;
}

inline const CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::units() const
{
	return this->m_units;
}

inline bool CustomObjectsCollection::hasItems() const
{
	return this->items().get() != 0;
}

inline CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::items()
{
	return this->m_items;
}

inline const CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::items() const
{
	return this->m_items;
}

inline bool CustomObjectsCollection::hasDestructibles() const
{
	return this->destructibles().get() != 0;
}

inline CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::destructibles()
{
	return this->m_destructibles;
}

inline const CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::destructibles() const
{
	return this->m_destructibles;
}

inline bool CustomObjectsCollection::hasDoodads() const
{
	return this->doodads().get() != 0;
}

inline CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::doodads()
{
	return this->m_doodads;
}

inline const CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::doodads() const
{
	return this->m_doodads;
}

inline bool CustomObjectsCollection::hasAbilities() const
{
	return this->abilities().get() != 0;
}

inline CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::abilities()
{
	return this->m_abilities;
}

inline const CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::abilities() const
{
	return this->m_abilities;
}

inline bool CustomObjectsCollection::hasBuffs() const
{
	return this->buffs().get() != 0;
}

inline CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::buffs()
{
	return this->m_buffs;
}

inline const CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::buffs() const
{
	return this->m_buffs;
}

inline bool CustomObjectsCollection::hasUpgrades() const
{
	return this->upgrades().get() != 0;
}

inline CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::upgrades()
{
	return this->m_upgrades;
}

inline const CustomObjectsCollection::CustomObjectsPtr& CustomObjectsCollection::upgrades() const
{
	return this->m_upgrades;
}

}

}

#endif
