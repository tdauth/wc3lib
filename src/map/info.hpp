/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_INFO_HPP
#define WC3LIB_MAP_INFO_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief The info file ("war3map.w3i") covers basic map information such as players, forces, available researches and random unit tables of the map.
 *
 * This class supports the format for Warcraft III: Reign of Chaos.
 * All types like players or forces are supported by nested classes.
 *
 * \todo Implement InfoX for expansion.
 */
class Info : public FileFormat
{
	public:
		class Player : public Format
		{
			public:
				typedef Vertex2d<float32> Position;

				enum class Type : int32
				{
					Human = 1,
					Computer = 2,
					Neutral = 3,
					Rescuable = 4
				};

				enum class Race : int32
				{
					Human = 1,
					Orc = 2,
					Undead = 3,
					NightElf = 4
				};

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				int32 number() const;
				Type type() const;
				Race race() const;
				bool hasFixedStartPosition() const;
				const string& name() const;
				const Position& position() const;
				int32 allowLowPriorityFlags() const;
				int32 allowHighPriorityFlags() const;

			protected:
				int32 m_number;
				Type m_type;
				Race m_race;
				bool m_hasFixedStartPosition; /// \ref int32
				string m_name;
				Position m_position;
				int32 m_allowLowPriorityFlags;
				int32 m_allowHighPriorityFlags;

		};

		class Force : public Format
		{
			public:
				enum class Flags : int32
				{
					Allied = 0x00000001,
					AlliedVictory = 0x00000002,
					ShareVision = 0x00000004,
					ShareUnitControl = 0x00000010,
					ShareAdvancedUnitControl = 0x00000020
				};

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				Flags flags() const;
				int32 playerMask() const;
				const string& name() const;

			protected:
				Flags m_flags;
				int32 m_playerMask; // (bit "x"=1 --> player "x" is in this force)
				string m_name;
		};

		class UpgradeAvailability : public Format
		{
			public:
				enum class Availability : int32
				{
					Unavailable = 0,
					Available = 1,
					Researched = 2
				};

				UpgradeAvailability();

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				int32 playerMask() const;
				id upgrade() const;
				int32 level() const;
				Availability availability() const;

			protected:
				int32 m_playerMask; // (bit "x"=1 if this change applies for player "x")
				id m_upgrade; // upgrade id (as in UpgradeData.slk)
				int32 m_level; // Level of the upgrade for which the availability is changed (this is actually the level - 1, so 1 => 0)
				Availability m_availability;
		};

		/**
		 * We do not need to specify the availability.
		 * If it is in the list that means "not available".
		 */
		class TechAvailability : public Format
		{
			public:
				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				int32 playerMask() const;
				id tech() const;

			protected:
				int32 m_playerMask; //  (bit "x"=1 if this change applies for player "x")
				id m_tech; // (this can be an item, unit or ability) there's no need for an availability value, if a tech-id is in this list, it means that it's not available
		};

		class RandomUnitTable : public Format
		{
			public:
				class Group : public Format
				{
					public:
						class Column : public Format
						{
							public:
								typedef std::vector<id> Rows;

								enum class Type : int32
								{
									UnitTable = 0,
									BuildingTable = 1,
									ItemTable = 2
								};

								Column();

								virtual std::streamsize read(InputStream &istream) override;
								virtual std::streamsize write(OutputStream &ostream) const override;

								void setType(Type type);
								Type type() const;
								/**
								 * Each row corresponds to one chance. Therefore rows do always have the size of \ref Group::chances().size().
								 */
								Rows& rows();
								const Rows& rows() const;

							protected:
								Type m_type;
								Rows m_rows;
						};

						typedef boost::ptr_vector<Column> Columns;
						/**
						 * Chances are set per row.
						 */
						typedef std::vector<int32> Chances;

						virtual std::streamsize read(InputStream &istream) override;
						virtual std::streamsize write(OutputStream &ostream) const override;

						int32 number() const;
						const string& name() const;
						Columns& columns();
						const Columns& columns() const;
						Chances& chances();
						const Chances& chances() const;

					protected:
						int32 m_number;
						string m_name;
						Columns m_columns;
						Chances m_chances;
				};

				typedef boost::ptr_vector<Group> Groups;

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

				Groups& groups();
				const Groups& groups() const;

			protected:
				Groups m_groups;
		};

		typedef boost::ptr_vector<Player> Players;
		typedef boost::ptr_vector<Force> Forces;
		typedef boost::ptr_vector<UpgradeAvailability> UpgradeAvailabilities;
		typedef boost::ptr_vector<TechAvailability> TechAvailabilities;
		typedef boost::ptr_vector<RandomUnitTable> RandomUnitTables;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

		virtual const byte* fileTextId() const override;
		virtual uint32 latestFileVersion() const override;
		virtual const byte* fileName() const override;

		virtual uint32 version() const override;

		int32 mapVersion() const;
		int32 editorVersion() const;
		const string& name() const;
		const string& author() const;
		const string& description() const;
		const string& playersRecommended() const;
		/**
		 * \return Returns camera bounds array with size of 8.
		 */
		const float32* cameraBoundsJass() const;
		/**
		 * \return Returns camera bounds array with size of 4.
		 */
		const int32* cameraBounds() const;
		int32 playableWidth() const;
		int32 playableHeight() const;
		MapFlags flags() const;
		byte mainGroundType() const;
		int32 campaignBackgroundIndex() const;
		const string& loadingScreenText() const;
		const string& loadingScreenTitle() const;
		const string& loadingScreenSubtitle() const;
		int32 loadingScreenIndex() const;
		const string& prologueScreenText() const;
		const string& prologueScreenTitle() const;
		const string& prologueScreenSubtitle() const;
		Players& players();
		const Players& players() const;
		Forces& forces();
		const Forces& forces() const;
		UpgradeAvailabilities& upgradeAvailabilities();
		const UpgradeAvailabilities& upgradeAvailabilities() const;
		TechAvailabilities& techAvailabilities();
		const TechAvailabilities& techAvailabilities() const;
		RandomUnitTables& randomUnitTables();
		const RandomUnitTables& randomUnitTables() const;

		/**
		 * Empties players, forces, availabilities and tables.
		 */
		void clear();

		/**
		 * Calculates the actual integer value by using \ref cameraBounds() and \ref playableWidth() or \ref playableHeight().
		 * The actual width for example is: camera bound[0] + playable width + camera bound[1]
		 * The actual height is: camera bound[2] + playable height + camera bound[3]
		 *
		 * @{
		 */
		int32 calculateMapWidth() const;
		int32 calculateMapHeight() const;
		/**
		 * @}
		 */

	protected:
		int32 m_mapVersion; // number of saves (map version)
		int32 m_editorVersion; // editor version (little endian)
		string m_name;
		string m_author;
		string m_description;
		string m_playersRecommended;
		float32 m_cameraBoundsJass[8];
		int32 m_cameraBounds[4];
		int32 m_playableWidth;
		int32 m_playableHeight;
		MapFlags m_flags;
		byte m_mainGroundType;
		int32 m_campaignBackgroundIndex;
		string m_loadingScreenText;
		string m_loadingScreenTitle;
		string m_loadingScreenSubtitle;
		int32 m_loadingScreenIndex;
		string m_prologueScreenText;
		string m_prologueScreenTitle;
		string m_prologueScreenSubtitle;
		Players m_players;
		Forces m_forces;
		UpgradeAvailabilities m_upgradeAvailabilities;
		TechAvailabilities m_techAvailabilities;
		RandomUnitTables m_randomUnitTables;
};

inline int32 Info::Player::number() const
{
	return m_number;
}

inline Info::Player::Type Info::Player::type() const
{
	return m_type;
}

inline Info::Player::Race Info::Player::race() const
{
	return m_race;
}

inline bool Info::Player::hasFixedStartPosition() const
{
	return m_hasFixedStartPosition;
}

inline const string& Info::Player::name() const
{
	return m_name;
}

inline const Info::Player::Position& Info::Player::position() const
{
	return m_position;
}

inline int32 Info::Player::allowLowPriorityFlags() const
{
	return m_allowLowPriorityFlags;
}

inline int32 Info::Player::allowHighPriorityFlags() const
{
	return m_allowHighPriorityFlags;
}


inline Info::Force::Flags Info::Force::flags() const
{
	return m_flags;
}

inline int32 Info::Force::playerMask() const
{
	return m_playerMask;
}

inline const string& Info::Force::name() const
{
	return m_name;
}

inline int32 Info::UpgradeAvailability::playerMask() const
{
	return m_playerMask;
}

inline id Info::UpgradeAvailability::upgrade() const
{
	return m_upgrade;
}

inline int32 Info::UpgradeAvailability::level() const
{
	return m_level;
}

inline Info::UpgradeAvailability::Availability Info::UpgradeAvailability::availability() const
{
	return m_availability;
}

inline int32 Info::TechAvailability::playerMask() const
{
	return m_playerMask;
}

inline id Info::TechAvailability::tech() const
{
	return m_tech;
}

inline void Info::RandomUnitTable::Group::Column::setType(Info::RandomUnitTable::Group::Column::Type type)
{
	m_type = type;
}

inline Info::RandomUnitTable::Group::Column::Type Info::RandomUnitTable::Group::Column::type() const
{
	return m_type;
}

inline Info::RandomUnitTable::Group::Column::Rows& Info::RandomUnitTable::Group::Column::rows()
{
	return m_rows;
}

inline const Info::RandomUnitTable::Group::Column::Rows& Info::RandomUnitTable::Group::Column::rows() const
{
	return m_rows;
}

inline int32 Info::RandomUnitTable::Group::number() const
{
	return m_number;
}

inline const string& Info::RandomUnitTable::Group::name() const
{
	return m_name;
}

inline Info::RandomUnitTable::Group::Columns& Info::RandomUnitTable::Group::columns()
{
	return m_columns;
}

inline const Info::RandomUnitTable::Group::Columns& Info::RandomUnitTable::Group::columns() const
{
	return m_columns;
}

inline Info::RandomUnitTable::Group::Chances& Info::RandomUnitTable::Group::chances()
{
	return m_chances;
}

inline const Info::RandomUnitTable::Group::Chances& Info::RandomUnitTable::Group::chances() const
{
	return m_chances;
}

inline const Info::RandomUnitTable::Groups& Info::RandomUnitTable::groups() const
{
	return m_groups;
}

inline const byte* Info::fileTextId() const
{
	return "";
}

inline uint32 Info::latestFileVersion() const
{
	return 18;
}

inline const byte* Info::fileName() const
{
	return "war3map.w3i";
}

inline uint32 Info::version() const
{
	return m_version;
}

inline int32 Info::mapVersion() const
{
	return m_mapVersion;
}

inline int32 Info::editorVersion() const
{
	return m_editorVersion;
}

inline const string& Info::name() const
{
	return m_name;
}

inline const string& Info::author() const
{
	return m_author;
}

inline const string& Info::description() const
{
	return m_description;
}

inline const string& Info::playersRecommended() const
{
	return m_playersRecommended;
}

inline const float32* Info::cameraBoundsJass() const
{
	return m_cameraBoundsJass;
}

inline const int32* Info::cameraBounds() const
{
	return m_cameraBounds;
}

inline int32 Info::playableWidth() const
{
	return m_playableWidth;
}

inline int32 Info::playableHeight() const
{
	return m_playableHeight;
}

inline MapFlags Info::flags() const
{
	return m_flags;
}

inline byte Info::mainGroundType() const
{
	return m_mainGroundType;
}

inline int32 Info::campaignBackgroundIndex() const
{
	return m_campaignBackgroundIndex;
}

inline const string& Info::loadingScreenText() const
{
	return m_loadingScreenText;
}

inline const string& Info::loadingScreenTitle() const
{
	return m_loadingScreenTitle;
}

inline const string& Info::loadingScreenSubtitle() const
{
	return m_loadingScreenSubtitle;
}

inline int32 Info::loadingScreenIndex() const
{
	return m_loadingScreenIndex;
}

inline const string& Info::prologueScreenText() const
{
	return m_prologueScreenText;
}

inline const string& Info::prologueScreenTitle() const
{
	return m_prologueScreenTitle;
}

inline const string& Info::prologueScreenSubtitle() const
{
	return m_prologueScreenSubtitle;
}

inline Info::Players& Info::players()
{
	return this->m_players;
}

inline const Info::Players& Info::players() const
{
	return m_players;
}

inline Info::Forces& Info::forces()
{
	return this->m_forces;
}

inline const Info::Forces& Info::forces() const
{
	return m_forces;
}

inline Info::UpgradeAvailabilities& Info::upgradeAvailabilities()
{
	return this->m_upgradeAvailabilities;
}

inline const Info::UpgradeAvailabilities& Info::upgradeAvailabilities() const
{
	return m_upgradeAvailabilities;
}

inline Info::TechAvailabilities& Info::techAvailabilities()
{
	return this->m_techAvailabilities;
}

inline const Info::TechAvailabilities& Info::techAvailabilities() const
{
	return m_techAvailabilities;
}

inline Info::RandomUnitTables& Info::randomUnitTables()
{
	return this->m_randomUnitTables;
}

inline const Info::RandomUnitTables& Info::randomUnitTables() const
{
	return m_randomUnitTables;
}

}

}

#endif
