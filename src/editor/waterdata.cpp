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

#include <QtGui>

#include "waterdata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

WaterData::WaterData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds WaterData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "Weather.slk" to standard weather effects in Unit Editor
	if (this->water() != 0 && !this->water()->isEmpty())
	{
		for (int row = 1; row < this->water()->rows(); ++row)
		{
			result << this->water()->value(row, "ID");
		}
	}

	return result;
}

ObjectData::MetaDataList WaterData::resolveDefaultField(const QString& objectId, const QString& fieldId, int level) const
{
	MetaDataList result;

	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "Water")
		{
			result.push_back(this->water());
		}
	}

	return result;
}

bool WaterData::hideField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, int level) const
{
	return false;
}

bool WaterData::hasCustomUnits() const
{
	return false;
}

bool WaterData::hasCustomObjects() const
{
	return false;
}

QString WaterData::customObjectsExtension() const
{
	return QString();
}

bool WaterData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList WaterData::metaDataList() const
{
	/*
	map::Slk slk;
	slk.table().resize(boost::extents[this->metaData()->rows()][this->m_objects.size() + this->weather()->rows()]);
	map::Slk::Table::size_type column = 0;

	for (map::Slk::Table::size_type row = 1; row < this->metaData()->rows() && column < slk.columns(); ++row, ++column)
	{
		const QString fieldId = this->metaData()->value(row, "ID");
		slk.table()[column][0] = this->metaData()->value(fieldId, "field").toUtf8().constData();
	}

	Slks result;
	result.append(slk);

	return result;
	TODO create files
	*/

	return MetaDataList();
}

void WaterData::load(QWidget *widget)
{
	this->m_waterMetaData.reset(new MetaData(QUrl("TerrainArt/WaterMetaData.slk")));
	this->m_waterMetaData->setSource(this->source());
	this->m_waterMetaData->load();
	this->m_water.reset(new MetaData(QUrl("TerrainArt/Water.slk")));
	this->m_water->setSource(this->source());
	this->m_water->load();
}

MetaData* WaterData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

QString WaterData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	QString result = fieldReadableValue(originalObjectId, customObjectId, "texf");

	return this->source()->sharedData()->tr(result, "WorldEditStrings", result);
}

QIcon WaterData::objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const
{
	const QString art = this->fieldValue(originalObjectId, customObjectId, "texf") + ".blp";

	return this->source()->sharedData()->icon(art, window);
}

int WaterData::objectLevels(const QString& originalObjectId, const QString& customObjectId) const
{
	return 1;
}

QString WaterData::nextCustomObjectId(const QString &originalObjectId) const
{
	QString result = ObjectData::nextCustomObjectId(originalObjectId);
	result[0] = 'M';

	return result;
}

}

}
