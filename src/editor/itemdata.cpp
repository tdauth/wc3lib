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

#include "itemdata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ItemData::ItemData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds ItemData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (this->itemData() != 0 && !this->itemData()->isEmpty())
	{
		for (int row = 1; row < this->itemData()->rows(); ++row)
		{
			result << this->itemData()->value(row, "itemID");
		}
	}

	return result;
}

ObjectData::MetaDataList ItemData::resolveDefaultField(const QString &objectId, const QString &fieldId, int level) const
{
	MetaDataList result;

	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "ItemData")
		{
			result.push_back(this->itemData());
		}
		else if (slk == "Profile")
		{
			result.push_back(this->itemFunc());
			result.push_back(this->itemStrings());
		}
		else
		{
			qDebug() << "Unknown SLK " << slk << " for field " << fieldId << "for object" << objectId;
		}
	}
	else
	{
		qDebug() << "Missing slk value" << fieldId << "for object" << objectId;
	}

	qDebug() << "Missing value" << fieldId << "for object" << objectId;

	return result;
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

QString ItemData::customObjectsExtension() const
{
	return "w3t";
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

bool ItemData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	if (this->metaData()->hasValue(fieldId, "useItem"))
	{
		return this->metaData()->value(fieldId, "useItem") != "1";
	}

	qDebug() << "Missing \"useItem\" entry for field" << fieldId;

	return true;
}

QString ItemData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	QString name = fieldReadableValue(originalObjectId, customObjectId, "unam");

	if (this->hasFieldValue(originalObjectId, customObjectId, "unsf"))
	{
		const QString suffix = this->fieldReadableValue(originalObjectId, customObjectId, "unsf");

		if (suffix.startsWith('('))
		{
			name = QObject::tr("%1 %2").arg(name).arg(suffix);
		}
		else
		{
			name = QObject::tr("%1 (%2)").arg(name).arg(suffix);
		}
	}

	return name;
}

QIcon ItemData::objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const
{
	const QString art = this->fieldValue(originalObjectId, customObjectId, "uico");

	if (!art.isEmpty())
	{
		return this->source()->sharedData()->icon(art, window);
	}

	return this->source()->sharedData()->worldEditDataIcon("InvalidIcon", "WorldEditArt", window);
}

int ItemData::objectLevels(const QString& originalObjectId, const QString& customObjectId) const
{
	return 1;
}

void ItemData::load(QWidget *widget)
{
	// TODO same meta data as for units -> share it!
	this->m_metaData.reset(new MetaData(QUrl("Units/UnitMetaData.slk")));
	this->m_metaData->setSource(this->source());
	this->m_metaData->load();
	this->m_itemData.reset(new MetaData(QUrl("Units/ItemData.slk")));
	this->m_itemData->setSource(this->source());
	this->m_itemData->load();
	this->m_itemFunc.reset(new MetaData(QUrl("Units/ItemFunc.txt")));
	this->m_itemFunc->setSource(this->source());
	this->m_itemFunc->load();
	this->m_itemStrings.reset(new MetaData(QUrl("Units/ItemStrings.txt")));
	this->m_itemStrings->setSource(this->source());
	this->m_itemStrings->load();
}

MetaData* ItemData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

}

}
