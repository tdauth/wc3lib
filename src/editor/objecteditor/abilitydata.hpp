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

#ifndef WC3LIB_EDITOR_ABILITYDATA_HPP
#define WC3LIB_EDITOR_ABILITYDATA_HPP

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class KDE_EXPORT AbilityData : public ObjectData
{
	public:
		AbilityData(MpqPriorityList *source, QObject *parent = 0);

		virtual void load(QWidget *widget) override;

		virtual StandardObjecIds standardObjectIds() const override;
		virtual MetaDataList resolveDefaultField(const QString& objectId, const QString& fieldId) const override;
		virtual bool hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const override;

		virtual MetaData* metaData() const override;
		virtual MetaData* objectTabData() const override;

		virtual bool hasCustomUnits() const override;
		virtual bool hasCustomObjects() const override;
		virtual bool hasMetaDataList() const override;

		virtual MetaDataList metaDataList() const override;

		virtual map::CustomObjects::Type type() const override;

		virtual QString objectName(const QString &originalObjectId, const QString &customObjectId) const override;

		MetaData* abilityData() const;

		MetaData* itemAbilityFunc() const;
		MetaData* itemAbilityStrings() const;

		MetaData* humanAbilityFunc() const;
		MetaData* humanAbilityStrings() const;

		MetaData* orcAbilityFunc() const;
		MetaData* orcAbilityStrings() const;

		MetaData* undeadAbilityFunc() const;
		MetaData* undeadAbilityStrings() const;

		MetaData* nightElfAbilityFunc() const;
		MetaData* nightElfAbilityStrings() const;

		MetaData* commonAbilityFunc() const;
		MetaData* commonAbilityStrings() const;

		MetaData* neutralAbilityFunc() const;
		MetaData* neutralAbilityStrings() const;

		MetaData* campaignAbilityFunc() const;
		MetaData* campaignAbilityStrings() const;

		bool objectIsHero(const QString &originalObjectId, const QString &customObjectId) const;
		bool objectIsUnit(const QString &originalObjectId, const QString &customObjectId) const;
		bool objectIsItem(const QString &originalObjectId, const QString &customObjectId) const;

	private:
		MetaDataPtr m_metaData;
		MetaDataPtr m_abilityData;
		MetaDataPtr m_itemAbilityFunc;
		MetaDataPtr m_itemAbilityStrings;
		MetaDataPtr m_humanAbilityFunc;
		MetaDataPtr m_humanAbilityStrings;
		MetaDataPtr m_orcAbilityFunc;
		MetaDataPtr m_orcAbilityStrings;
		MetaDataPtr m_undeadAbilityFunc;
		MetaDataPtr m_undeadAbilityStrings;
		MetaDataPtr m_nightElfAbilityFunc;
		MetaDataPtr m_nightElfAbilityStrings;
		MetaDataPtr m_commonAbilityFunc;
		MetaDataPtr m_commonAbilityStrings;
		MetaDataPtr m_neutralAbilityFunc;
		MetaDataPtr m_neutralAbilityStrings;
		// Frozen Throne
		MetaDataPtr m_campaignAbilityFunc;
		MetaDataPtr m_campaignAbilityStrings;
};

inline MetaData* AbilityData::metaData() const
{
	return this->m_metaData.data();
}

inline MetaData* AbilityData::abilityData() const
{
	return this->m_abilityData.data();
}

inline MetaData* AbilityData::itemAbilityFunc() const
{
	return this->m_itemAbilityFunc.data();
}

inline MetaData* AbilityData::itemAbilityStrings() const
{
	return this->m_itemAbilityStrings.data();
}

inline MetaData* AbilityData::humanAbilityFunc() const
{
	return this->m_humanAbilityFunc.data();
}

inline MetaData* AbilityData::humanAbilityStrings() const
{
	return this->m_humanAbilityStrings.data();
}

inline MetaData* AbilityData::orcAbilityFunc() const
{
	return this->m_orcAbilityFunc.data();
}

inline MetaData* AbilityData::orcAbilityStrings() const
{
	return this->m_orcAbilityStrings.data();
}


inline MetaData* AbilityData::undeadAbilityFunc() const
{
	return this->m_undeadAbilityFunc.data();
}

inline MetaData* AbilityData::undeadAbilityStrings() const
{
	return this->m_undeadAbilityStrings.data();
}

inline MetaData* AbilityData::nightElfAbilityFunc() const
{
	return this->m_nightElfAbilityFunc.data();
}

inline MetaData* AbilityData::nightElfAbilityStrings() const
{
	return this->m_nightElfAbilityStrings.data();
}

inline MetaData* AbilityData::commonAbilityFunc() const
{
	return this->m_commonAbilityFunc.data();
}

inline MetaData* AbilityData::commonAbilityStrings() const
{
	return this->m_commonAbilityStrings.data();
}

inline MetaData* AbilityData::neutralAbilityFunc() const
{
	return this->m_neutralAbilityFunc.data();
}

inline MetaData* AbilityData::neutralAbilityStrings() const
{
	return this->m_neutralAbilityStrings.data();
}

inline MetaData* AbilityData::campaignAbilityFunc() const
{
	return this->m_campaignAbilityFunc.data();
}

inline MetaData* AbilityData::campaignAbilityStrings() const
{
	return this->m_campaignAbilityStrings.data();
}

}

}

#endif // WC3LIB_EDITOR_ABILITYDATA_HPP
