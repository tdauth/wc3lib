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

#include <QtCore>

#include <KMessageBox>

#include "itemdata.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

ItemData::ItemData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

QString ItemData::defaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	if (this->metaData()->hasValue(fieldId, "slk") && this->metaData()->hasValue(fieldId, "field"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");
		const QString field = this->metaData()->value(fieldId, "field");

		if (slk == "ItemData")
		{
			if (this->itemData()->hasValue(objectId, field))
			{
				return this->itemData()->value(objectId, field);
			}
		}
		else if (slk == "Profile")
		{
			if (this->itemFunc()->hasValue(objectId, field))
			{
				return this->itemFunc()->value(objectId, field);
			}
			else if (this->itemStrings()->hasValue(objectId, field))
			{
				return this->itemStrings()->value(objectId, field);
			}
		}
	}

	qDebug() << "Missing value" << fieldId << "for object" << objectId;

	return QString();
}

bool ItemData::hasDefaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	if (this->metaData()->hasValue(fieldId, "slk") && this->metaData()->hasValue(fieldId, "field"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");
		const QString field = this->metaData()->value(fieldId, "field");

		if (slk == "ItemData")
		{
			return this->itemData()->hasValue(objectId, field);
		}
		else if (slk == "Profile")
		{
			if (this->itemFunc()->hasValue(objectId, field))
			{
				return true;
			}
			else if (this->itemStrings()->hasValue(objectId, field))
			{
				return true;
			}
		}
	}

	qDebug() << "Missing value" << fieldId << "for object" << objectId;

	return false;
}

map::CustomObjects::Type ItemData::type() const
{
	return map::CustomObjects::Type::Items;
}

bool ItemData::hasCustomUnits() const
{
	return true;
}

bool ItemData::hasCustomObjects() const
{
	return true;
}

bool ItemData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList ItemData::metaDataList() const
{
	// TODO support slks
	return ObjectData::MetaDataList();
}

bool ItemData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const
{
	if (this->metaData()->hasValue(fieldId, "useItem"))
	{
		return this->metaData()->value(fieldId, "useItem") != "1";
	}

	qDebug() << "Missing \"useItem\" entry for field" << fieldId;

	return true;
}

QString ItemData::objectNameFieldId() const
{
	return "unam";
}

MetaData* ItemData::objectTabData() const
{
	// TODO needs "UI/UnitEditorData.txt"? share with unit data.
	return 0;
}

void ItemData::load(QWidget *widget)
{
	// TODO same meta data as for units -> share it!
	this->m_metaData.reset(new MetaData(KUrl("Units/UnitMetaData.slk")));
	this->m_metaData->setSource(this->source());
	this->m_metaData->load();
	this->m_itemData.reset(new MetaData(KUrl("Units/ItemData.slk")));
	this->m_itemData->setSource(this->source());
	this->m_itemData->load();
	this->m_itemFunc.reset(new MetaData(KUrl("Units/ItemFunc.txt")));
	this->m_itemFunc->setSource(this->source());
	this->m_itemFunc->load();
	this->m_itemStrings.reset(new MetaData(KUrl("Units/ItemStrings.txt")));
	this->m_itemStrings->setSource(this->source());
	this->m_itemStrings->load();
}

}

}
