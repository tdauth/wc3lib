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

#ifndef WC3LIB_EDITOR_UPGRADEDATA_HPP
#define WC3LIB_EDITOR_UPGRADEDATA_HPP

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class KDE_EXPORT UpgradeData : public ObjectData
{
	public:
		UpgradeData(MpqPriorityList *source, QObject *parent = 0);

		virtual StandardObjecIds standardObjectIds() const override;
		virtual MetaDataList resolveDefaultField(const QString& objectId, const QString& fieldId, int level = 0) const override;
		virtual bool hideField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, int level = 0) const override;


		virtual bool hasCustomUnits() const override;
		virtual bool hasCustomObjects() const override;
		virtual bool hasMetaDataList() const override;

		virtual MetaDataList metaDataList() const override;

		virtual QString objectName(const QString &originalObjectId, const QString &customObjectId) const override;
		virtual QIcon objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const override;
		virtual int objectLevels(const QString &originalObjectId, const QString &customObjectId) const override;

		virtual MetaData* objectTabData() const override;
		virtual map::CustomObjects::Type type() const override;


		virtual void load(QWidget* widget) override;

		virtual MetaData* metaData() const override;

		MetaData* upgradeData() const;
		MetaData* humanUpgradeStrings() const;
		MetaData* humanUpgradeFunc() const;
		MetaData* orcUpgradeStrings() const;
		MetaData* orcUpgradeFunc() const;
		MetaData* undeadUpgradeStrings() const;
		MetaData* undeadUpgradeFunc() const;
		MetaData* nightElfUpgradeStrings() const;
		MetaData* nightElfUpgradeFunc() const;
		MetaData* neutralUpgradeStrings() const;
		MetaData* neutralUpgradeFunc() const;
		MetaData* campaignUpgradeStrings() const;
		MetaData* campaignUpgradeFunc() const;

	private:
		MetaDataPtr m_upgradeMetaData;
		MetaDataPtr m_upgradeData;

		MetaDataPtr m_humanUpgradeStrings;
		MetaDataPtr m_humanUpgradeFunc;
		MetaDataPtr m_orcUpgradeStrings;
		MetaDataPtr m_orcUpgradeFunc;
		MetaDataPtr m_undeadUpgradeStrings;
		MetaDataPtr m_undeadUpgradeFunc;
		MetaDataPtr m_nightElfUpgradeStrings;
		MetaDataPtr m_nightElfUpgradeFunc;
		MetaDataPtr m_neutralUpgradeStrings;
		MetaDataPtr m_neutralUpgradeFunc;
		MetaDataPtr m_campaignUpgradeStrings;
		MetaDataPtr m_campaignUpgradeFunc;
};

inline MetaData* UpgradeData::metaData() const
{
	return this->m_upgradeMetaData.data();
}

inline MetaData* UpgradeData::upgradeData() const
{
	return this->m_upgradeData.data();
}

inline map::CustomObjects::Type UpgradeData::type() const
{
	return map::CustomObjects::Type::Upgrades;
}

inline MetaData* UpgradeData::humanUpgradeStrings() const
{
	return this->m_humanUpgradeStrings.data();
}

inline MetaData* UpgradeData::humanUpgradeFunc() const
{
	return this->m_humanUpgradeFunc.data();
}

inline MetaData* UpgradeData::orcUpgradeStrings() const
{
	return this->m_orcUpgradeStrings.data();
}

inline MetaData* UpgradeData::orcUpgradeFunc() const
{
	return this->m_orcUpgradeFunc.data();
}

inline MetaData* UpgradeData::undeadUpgradeStrings() const
{
	return this->m_undeadUpgradeStrings.data();
}

inline MetaData* UpgradeData::undeadUpgradeFunc() const
{
	return this->m_undeadUpgradeFunc.data();
}

inline MetaData* UpgradeData::nightElfUpgradeStrings() const
{
	return this->m_nightElfUpgradeStrings.data();
}

inline MetaData* UpgradeData::nightElfUpgradeFunc() const
{
	return this->m_nightElfUpgradeFunc.data();
}

inline MetaData* UpgradeData::neutralUpgradeStrings() const
{
	return this->m_neutralUpgradeStrings.data();
}

inline MetaData* UpgradeData::neutralUpgradeFunc() const
{
	return this->m_neutralUpgradeFunc.data();
}

inline MetaData* UpgradeData::campaignUpgradeStrings() const
{
	return this->m_campaignUpgradeStrings.data();
}

inline MetaData* UpgradeData::campaignUpgradeFunc() const
{
	return this->m_campaignUpgradeFunc.data();
}

}

}

#endif // WC3LIB_EDITOR_UPGRADEDATA_HPP
