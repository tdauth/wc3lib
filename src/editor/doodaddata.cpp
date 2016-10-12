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

#include "doodaddata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

DoodadData::DoodadData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds DoodadData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (this->doodadData() != 0 && !this->doodadData()->isEmpty())
	{
		for (int row = 1; row < this->doodadData()->rows(); ++row)
		{
			result << this->doodadData()->value(row, "doodID");
		}
	}

	return result;
}

ObjectData::MetaDataList DoodadData::resolveDefaultField(const QString &objectId, const QString &fieldId, int level) const
{
	MetaDataList result;

	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "DoodadData")
		{
			result.push_back(this->doodadData());
		}
	}

	if (result.empty())
	{
		qDebug() << "Missing value" << fieldId << "for object" << objectId;
	}

	return result;
}

map::CustomObjects::Type DoodadData::type() const
{
	return map::CustomObjects::Type::Doodads;
}

bool DoodadData::hasCustomUnits() const
{
	return false;
}

bool DoodadData::hasCustomObjects() const
{
	return true;
}

QString DoodadData::customObjectsExtension() const
{
	return "w3d";
}

bool DoodadData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList DoodadData::metaDataList() const
{
	// TODO support slks
	return ObjectData::MetaDataList();
}

bool DoodadData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	return false;
}

QString DoodadData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	return fieldReadableValue(originalObjectId, customObjectId, "dnam");
}

QIcon DoodadData::objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const
{
	return this->source()->sharedData()->worldEditDataIcon("InvalidIcon", "WorldEditArt", window);
}

int DoodadData::objectLevels(const QString& originalObjectId, const QString& customObjectId) const
{
	return 1;
}

void DoodadData::load(QWidget *widget)
{
	// TODO same meta data as for units -> share it!
	this->m_metaData.reset(new MetaData(QUrl("Doodads/DoodadMetaData.slk")));
	this->m_metaData->setSource(this->source());
	this->m_metaData->load();
	this->m_doodadData.reset(new MetaData(QUrl("Doodads/Doodads.slk")));
	this->m_doodadData->setSource(this->source());
	this->m_doodadData->load();
}

MetaData* DoodadData::objectTabData() const
{
	// contains the section [DoodadCategories]
	return this->source()->sharedData()->worldEditData().get();
}

}

}
