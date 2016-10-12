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

#include <QtCore>

#include "destructabledata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

DestructableData::DestructableData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds DestructableData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (this->destructableData() != 0 && !this->destructableData()->isEmpty())
	{
		for (int row = 1; row < this->destructableData()->rows(); ++row)
		{
			result << this->destructableData()->value(row, "DestructableID");
		}
	}

	return result;
}

ObjectData::MetaDataList DestructableData::resolveDefaultField(const QString& objectId, const QString& fieldId, int level) const
{
	MetaDataList result;

	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "DestructableData")
		{
			result.push_back(this->destructableData());
		}
	}

	if (result.empty())
	{
		qDebug() << "Missing value" << fieldId << "for object" << objectId;
	}

	return result;
}

map::CustomObjects::Type DestructableData::type() const
{
	return map::CustomObjects::Type::Destructibles;
}

bool DestructableData::hasCustomUnits() const
{
	return false;
}

bool DestructableData::hasCustomObjects() const
{
	return true;
}

QString DestructableData::customObjectsExtension() const
{
	return "w3b";
}

bool DestructableData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList DestructableData::metaDataList() const
{
	// TODO support slks
	return ObjectData::MetaDataList();
}

bool DestructableData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	return false;
}

QString DestructableData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	// destructable strings are WESTRING entries
	QString name = fieldReadableValue(originalObjectId, customObjectId, "bnam");

	if (this->hasFieldValue(originalObjectId, customObjectId, "bsuf"))
	{
		const QString readableSuffix = this->fieldReadableValue(originalObjectId, customObjectId, "bsuf");

		if (!readableSuffix.isEmpty() && readableSuffix != "_")
		{
			const QString suffix = readableSuffix;

			if (suffix.startsWith('('))
			{
				name = QObject::tr("%1 %2").arg(name).arg(suffix);
			}
			else
			{
				name = QObject::tr("%1 (%2)").arg(name).arg(suffix);
			}
		}
	}

	return name;
}

QIcon DestructableData::objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const
{
	return this->source()->sharedData()->worldEditDataIcon("InvalidIcon", "WorldEditArt", window);
}

int DestructableData::objectLevels(const QString& originalObjectId, const QString& customObjectId) const
{
	return 1;
}

void DestructableData::load(QWidget *widget)
{
	// TODO same meta data as for units -> share it!
	this->m_metaData.reset(new MetaData(QUrl("Units/DestructableMetaData.slk")));
	this->m_metaData->setSource(this->source());
	this->m_metaData->load();
	this->m_destructableData.reset(new MetaData(QUrl("Units/DestructableData.slk")));
	this->m_destructableData->setSource(this->source());
	this->m_destructableData->load();
}

MetaData* DestructableData::objectTabData() const
{
	// contains the section [DestructibleCategories]
	return this->source()->sharedData()->worldEditData().get();
}

}

}
