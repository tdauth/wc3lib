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

#include <KMessageBox>

#include "weatherdata.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

WeatherData::WeatherData(MpqPriorityList* source) : ObjectData(source)
{
}

QString WeatherData::defaultFieldValue(const QString& objectId, const QString& fieldId) const
{
	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->metaData()->hasValue(fieldId, "field"))
	{
		const QString field = this->metaData()->value(fieldId, "field");
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "Weather")
		{
			return this->weather()->value(objectId, field);
		}
	}

	return QString();
}


bool WeatherData::hasDefaultFieldValue(const QString& objectId, const QString& fieldId) const
{
	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->metaData()->hasValue(fieldId, "field"))
	{
		const QString field = this->metaData()->value(fieldId, "field");
		const QString slk = this->metaData()->value(fieldId, "slk");

		// TODO improve performance by not calling the hasValue() methods?
		if (slk == "Weather")
		{
			return this->weather()->hasValue(objectId, field);
		}
	}

	return false;
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

bool WeatherData::hasSlks() const
{
	return true;
}

ObjectData::Slks WeatherData::slks() const
{
	map::Slk slk;
	slk.table().resize(boost::extents[this->metaData()->slk().rows()][this->m_objects.size() + this->weather()->slk().rows()]);
	map::Slk::Table::size_type column = 0;

	for (map::Slk::Table::size_type row = 1; row < this->metaData()->slk().rows() && column < slk.columns(); ++row, ++column)
	{
		const QString fieldId = this->metaData()->value(row, "ID");
		slk.table()[column][0] = this->metaData()->value(fieldId, "field").toUtf8().constData();
	}

	Slks result;
	result.append(slk);

	return result;
}

void WeatherData::load(QWidget* widget)
{
	this->m_weatherMetaData.reset(new MetaData(KUrl("TerrainArt/WeatherMetaData.slk")));
	this->m_weatherMetaData->setSource(this->source());

	try
	{
		this->m_weatherMetaData->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_weatherMetaData->url().toEncoded().constData(), e.what()));
	}

	this->m_weather.reset(new MetaData(KUrl("TerrainArt/Weather.slk")));
	this->m_weather->setSource(this->source());

	try
	{
		this->m_weather->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_weather->url().toEncoded().constData(), e.what()));
	}
}

QString WeatherData::objectName(const QString& originalObjectId, const QString& customObjectId) const
{
	return fieldValue(originalObjectId, customObjectId, "unam");
}

MetaData* WeatherData::objectTabData() const
{
	return 0;
}

}

}
