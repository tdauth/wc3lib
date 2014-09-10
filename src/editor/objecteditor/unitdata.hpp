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
 */
class UnitData : public ObjectData
{
	public:
		UnitData(MpqPriorityList *source);

		virtual void load(QWidget *widget) override;

		virtual bool hasDefaultFieldValue(const QString &objectId, const QString &fieldId) const override;
		virtual QString defaultFieldValue(const QString &objectId, const QString &fieldId) const override;

		virtual MetaData* metaData() const override;
		virtual MetaData* objectTabData() const override;

		virtual bool hasCustomUnits() const override;
		virtual bool hasCustomObjects() const override;

		bool objectIsHero(const QString &originalObjectId, const QString &customObjectId) const;
		bool objectIsUnit(const QString &originalObjectId, const QString &customObjectId) const;
		bool objectIsBuilding(const QString &originalObjectId, const QString &customObjectId) const;

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
		MetaData *m_unitMetaData;
		MetaData *m_unitEditorData;
		MetaData *m_unitData;
		MetaData *m_unitUi;
		MetaData *m_unitBalance;
		MetaData *m_unitWeapons;
		MetaData *m_unitAbilities;
		MetaData *m_humanUnitStrings;
		MetaData *m_humanUnitFunc;
		MetaData *m_orcUnitStrings;
		MetaData *m_orcUnitFunc;
		MetaData *m_undeadUnitStrings;
		MetaData *m_undeadUnitFunc;
		MetaData *m_nightElfUnitStrings;
		MetaData *m_nightElfUnitFunc;
		MetaData *m_neutralUnitStrings;
		MetaData *m_neutralUnitFunc;
		MetaData *m_campaignUnitStrings;
		MetaData *m_campaignUnitFunc;
};

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
	return this->m_unitMetaData;
}

inline MetaData* UnitData::unitEditorData() const
{
	return this->m_unitEditorData;
}

inline MetaData* UnitData::unitData() const
{
	return this->m_unitData;
}

inline MetaData* UnitData::unitUi() const
{
	return this->m_unitUi;
}

inline MetaData* UnitData::unitBalance() const
{
	return this->m_unitBalance;
}

inline MetaData* UnitData::unitWeapons() const
{
	return this->m_unitWeapons;
}

inline MetaData* UnitData::unitAbilities() const
{
	return this->m_unitAbilities;
}

inline MetaData* UnitData::humanUnitStrings() const
{
	return this->m_humanUnitStrings;
}

inline MetaData* UnitData::humanUnitFunc() const
{
	return this->m_humanUnitFunc;
}

inline MetaData* UnitData::orcUnitStrings() const
{
	return this->m_orcUnitStrings;
}

inline MetaData* UnitData::orcUnitFunc() const
{
	return this->m_orcUnitFunc;
}

inline MetaData* UnitData::undeadUnitStrings() const
{
	return this->m_undeadUnitStrings;
}

inline MetaData* UnitData::undeadUnitFunc() const
{
	return this->m_undeadUnitFunc;
}

inline MetaData* UnitData::nightElfUnitStrings() const
{
	return this->m_nightElfUnitStrings;
}

inline MetaData* UnitData::nightElfUnitFunc() const
{
	return this->m_nightElfUnitFunc;
}

inline MetaData* UnitData::neutralUnitStrings() const
{
	return this->m_neutralUnitStrings;
}

inline MetaData* UnitData::neutralUnitFunc() const
{
	return this->m_neutralUnitFunc;
}

inline MetaData* UnitData::campaignUnitStrings() const
{
	return this->m_campaignUnitStrings;
}

inline MetaData* UnitData::campaignUnitFunc() const
{
	return this->m_campaignUnitFunc;
}

}

}

#endif // WC3LIB_EDITOR_UNITDATA_HPP
