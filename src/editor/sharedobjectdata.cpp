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

#include "sharedobjectdata.hpp"
#include "unitdata.hpp"
#include "itemdata.hpp"
#include "abilitydata.hpp"
#include "weatherdata.hpp"
#include "miscdata.hpp"

namespace wc3lib
{

namespace editor
{

SharedObjectData::SharedObjectData(MpqPriorityList *source) : m_source(source)
, m_unitData(new UnitData(source))
, m_itemData(new ItemData(source))
, m_abilityData(new AbilityData(source))
, m_weatherData(new WeatherData(source))
, m_miscData(new MiscData(source))
{
}

void SharedObjectData::load(QWidget *widget)
{
	m_unitData->load(widget);
	m_itemData->load(widget);
	m_abilityData->load(widget);
	m_weatherData->load(widget);
	m_miscData->load(widget);
}

ObjectData* SharedObjectData::resolveByFieldType(const QString &fieldType) const
{
	if (fieldType == "unitList")
	{
		return unitData().get();
	}
	else if (fieldType == "abilityList" || fieldType == "heroAbilityList")
	{
		return abilityData().get();
	}

	return 0;
}

}

}
