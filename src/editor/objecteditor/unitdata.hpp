/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_UNITDATA_HPP
#define WC3LIB_EDITOR_UNITDATA_HPP

#include <QWidget>

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class MetaData;

/**
 * \brief Stores all data for custom and standard units and unit meta data.
 *
 * Uses several .txt files for the different races as sources for the default field data.
 * Other unit data such as UI data, balance data etc. is split up into sepa .slk files.
 *
 * \ingroup objectdata
 */
class KDE_EXPORT UnitData : public ObjectData
{
	public:
		typedef QScopedPointer<MetaData> MetaDataPtr;

		UnitData(MpqPriorityList *source, QObject *parent = 0);

		virtual void load(QWidget *widget) override;

		virtual StandardObjecIds standardObjectIds() const override;
		virtual bool hasDefaultFieldValue(const QString &objectId, const QString &fieldId) const override;
		virtual QString defaultFieldValue(const QString &objectId, const QString &fieldId) const override;
		virtual bool hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const override;

		virtual MetaData* metaData() const override;
		virtual MetaData* objectTabData() const override;

		virtual bool hasCustomUnits() const override;
		virtual bool hasCustomObjects() const override;
		virtual bool hasMetaDataList() const override;

		virtual MetaDataList metaDataList() const override;

		virtual map::CustomObjects::Type type() const override;

		virtual QString objectName(const QString &originalObjectId, const QString &customObjectId) const override;

		bool objectIsHero(const QString &originalObjectId, const QString &customObjectId) const;
		bool objectIsUnit(const QString &originalObjectId, const QString &customObjectId) const;
		bool objectIsBuilding(const QString &originalObjectId, const QString &customObjectId) const;
		bool objectIsSpecial(const QString &originalObjectId, const QString &customObjectId) const;

		QString objectTilesets(const QString &originalObjectId, const QString &customObjectId) const;
		/**
		 * \return Returns true if a tileset combo box is shown for \p race. For example when selecting a neutral unit it is shown for specific tilesets only.
		 */
		bool showTilesetForRace(const QString &race) const;
		/**
		 * \return Returns true if a level combo box is shown for \p race. This helps you finding units of a specific level (mostly used for creeps).
		 */
		bool showLevelForRace(const QString &race) const;

		MetaData* unitMetaData() const;
		MetaData* unitEditorData() const;
		MetaData* unitData() const;
		MetaData* unitUi() const;
		MetaData* unitBalance() const;
		MetaData* unitWeapons() const;
		MetaData* unitAbilities() const;
		MetaData* humanUnitStrings() const;
		MetaData* humanUnitFunc() const;
		MetaData* orcUnitStrings() const;
		MetaData* orcUnitFunc() const;
		MetaData* undeadUnitStrings() const;
		MetaData* undeadUnitFunc() const;
		MetaData* nightElfUnitStrings() const;
		MetaData* nightElfUnitFunc() const;
		MetaData* neutralUnitStrings() const;
		MetaData* neutralUnitFunc() const;
		MetaData* campaignUnitStrings() const;
		MetaData* campaignUnitFunc() const;
	private:
		MetaDataPtr m_unitMetaData;
		MetaDataPtr m_unitEditorData;
		MetaDataPtr m_unitData;
		MetaDataPtr m_unitUi;
		MetaDataPtr m_unitBalance;
		MetaDataPtr m_unitWeapons;
		MetaDataPtr m_unitAbilities;
		MetaDataPtr m_humanUnitStrings;
		MetaDataPtr m_humanUnitFunc;
		MetaDataPtr m_orcUnitStrings;
		MetaDataPtr m_orcUnitFunc;
		MetaDataPtr m_undeadUnitStrings;
		MetaDataPtr m_undeadUnitFunc;
		MetaDataPtr m_nightElfUnitStrings;
		MetaDataPtr m_nightElfUnitFunc;
		MetaDataPtr m_neutralUnitStrings;
		MetaDataPtr m_neutralUnitFunc;
		MetaDataPtr m_campaignUnitStrings;
		MetaDataPtr m_campaignUnitFunc;
};

inline map::CustomObjects::Type UnitData::type() const
{
	return map::CustomObjects::Type::Units;
}

inline MetaData* UnitData::metaData() const
{
	return this->unitMetaData();
}

inline MetaData* UnitData::objectTabData() const
{
	return this->unitEditorData();
}

inline MetaData* UnitData::unitMetaData() const
{
	return this->m_unitMetaData.data();
}

inline MetaData* UnitData::unitEditorData() const
{
	return this->m_unitEditorData.data();
}

inline MetaData* UnitData::unitData() const
{
	return this->m_unitData.data();
}

inline MetaData* UnitData::unitUi() const
{
	return this->m_unitUi.data();
}

inline MetaData* UnitData::unitBalance() const
{
	return this->m_unitBalance.data();
}

inline MetaData* UnitData::unitWeapons() const
{
	return this->m_unitWeapons.data();
}

inline MetaData* UnitData::unitAbilities() const
{
	return this->m_unitAbilities.data();
}

inline MetaData* UnitData::humanUnitStrings() const
{
	return this->m_humanUnitStrings.data();
}

inline MetaData* UnitData::humanUnitFunc() const
{
	return this->m_humanUnitFunc.data();
}

inline MetaData* UnitData::orcUnitStrings() const
{
	return this->m_orcUnitStrings.data();
}

inline MetaData* UnitData::orcUnitFunc() const
{
	return this->m_orcUnitFunc.data();
}

inline MetaData* UnitData::undeadUnitStrings() const
{
	return this->m_undeadUnitStrings.data();
}

inline MetaData* UnitData::undeadUnitFunc() const
{
	return this->m_undeadUnitFunc.data();
}

inline MetaData* UnitData::nightElfUnitStrings() const
{
	return this->m_nightElfUnitStrings.data();
}

inline MetaData* UnitData::nightElfUnitFunc() const
{
	return this->m_nightElfUnitFunc.data();
}

inline MetaData* UnitData::neutralUnitStrings() const
{
	return this->m_neutralUnitStrings.data();
}

inline MetaData* UnitData::neutralUnitFunc() const
{
	return this->m_neutralUnitFunc.data();
}

inline MetaData* UnitData::campaignUnitStrings() const
{
	return this->m_campaignUnitStrings.data();
}

inline MetaData* UnitData::campaignUnitFunc() const
{
	return this->m_campaignUnitFunc.data();
}

}

}

#endif // WC3LIB_EDITOR_UNITDATA_HPP
