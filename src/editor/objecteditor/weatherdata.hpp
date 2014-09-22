/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef WC3LIB_EDITOR_WEATHERDATA_HPP
#define WC3LIB_EDITOR_WEATHERDATA_HPP

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class WeatherData : public ObjectData
{
	public:
		typedef QScopedPointer<MetaData> MetaDataPtr;

		WeatherData(MpqPriorityList *source);

		virtual QString defaultFieldValue(const QString& objectId, const QString& fieldId) const override;
		virtual bool hasDefaultFieldValue(const QString& objectId, const QString& fieldId) const override;
		virtual bool hideField(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId) const override;


		virtual bool hasCustomUnits() const override;
		virtual bool hasCustomObjects() const override;

		virtual QString objectName(const QString& originalObjectId, const QString& customObjectId) const;
		virtual MetaData* objectTabData() const;
		virtual map::CustomObjects::Type type() const;


		virtual void load(QWidget* widget) override;

		virtual MetaData* metaData() const override;

		MetaData* weather() const;

	private:
		MetaDataPtr m_weatherMetaData;
		MetaDataPtr m_weather;
};

inline MetaData* WeatherData::metaData() const
{
	return this->m_weatherMetaData.data();
}

inline MetaData* WeatherData::weather() const
{
	return this->m_weather.data();
}

inline map::CustomObjects::Type WeatherData::type() const
{
	return map::CustomObjects::Type::Abilities;
}

}

}

#endif // WC3LIB_EDITOR_WEATHERDATA_HPP
