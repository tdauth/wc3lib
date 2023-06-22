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

#ifndef WC3LIB_MAP_CUSTOMUNITS_HPP
#define WC3LIB_MAP_CUSTOMUNITS_HPP

/**
 * \defgroup objectdata Object Data
 * Warcraft III allows modification of object data based on meta data information.
 */

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "value.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Customized units for Warcraft III: Reign of Chaos.
 *
 * Warcraft III RoC only (not in TFT), allows you only to customize unit and item data.
 * Later, in Frozen Throne Blizzard introduced general object data modification (\ref CustomObjects).
 * The custom units are usually stored in the file "war3map.w3u" of the map.
 * It contains two unit tables. One with the original units of Warcraft III which have been changed in the map and another with new custom units based on an original unit.
 * Both tables can be accessed via \ref originalTable() and \ref customTable().
 * Each unit in a table is represented by \ref CustomUnits::Unit and contains a list of modified fields of which each is represented by a \ref CustomUnits::Modification instance.
 *
 * \sa CustomObjects
 *
 * \ingroup objectdata
 */
class CustomUnits : public FileFormat
{
	public:

		enum class KnownVersions : uint32 {
			ReignOfChaos = 1,
			FrozenThrone = 2, /// Level/variation data. \ref CustomObjects.
			Reforged = 3 /// Sets. Each object can have multiple sets with modifications.
		};

		/**
		 * Provides access to one single modification of a unit field. Each field has its unique id which is returned by \ref valueId().
		 * All valid unit field ids are stored in "Units\UnitMetaData.slk" of the latest Warcraft MPQ archive.
		 * The field value itself is returned by \ref value() which returns an instance of the boost::variant based class \ref Value.
		 * \sa Unit
		 */
		class Modification : public Format
		{
			public:
				Modification();
				Modification(const Modification &other);
				virtual ~Modification();

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				virtual Modification* clone() const;

				void setValueId(id valueId);
				id valueId() const;
                void setValue(Value value);
				Value& value();
				const Value& value() const;

			protected:
				std::streamsize readList(InputStream &istream, Value::Type type);
				std::streamsize writeList(OutputStream &ostream) const;

				/**
				 * Reads data into \p m_value depending on parameter \p type.
				 */
				std::streamsize readData(InputStream &istream, Value::Type type);
				std::streamsize writeData(OutputStream &ostream, Value::Type type) const;

				id m_id; // from "Units\UnitMetaData.slk"
				Value m_value;
		};

		/**
		 * \brief Represents one single set for a unit which contains modifications.
		 *
		 * Sets have been introduced with version 3 of the custom objects format in Reforged.
		 * Previously, there was only 1 set per unit.
		 *
		 * Use \ref modifications() to access the list.
		 * \sa Modification
		 */
		class Set : public Format
		{
			public:
				/**
				 * \brief List of modifications for one single unit entry.
				 */
				typedef boost::ptr_vector<Modification> Modifications;

				Set(uint32 version);
				Set(const Set &other);
				virtual ~Set();

				virtual Set* clone() const;

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				Modifications& modifications();
				const Modifications& modifications() const;

			protected:
				/**
				 * This internal function must be overwritten to provide another data type for \ref Modification.
				 */
				virtual Modification* createModification() const;

				uint32 m_version;
				int32 m_flag;
				Modifications m_modifications;
		};

		/**
		 * \brief Represents one single unit entry in a table which contains a list of sets.
		 *
		 * If \ref isOriginal() returns true it is not a custom unit.
		 * All custom units are based on an original one whichs id can be got using \ref originalId().
		 * The custom unit's id is returned by \ref customId() which returns 0 for original units.
		 * \sa Set
		 */
		class Unit : public Format
		{
			public:
				/**
				 * \brief List of sets for one single unit entry.
				 */
				typedef boost::ptr_vector<Set> Sets;

				Unit(uint32 version);
				Unit(const Unit &other);
				virtual ~Unit();

				virtual Unit* clone() const;

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				bool isOriginal() { return m_customId == 0; };

				/**
				 * The original ID of the unit.
				 * For custom units this is the ID of the original unit which has been copied and modified.
				 *
				 * For original units this is simply the ID of the unit and the custom ID is 0.
				 *
				 * @{
				 */
				void setOriginalId(id originalId);
				id originalId() const;
				/**
				 * @}
				 */
				void setCustomId(id customId);
				id customId() const;
				Sets& sets();
				const Sets& sets() const;

			protected:
				/**
				 * This internal function must be overwritten to provide another data type for \ref Set.
				 */
				virtual Set* createSet() const;

				uint32 m_version;
				id m_originalId;
				id m_customId;
				Sets m_sets;
		};

		/**
		 * \brief List of unit entries for a custom units file.
		 */
		typedef boost::ptr_vector<Unit> Table;

		/**
		 * Creates new empty custom units.
		 */
		CustomUnits();
		/**
		 * The copy constructor clones the units of \p other.
		 */
		CustomUnits(const CustomUnits &other);
		virtual ~CustomUnits();

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

		virtual const byte* fileName() const override;
		virtual const byte* fileTextId() const override;
		virtual uint32 latestFileVersion() const override;

		/**
		 * The original table contains all unit entries for standard units.
		 *
		 * @{
		 */
		Table& originalTable();
		const Table& originalTable() const;
		/**
		 * @}
		 */
		/**
		 * The custom table contains all unit entries for custom units.
		 * @{
		 */
		Table& customTable();
		const Table& customTable() const;
		/**
		 * @}
		 */

		/**
		 * Clears both tables.
		 */
		void clear();

	protected:
		/**
		 * This internal function must be overwritten to provide another from \ref CustomUnits::Unit derived data type for unit objects.
		 */
		virtual Unit* createUnit() const;

		Table m_originalTable;
		Table m_customTable;
};

inline void CustomUnits::Modification::setValueId(id valueId)
{
	this->m_id = valueId;
}

inline id CustomUnits::Modification::valueId() const
{
	return m_id;
}

inline void CustomUnits::Modification::setValue(Value value)
{
	m_value = value;
}

inline Value& CustomUnits::Modification::value()
{
	return m_value;
}

inline const Value& CustomUnits::Modification::value() const
{
	return m_value;
}

inline CustomUnits::Set::Modifications& CustomUnits::Set::modifications()
{
	return m_modifications;
}

inline const CustomUnits::Set::Modifications& CustomUnits::Set::modifications() const
{
	return m_modifications;
}

inline void CustomUnits::Unit::setOriginalId(id originalId)
{
	this->m_originalId = originalId;
}

inline id CustomUnits::Unit::originalId() const
{
	return m_originalId;
}

inline void CustomUnits::Unit::setCustomId(id customId)
{
	this->m_customId = customId;
}

inline id CustomUnits::Unit::customId() const
{
	return m_customId;
}

inline CustomUnits::Unit::Sets& CustomUnits::Unit::sets()
{
	return m_sets;
}

inline const CustomUnits::Unit::Sets& CustomUnits::Unit::sets() const
{
	return m_sets;
}

inline const byte* CustomUnits::fileName() const
{
	return "war3map.w3u";
}

inline const byte* CustomUnits::fileTextId() const
{
	return "";
}

inline uint32 CustomUnits::latestFileVersion() const
{
	return 1;
}

inline CustomUnits::Table& CustomUnits::originalTable()
{
	return m_originalTable;
}

inline const CustomUnits::Table& CustomUnits::originalTable() const
{
	return m_originalTable;
}

inline CustomUnits::Table& CustomUnits::customTable()
{
	return m_customTable;
}

inline const CustomUnits::Table& CustomUnits::customTable() const
{
	return m_customTable;
}

}

}

#endif
