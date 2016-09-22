/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_BUFFDATA_HPP
#define WC3LIB_EDITOR_BUFFDATA_HPP

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class BuffData : public ObjectData
{
	public:
		BuffData(MpqPriorityList *source, QObject *parent = 0);

		virtual void load(QWidget *widget) override;

		virtual StandardObjecIds standardObjectIds() const override;
		virtual MetaDataList resolveDefaultField(const QString& objectId, const QString& fieldId, int level = 0) const override;
		virtual bool hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const override;

		virtual MetaData* metaData() const override;
		virtual MetaData* objectTabData() const override;

		virtual bool hasCustomUnits() const override;
		virtual bool hasCustomObjects() const override;
		virtual QString customObjectsExtension() const override;
		virtual bool hasMetaDataList() const override;

		virtual MetaDataList metaDataList() const override;

		virtual map::CustomObjects::Type type() const override;

		virtual QString objectName(const QString &originalObjectId, const QString &customObjectId) const override;
		virtual QIcon objectIcon(const QString &originalObjectId, const QString &customObjectId, QWidget* window) const override;
		virtual int objectLevels(const QString &originalObjectId, const QString &customObjectId) const override;

		MetaData* buffData() const;

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
		MetaDataPtr m_buffData;
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

inline MetaData* BuffData::metaData() const
{
	return this->m_metaData.data();
}

inline MetaData* BuffData::buffData() const
{
	return this->m_buffData.data();
}

inline MetaData* BuffData::itemAbilityFunc() const
{
	return this->m_itemAbilityFunc.data();
}

inline MetaData* BuffData::itemAbilityStrings() const
{
	return this->m_itemAbilityStrings.data();
}

inline MetaData* BuffData::humanAbilityFunc() const
{
	return this->m_humanAbilityFunc.data();
}

inline MetaData* BuffData::humanAbilityStrings() const
{
	return this->m_humanAbilityStrings.data();
}

inline MetaData* BuffData::orcAbilityFunc() const
{
	return this->m_orcAbilityFunc.data();
}

inline MetaData* BuffData::orcAbilityStrings() const
{
	return this->m_orcAbilityStrings.data();
}


inline MetaData* BuffData::undeadAbilityFunc() const
{
	return this->m_undeadAbilityFunc.data();
}

inline MetaData* BuffData::undeadAbilityStrings() const
{
	return this->m_undeadAbilityStrings.data();
}

inline MetaData* BuffData::nightElfAbilityFunc() const
{
	return this->m_nightElfAbilityFunc.data();
}

inline MetaData* BuffData::nightElfAbilityStrings() const
{
	return this->m_nightElfAbilityStrings.data();
}

inline MetaData* BuffData::commonAbilityFunc() const
{
	return this->m_commonAbilityFunc.data();
}

inline MetaData* BuffData::commonAbilityStrings() const
{
	return this->m_commonAbilityStrings.data();
}

inline MetaData* BuffData::neutralAbilityFunc() const
{
	return this->m_neutralAbilityFunc.data();
}

inline MetaData* BuffData::neutralAbilityStrings() const
{
	return this->m_neutralAbilityStrings.data();
}

inline MetaData* BuffData::campaignAbilityFunc() const
{
	return this->m_campaignAbilityFunc.data();
}

inline MetaData* BuffData::campaignAbilityStrings() const
{
	return this->m_campaignAbilityStrings.data();
}

}

}

#endif // WC3LIB_EDITOR_BUFFDATA_HPP
