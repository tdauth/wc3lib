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

#include <QtGui>

#include "weatherdata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

WeatherData::WeatherData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds WeatherData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "Weather.slk" to standard weather effects in Unit Editor
	if (this->weather() != 0 && !this->weather()->isEmpty())
	{
		for (map::Slk::Table::size_type row = 1; row < this->weather()->rows(); ++row)
		{
			result << this->weather()->value(row, "effectID");
		}
	}

	return result;
}

ObjectData::MetaDataList WeatherData::resolveDefaultField(const QString& objectId, const QString& fieldId) const
{
	MetaDataList result;

	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "Weather")
		{
			result.push_back(this->weather());
		}
	}

	return result;
}

bool WeatherData::hideField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId) const
{
	return false;
}

bool WeatherData::hasCustomUnits() const
{
	return false;
}

bool WeatherData::hasCustomObjects() const
{
	return false;
}

bool WeatherData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList WeatherData::metaDataList() const
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

void WeatherData::load(QWidget* widget)
{
	this->m_weatherMetaData.reset(new MetaData(KUrl("TerrainArt/WeatherMetaData.slk")));
	this->m_weatherMetaData->setSource(this->source());
	this->m_weatherMetaData->load();
	this->m_weather.reset(new MetaData(KUrl("TerrainArt/Weather.slk")));
	this->m_weather->setSource(this->source());
	this->m_weather->load();
}

MetaData* WeatherData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

QString WeatherData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	return fieldReadableValue(originalObjectId, customObjectId, "unam");
}

QIcon WeatherData::objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const
{
	const QString art = "Textures\\" + this->fieldValue(originalObjectId, customObjectId, "texf") + ".blp";

	return this->source()->sharedData()->icon(art, window);
}

QString WeatherData::nextCustomObjectId() const
{
	QString result = ObjectData::nextCustomObjectId();
	result[0] = 'W';

	return result;
}

}

}
