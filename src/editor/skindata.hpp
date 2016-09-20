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

#ifndef WC3LIB_EDITOR_SKINDATA_HPP
#define WC3LIB_EDITOR_SKINDATA_HPP

#include <QMap>

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class MetaData;

/**
 * \brief Stores all data for custom and standard skin data.
 *
 * \ingroup objectdata
 */
class SkinData : public ObjectData
{
	public:
		SkinData(MpqPriorityList *source, QObject *parent = 0);

		virtual void load(QWidget *widget) override;

		virtual StandardObjecIds standardObjectIds() const override;
		virtual MetaDataList resolveDefaultField(const QString &objectId, const QString &fieldId, int level = 0) const override;
		virtual bool hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const override;

		virtual MetaData* metaData() const override;
		virtual MetaData* objectTabData() const override;

		virtual bool hasCustomUnits() const override;
		virtual bool hasCustomObjects() const override;
		virtual bool hasMetaDataList() const override;

		virtual MetaDataList metaDataList() const override;

		virtual map::CustomObjects::Type type() const override;
		virtual QString nextCustomObjectId(const QString &originalObjectId) const override;

		virtual QString objectName(const QString &originalObjectId, const QString &customObjectId) const override;
		virtual QIcon objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const override;
		virtual int objectLevels(const QString &originalObjectId, const QString &customObjectId) const override;

		MetaData* war3Skins() const;

	private:
		/**
		 * \brief All misc data sources from Warcraft III.
		 */
		typedef QMap<QString, MetaData*> StandardFiles;
		StandardFiles m_standardFiles;

		MetaDataPtr m_metaData;
		MetaDataPtr m_war3skins;

};

inline map::CustomObjects::Type SkinData::type() const
{
	return map::CustomObjects::Type::Units;
}

inline MetaData* SkinData::metaData() const
{
	return this->m_metaData.data();
}

inline MetaData* SkinData::war3Skins() const
{
	return this->m_war3skins.data();
}

}

}

#endif // WC3LIB_EDITOR_SKINDATA_HPP
