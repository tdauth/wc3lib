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
 * Warcraft III: Reforged allows multiple sets of modifications per object entry.
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
		 * <li>UnitMetaData.slk - \ref CustomObjects::Type::Units </li>
		 * </ul>
		 *
		 * \ingroup objectdata
		 */
		class Modification : public CustomUnits::Modification
		{
			public:
				Modification(CustomObjects::Type type);
				Modification(const Modification &other);
				virtual ~Modification();

				CustomObjects::Type type() const;

				void setLevel(int32 level);
				/**
				 * Only read for doodads, abilities and upgrades. Doodads use this as variation number.
				 */
				int32 level() const;

				void setData(int32 data);
				/**
				 * Only read for doodads, abilities and upgrades but actually only used by abilities to refer specific raw data:
				 * A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H
				 * In reality this is only used in the ability file for values that are originally stored in one of the Data columns in AbilityData.slk, this int tells the game to which of those columns the value resolves (0 = A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H), for example if the change applies to the column DataA3 the level int will be set to 3 and the data pointer to 0
				 */
				int32 data() const;

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				virtual CustomUnits::Modification* clone() const override;

			protected:
				CustomObjects::Type m_type;
				int32 m_level; // level/variation
				int32 m_data; // A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H
		};

		class Set : public CustomUnits::Set
		{
			public:
				Set(uint32 version, CustomObjects::Type type);
				Set(const Set &other);
				virtual ~Set();

				virtual CustomUnits::Set* clone() const override;

				CustomObjects::Type type() const;

			protected:
				virtual CustomUnits::Modification* createModification() const;

				CustomObjects::Type m_type;
		};

		class Object : public CustomUnits::Unit
		{
			public:
				Object(uint32 version, CustomObjects::Type type);
				Object(const Object &other);
				virtual ~Object();

				virtual Unit* clone() const override;

				CustomObjects::Type type() const;

			protected:
				virtual CustomUnits::Set* createSet() const override;

				CustomObjects::Type m_type;
		};

		CustomObjects(Type type);
		/**
		 * The copy constructor clones the custom objects of \p other.
		 */
		CustomObjects(const CustomObjects &other);
		Type type() const;

		virtual const byte* fileName() const override;
		/**
		 * For The Frozen Throne the latest version became 2 instead of 1.
		 * \note The specification http://wc3lib.org/spec/InsideTheW3M.html states that the version is still 1. According to tests this is wrong.
		 */
		virtual uint32 latestFileVersion() const override;

		/**
		 * .w3u 	Units
		 * .w3t 	Items
		 * .w3b 	Destructables
		 * .w3d 	Doodads
		 * .w3a 	Abilities
		 * .w3h 	Buffs
		 * .w3q 	Upgrades
		 */
		static Type typeByExtension(const std::string &extension);

	protected:
		virtual Unit* createUnit() const override;

		Type m_type;
};

inline CustomObjects::Type CustomObjects::Object::type() const
{
	return this->m_type;
}

inline CustomObjects::Type CustomObjects::Set::type() const
{
	return this->m_type;
}

inline CustomObjects::Type CustomObjects::Modification::type() const
{
	return this->m_type;
}

inline void CustomObjects::Modification::setLevel(int32 level)
{
	this->m_level = level;
}

inline int32 CustomObjects::Modification::level() const
{
	return this->m_level;
}

inline void CustomObjects::Modification::setData(int32 data)
{
	this->m_data = data;
}

inline int32 CustomObjects::Modification::data() const
{
	return this->m_data;
}

inline CustomObjects::Type CustomObjects::type() const
{
	return this->m_type;
}

inline CustomObjects::Type CustomObjects::typeByExtension(const std::string &extension)
{
	//std::cerr << "Extension " << extension << std::endl;

	if (extension == ".w3u")
	{
		return CustomObjects::Type::Units;
	}
	else if (extension == ".w3t")
	{
		return CustomObjects::Type::Items;
	}
	else if (extension == ".w3b")
	{
		return CustomObjects::Type::Destructibles;
	}
	else if (extension == ".w3d")
	{
		return CustomObjects::Type::Doodads;
	}
	else if (extension == ".w3a")
	{
		return CustomObjects::Type::Abilities;
	}
	else if (extension == ".w3h")
	{
		return CustomObjects::Type::Buffs;
	}
	else if (extension == ".w3q")
	{
		return CustomObjects::Type::Upgrades;
	}

	return CustomObjects::Type::Units;
}

}

}

#endif
