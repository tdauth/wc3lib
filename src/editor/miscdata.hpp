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

#ifndef WC3LIB_EDITOR_MISCDATA_HPP
#define WC3LIB_EDITOR_MISCDATA_HPP

#include <QSet>

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class MetaData;

/**
 * \brief Stores all data for custom and standard misc data.
 *
 * \ingroup objectdata
 */
class KDE_EXPORT MiscData : public ObjectData
{
	public:
		MiscData(MpqPriorityList *source, QObject *parent = 0);

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

		MetaData* miscMetaData() const;
		MetaData* miscGame() const;
		MetaData* customV0() const;
		MetaData* customV1() const;
		MetaData* meleeV0() const;

	private:
		/**
		 * \brief All misc data sources from Warcraft III.
		 */
		typedef QMap<QString,MetaData*> MiscFiles;
		MiscFiles m_miscFiles;

		MetaDataPtr m_miscMetaData;
		MetaDataPtr m_miscGame;
		MetaDataPtr m_customV0;
		MetaDataPtr m_customV1;
		MetaDataPtr m_meleeV0;

};

inline map::CustomObjects::Type MiscData::type() const
{
	return map::CustomObjects::Type::Units;
}

inline MetaData* MiscData::metaData() const
{
	return this->miscMetaData();
}

inline MetaData* MiscData::miscMetaData() const
{
	return this->m_miscMetaData.data();
}

inline MetaData* MiscData::miscGame() const
{
	return this->m_miscGame.data();
}

inline MetaData* MiscData::customV0() const
{
	return this->m_customV0.data();
}

inline MetaData* MiscData::customV1() const
{
	return this->m_customV1.data();
}

inline MetaData* MiscData::meleeV0() const
{
	return this->m_meleeV0.data();
}

}

}

#endif // WC3LIB_EDITOR_MISCDATA_HPP
