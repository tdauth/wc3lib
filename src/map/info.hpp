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

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/// \todo Implement InfoX for expansion.
class Info : public FileFormat
{
	public:
		class Player : public Format
		{
			public:
				BOOST_SCOPED_ENUM_START(Type) /// \todo C++0x : int32
				{
					Human = 1,
					Computer = 2,
					Neutral = 3,
					Rescuable = 4
				};
				BOOST_SCOPED_ENUM_END

				BOOST_SCOPED_ENUM_START(Race) /// \todo C++0x : int32
				{
					Human = 1,
					Orc = 2,
					Undead = 3,
					NightElf = 4
				};
				BOOST_SCOPED_ENUM_END

			protected:
				int32 m_number;
				BOOST_SCOPED_ENUM(Type) m_type;
				BOOST_SCOPED_ENUM(Race) m_race;
				bool m_hasFixedStartPosition;
				string m_name;
				FloatPosition m_position;
				int32 m_allowLowPriorityFlags;
				int32 m_allowHighPriorityFlags;

		};

		class Force : public Format
		{
			public:
				BOOST_SCOPED_ENUM_START(Flags) /// \todo C++0x : int32
				{
					Allied = 0x00000001,
					AlliedVictory = 0x00000002,
					ShareVision = 0x00000004,
					ShareUnitControl = 0x00000010,
					ShareAdvancedUnitControl = 0x00000020
				};
				BOOST_SCOPED_ENUM_END

			protected:
				BOOST_SCOPED_ENUM(Flags) m_flags;
				int32 m_playerMask; // (bit "x"=1 --> player "x" is in this force)
				string m_name;
		};

		class UpgradeAvailability : public Format
		{
			public:
				BOOST_SCOPED_ENUM_START(Availability) /// \todo C++0x : int32
				{
					Unavailable = 0,
					Available = 1,
					Researched = 2
				};
				BOOST_SCOPED_ENUM_END

			protected:
				int32 m_playerMask; // (bit "x"=1 if this change applies for player "x")
				id m_id; // upgrade id (as in UpgradeData.slk)
				int32 m_level; // Level of the upgrade for which the availability is changed (this is actually the level - 1, so 1 => 0)
		};

		class TechAvailability : public Format
		{
			public:
			protected:
				int32 m_playerMask; //  (bit "x"=1 if this change applies for player "x")
				id m_id; // (this can be an item, unit or ability) there's no need for an availability value, if a tech-id is in this list, it means that it's not available
		};

		class RandomUnitTable : public Format
		{
			public:
				class Group : public Format
				{
					public:
						class Position : public Format
						{
							public:
								class Line : public Format
								{
									public:
										typedef std::map<id, Position*> Ids;

									protected:
										int32 m_chance; //: Chance of the unit/item (percentage)
										Ids m_ids; // for each position are the unit/item id's for this line specified this can also be random unit/item ids (see bottom of war3mapUnits.doo definition) a unit/item id of 0x00000000 indicates that no unit/item is created
								};

								typedef boost::shared_ptr<Line> LinePtr;
								typedef std::vector<LinePtr> Lines;

								BOOST_SCOPED_ENUM_START(Type) /// \todo C++0x : int32
								{
									UnitTable = 0,
									BuildingTable = 1,
									ItemTable = 2
								};
								BOOST_SCOPED_ENUM_END

							protected:
								BOOST_SCOPED_ENUM(Type) m_type;
								Lines m_lines;
						};

						typedef boost::shared_ptr<Position> PositionPtr;
						typedef std::vector<PositionPtr> Positions;

					protected:
						string m_name;
						Positions m_positions;
				};

				typedef boost::shared_ptr<Group> GroupPtr;
				typedef std::vector<GroupPtr> Groups;
		};

		typedef boost::shared_ptr<Player> PlayerPtr;
		typedef std::vector<PlayerPtr> Players;
		typedef boost::shared_ptr<Force> ForcePtr;
		typedef std::vector<ForcePtr> Forces;
		typedef boost::shared_ptr<UpgradeAvailability> UpgradeAvailabilityPtr;
		typedef std::vector<UpgradeAvailabilityPtr> UpgradeAvailabilities;
		typedef boost::shared_ptr<TechAvailability> TechAvailabilityPtr;
		typedef std::vector<TechAvailabilityPtr> TechAvailabilities;
		typedef boost::shared_ptr<RandomUnitTable> RandomUnitTablePtr;
		typedef std::vector<RandomUnitTablePtr> RandomUnitTables;

		Info(class W3m *w3m);

		std::streamsize read(InputStream &istream) throw (class Exception);
		std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual id fileId() const;
		virtual int32 latestFileVersion() const;
		virtual const char8* fileName() const;

		virtual int32 version() const;

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
		const float32* cameraBounds() const;
		int32 playableWidth() const;
		int32 playableHeight() const;
		BOOST_SCOPED_ENUM(MapFlags) flags() const;
		byte mainGroundType() const;
		int32 campaignBackgroundIndex() const;
		const string& loadingScreenText() const;
		const string& loadingScreenTitle() const;
		const string& loadingScreenSubtitle() const;
		int32 loadingScreenIndex() const;
		const string& prologueScreenText() const;
		const string& prologueScreenTitle() const;
		const string& prologueScreenSubtitle() const;
		const Players& players() const;
		const Forces& forces() const;
		const UpgradeAvailabilities& upgradeAvailabilities() const;
		const TechAvailabilities& techAvailabilities() const;
		const RandomUnitTables& randomUnitTables() const;

	protected:
		class W3m *m_w3m;
		int32 m_version;
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
		BOOST_SCOPED_ENUM(MapFlags) m_flags;
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

inline id Info::fileId() const
{
	return 0;
}

inline int32 Info::latestFileVersion() const
{
	return 13;
}

inline const char8* Info::fileName() const
{
	return "war3map.w3i";
}

inline int32 Info::version() const
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

}

}

#endif
