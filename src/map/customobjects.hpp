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

#ifndef WC3LIB_MAP_CUSTOMOBJECTS_HPP
#define WC3LIB_MAP_CUSTOMOBJECTS_HPP

#include "customunits.hpp"

namespace wc3lib
{

namespace map
{

/**
* Warcraft 3 The Frozen Throne allows you to customize every possible object data using Warcraft 3's unit data format with some minor changes.
* Corresponding type of data is stored dynamically (\ref CustomObjects::Type).
*/
class CustomObjects : public CustomUnits
{
	public:
		BOOST_SCOPED_ENUM_START(Type)
		{
			Units,
			Items,
			Destructibles,
			Doodads,
			Abilities,
			Buffs,
			Upgrades
		};
		BOOST_SCOPED_ENUM_END

		class Modification : public CustomUnits::Modification
		{
			public:
				Modification(BOOST_SCOPED_ENUM(CustomObjects::Type) type);
				BOOST_SCOPED_ENUM(CustomObjects::Type) type() const;
				/**
				* Only read for doodads, abilities and upgrades. Doodads use this as variation number.
				*/
				int32 level() const;
				/**
				* Only read for doodads, abilities and upgrades but actually only used by abilities to refer specific raw data:
				* A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H
				*/
				int32 data() const;

				std::streamsize read(InputStream &istream) throw (class Exception);
				std::streamsize write(OutputStream &ostream) const throw (class Exception);

			protected:
				BOOST_SCOPED_ENUM(CustomObjects::Type) m_type;
				int32 m_level; // level/variation
				int32 m_data; // A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H
		};

		class Object : public CustomUnits::Unit
		{
			public:
				Object(BOOST_SCOPED_ENUM(CustomObjects::Type) type);
				BOOST_SCOPED_ENUM(CustomObjects::Type) type() const;

			protected:
				virtual CustomUnits::Modification* createModification() const;

				BOOST_SCOPED_ENUM(CustomObjects::Type) m_type;
		};

		CustomObjects(BOOST_SCOPED_ENUM(Type) type);
		BOOST_SCOPED_ENUM(Type) type() const;

		virtual const char8* fileName() const;

	protected:
		virtual Unit* createUnit() const;

		BOOST_SCOPED_ENUM(Type) m_type;
};

inline BOOST_SCOPED_ENUM(CustomObjects::Type) CustomObjects::Object::type() const
{
	return this->m_type;
}

inline BOOST_SCOPED_ENUM(CustomObjects::Type) CustomObjects::Modification::type() const
{
	return this->m_type;
}

inline int32 CustomObjects::Modification::level() const
{
	return this->m_level;
}

inline int32 CustomObjects::Modification::data() const
{
	return this->m_data;
}

inline BOOST_SCOPED_ENUM(CustomObjects::Type) CustomObjects::type() const
{
	return this->m_type;
}

}

}

#endif
