/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_PLATFORM_HPP
#define WC3LIB_EDITOR_PLATFORM_HPP

#include <QSettings>
#include <QColor>

#include <KUrl>

#include "../core.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

namespace
{

inline QList<QVariant> stringList(const map::List &list)
{
	QList<QVariant> result;
	
	foreach (const std::string &value, list)
		result << QVariant(value.c_str());
	
	return result;
}

}

inline QVariant valueToVariant(const map::Value &value)
{
	switch (value.type())
	{
		case map::Value::Type::Integer:
			return QVariant(value.toInteger());
			
		case map::Value::Type::Real:
			return QVariant(value.toReal());
			
		case map::Value::Type::Unreal:
			return QVariant(value.toUnreal());
			
		case map::Value::Type::String:
			return QVariant(value.toString().c_str());
			
		case map::Value::Type::Boolean:
			return QVariant(value.toBoolean());
			
		case map::Value::Type::Character:
			return QVariant(value.toCharacter());
			
		case map::Value::Type::UnitList:
			return QVariant(stringList(value.toUnitList()));
			
		case map::Value::Type::ItemList:
			return QVariant(stringList(value.toItemList()));
			
		case map::Value::Type::RegenerationType:
			return QVariant(value.toRegenerationType().c_str());
			
		case map::Value::Type::AttackType:
			return QVariant(value.toAttackType().c_str());
			
		case map::Value::Type::WeaponType:
			return QVariant(value.toWeaponType().c_str());
			
		case map::Value::Type::TargetType:
			return QVariant(value.toTargetType().c_str());
			
		case map::Value::Type::MoveType:
			return QVariant(value.toMoveType().c_str());
			
		case map::Value::Type::DefenseType:
			return QVariant(value.toDefenseType().c_str());
			
		case map::Value::Type::PathingTexture:
			return QVariant(value.toPathingTexture().c_str());
			
		case map::Value::Type::UpgradeList:
			return QVariant(stringList(value.toUpgradeList()));
			
		case map::Value::Type::StringList:
			return QVariant(stringList(value.toStringList()));
			
		case map::Value::Type::AbilityList:
			return QVariant(stringList(value.toAbilityList()));
			
		case map::Value::Type::HeroAbilityList:
			return QVariant(stringList(value.toHeroAbilityList()));
			
		case map::Value::Type::MissileArt:
			return QVariant(value.toMissileArt().c_str());
			
		case map::Value::Type::AttributeType:
			return QVariant(value.toAttributeType().c_str());
			
		case map::Value::Type::AttackBits:
			return QVariant(value.toAttackBits().c_str());
	}
	
	return QVariant();
}

BOOST_SCOPED_ENUM_START(TeamColor)
{
	Red,
	Blue,
	Teal,
	Purple,
	Yellow,
	Orange,
	Green,
	Pink,
	Gray,
	LightBlue,
	DarkGreen,
	Brown,
	Black,
	MaxTeamColors
};
BOOST_SCOPED_ENUM_END

/**
 * Converts team color enumeration value into Qt RGB color.
 * \param teamColor Team color which is converted.
 * \return Returns corresponding Qt RGB color.
 * \sa teamColor(const QColor&)
 */
inline QColor teamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor)
{
	switch (teamColor)
	{
		case TeamColor::Red:
			return QColor(Qt::red);
		
		case TeamColor::Blue:
			return QColor(Qt::blue);
			
		case TeamColor::Teal:
			return QColor(0x1CB619);
			
		case TeamColor::Purple:
			return QColor(0x800080);
			
		case TeamColor::Yellow:
			return QColor(Qt::yellow);
			
		case TeamColor::Orange:
			return QColor(0xFF8000);
			
		case TeamColor::Green:
			return QColor(Qt::green);
			
		case TeamColor::Pink:
			return QColor(0xFF80C0);
			
		case TeamColor::Gray:
			return QColor(0xC0C0C0);
			
		case TeamColor::LightBlue:
			return QColor(0x0080FF);
			
		case TeamColor::DarkGreen:
			return QColor(0x106246);
			
		case TeamColor::Brown:
			return QColor(0x804000);
			
		case TeamColor::Black:
			return QColor(Qt::black);
	}
	
	return QColor(Qt::red);
}

/**
 * Converts Qt RGB color into team color enumeration value.
 * \param color Qt RGB color which is converted.
 * \return Returns corresponding team color enumeration value.
 * \sa teamColor(BOOST_SCOPED_ENUM(TeamColor))
 */
inline BOOST_SCOPED_ENUM(TeamColor) teamColor(const QColor &color)
{
	if (color == Qt::red)
		return TeamColor::Red;
	else if (color == Qt::blue)
		return TeamColor::Blue;
	else if (color == 0x1CB619)
		return TeamColor::Teal;
	else if (color == 0x800080)
		return TeamColor::Purple;
	else if (color == Qt::yellow)
		return TeamColor::Yellow;
	else if (color == 0xFF8000)
		return TeamColor::Orange;
	else if (color == Qt::green)
		return TeamColor::Green;
	else if (color == 0xFF80C0)
		return TeamColor::Pink;
	else if (color == 0xC0C0C0)
		return TeamColor::Gray;
	else if (color == 0x0080FF)
		return TeamColor::LightBlue;
	else if (color == 0x106246)
		return TeamColor::DarkGreen;
	else if (color == 0x804000)
		return TeamColor::Brown;
	else if (color == Qt::black)
		return TeamColor::Black;
	
	return TeamColor::Red;
}

/**
 * Required by textures which use replaceable id \ref mdlx::ReplaceableId::TeamColor.
 * \sa mdlx::ReplaceableId, mdlx::Texture
 */
inline KUrl teamColorUrl(BOOST_SCOPED_ENUM(TeamColor) teamColor)
{
	QString number = QString::number((int)teamColor);
	
	if (number.size() == 1)
		number.prepend('0');

	return KUrl("ReplaceableTextures/TeamColor/TeamColor" + number + ".blp");
}

/**
 * Required by textures which use replaceable id \ref mdlx::ReplaceableId::TeamGlow.
 * \sa mdlx::ReplaceableId, mdlx::Texture
 */
inline KUrl teamGlowUrl(BOOST_SCOPED_ENUM(TeamColor) teamGlow)
{
	QString number = QString::number((int)teamGlow);

	if (number.size() == 1)
		number.prepend('0');

	return KUrl("ReplaceableTextures/TeamGlow/TeamGlow" + number + ".blp");
}

inline KUrl installUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl());
}

inline KUrl installXUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("InstallPathX");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl());
}

inline KUrl war3Url()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	KUrl url(settings.value("???").toUrl());
	url.addPath("war3.mpq");
	
	return url;
}

inline KUrl war3XUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("InstallPathX");
	
	if (!settings.contains("???"))
		return KUrl();
	
	KUrl url(settings.value("???").toUrl());
	url.addPath("war3x.mpq");
	
	return url;
}

inline KUrl war3PatchUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	KUrl url(settings.value("???").toUrl());
	url.addPath("War3Patch.mpq");
	
	return url;
}

inline KUrl war3XLocalUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	KUrl url(settings.value("???").toUrl());
	url.addPath("War3xlocal.mpq");
	
	return url;
}

}

}

#endif
