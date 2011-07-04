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

#include "platform.hpp"

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
		CustomObjectsCollection();
		~CustomObjectsCollection();

		std::streamsize read(InputStream &istream) throw (class Exception);
		std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual int32 latestFileVersion() const;
		virtual const char8* fileExtension() const;

		virtual int32 version() const;
		bool hasUnits() const;
		class CustomObjects* units() const;
		bool hasItems() const;
		class CustomObjects* items() const;
		bool hasDestructibles() const;
		class CustomObjects* destructibles() const;
		bool hasDoodads() const;
		class CustomObjects* doodads() const;
		bool hasAbilities() const;
		class CustomObjects* abilities() const;
		bool hasBuffs() const;
		class CustomObjects* buffs() const;
		bool hasUpgrades() const;
		class CustomObjects* upgrades() const;

	protected:
		int32 m_version;
		class CustomObjects *m_units;
		class CustomObjects *m_items;
		class CustomObjects *m_destructibles;
		class CustomObjects *m_doodads;
		class CustomObjects *m_abilities;
		class CustomObjects *m_buffs;
		class CustomObjects *m_upgrades;
};

inline int32 CustomObjectsCollection::latestFileVersion() const
{
	return 1;
}

inline const char8* CustomObjectsCollection::fileExtension() const
{
	return "w3o";
}

inline int32 CustomObjectsCollection::version() const
{
	return this->m_version;
}

inline bool CustomObjectsCollection::hasUnits() const
{
	return this->m_units != 0;
}

inline class CustomObjects* CustomObjectsCollection::units() const
{
	return this->m_units;
}

inline bool CustomObjectsCollection::hasItems() const
{
	return this->m_items != 0;
}

inline class CustomObjects* CustomObjectsCollection::items() const
{
	return this->m_items;
}

inline bool CustomObjectsCollection::hasDestructibles() const
{
	return this->m_destructibles != 0;
}

inline class CustomObjects* CustomObjectsCollection::destructibles() const
{
	return this->m_destructibles;
}

inline bool CustomObjectsCollection::hasDoodads() const
{
	return this->m_doodads != 0;
}

inline class CustomObjects* CustomObjectsCollection::doodads() const
{
	return this->m_doodads;
}

inline bool CustomObjectsCollection::hasAbilities() const
{
	return this->m_abilities != 0;
}

inline class CustomObjects* CustomObjectsCollection::abilities() const
{
	return this->m_abilities;
}

inline bool CustomObjectsCollection::hasBuffs() const
{
	return this->m_buffs != 0;
}

inline class CustomObjects* CustomObjectsCollection::buffs() const
{
	return this->m_buffs;
}

inline bool CustomObjectsCollection::hasUpgrades() const
{
	return this->m_upgrades != 0;
}

inline class CustomObjects* CustomObjectsCollection::upgrades() const
{
	return this->m_upgrades;
}

}

}

#endif
