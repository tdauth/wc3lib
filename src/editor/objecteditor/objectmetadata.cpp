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

#include "objectmetadata.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

bool ObjectMetaData::isReignOfChaos() const
{
	// TODO check more types and check it on loading one single time???
	/*
	 6=unitList
	7=itemList
	8=regenType
	9=attackType
	10=weaponType
	11=targetType
	12=moveType
	13=defenseType
	14=pathingTexture
	15=upgradeList
	16=stringList
	17=abilityList
	18=heroAbilityList
	19=missileArt
	20=attributeType
	21=attackBits
	*/
	auto column = this->metaData()->column(MetaData::toSlkString("type"));

	for (map::Slk::Table::size_type i = 0; i < column.size(); ++i)
	{
		if (column[i] == MetaData::toSlkString("unitList").toUtf8().constData())
		{
			return true;
		}
	}

	return false;
}

map::Value::Type ObjectMetaData::fieldType(const QString &fieldId) const
{
	qDebug() << "Getting type of field" << fieldId;

	const QString type = MetaData::fromSlkString(this->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("type")));

	if (type == "string")
	{
		return map::Value::Type::String;
	}
	else if (type == "int" || type == "uint")
	{
		return map::Value::Type::Integer;
	}

	throw Exception(boost::format(_("Unsupported type %1%.")) % type.toUtf8().constData());
}

}

}
