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
 * Warcraft III: The Frozen Throne allows you to customize every possible object data using Warcraft III's unit data format with some minor changes.
 * Corresponding type of data is stored dynamically (\ref CustomObjects::Type).
 *
 * \sa CustomUnits
 * \sa CustomObjectsCollection
 * \ingroup objectdata
 */
class CustomObjects : public CustomUnits
{
	public:
		enum class Type : int32
		{
			Units,
			Items,
			Destructibles,
			Doodads,
			Abilities,
			Buffs,
			Upgrades
		};

		/**
		 * \brief Single value modfication entry.
		 *
		 * The value id of a modification can be accessed via \ref valueId(). It is the corresponding value of the SLK file entry.
		 * For each type of modification there exists a meta data SLK file which contains information about the field to which
		 * the modification belongs. This information is mostly required by the World Editor because it specifies which values
		 * can be defined by the user.
		 *
		 * This is a list of all meta data SLK files from The Frozen Throne:
		 * <ul>
		 * <li>UnitMetaData.slk - \ref CustomObjects::Type::Unit </li>
		 * </ul>
		 *
		 * \ingroup objectdata
		 */
		class Modification : public CustomUnits::Modification
		{
			public:
				Modification(CustomObjects::Type type);
				CustomObjects::Type type() const;
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
				CustomObjects::Type m_type;
				int32 m_level; // level/variation
				int32 m_data; // A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H
		};

		class Object : public CustomUnits::Unit
		{
			public:
				Object(CustomObjects::Type type);
				CustomObjects::Type type() const;

			protected:
				/// \todo C++11 override
				virtual CustomUnits::Modification* createModification() const;

				CustomObjects::Type m_type;
		};

		CustomObjects(Type type);
		Type type() const;

		virtual const byte* fileName() const;
		/**
		 * For The Frozen Throne the latest version became 2 instead of 1.
		 * \note The specification http://wc3lib.org/spec/InsideTheW3M.html states that the version is still 1. According to tests this is wrong.
		 */
		virtual uint32 latestFileVersion() const;

	protected:
		/// \todo C++11 override
		virtual Unit* createUnit() const;

		Type m_type;
};

inline CustomObjects::Type CustomObjects::Object::type() const
{
	return this->m_type;
}

inline CustomObjects::Type CustomObjects::Modification::type() const
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

inline CustomObjects::Type CustomObjects::type() const
{
	return this->m_type;
}

}

}

#endif
