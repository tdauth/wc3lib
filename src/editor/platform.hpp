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

#include "config.h"

#include <boost/filesystem.hpp>

#include <QColor>
#include <QString>
#include <QList>
#include <QUrl>
#ifdef Q_OS_UNIX
#include <QDir>
#include <QFileInfo>
#else // for Windows
#include <QSettings>
#endif
#include <QDir>
#include <QFileInfo>
#include <QVariant>

#include "../blp.hpp"
#include "../map/value.hpp"
#ifdef MDLX
#include "../mdlx.hpp"
#endif

namespace wc3lib
{

namespace editor
{

/**
 * Converts a Warcraft III string into a QString object.
 * \param value The string which is converted into a QString object.
 * \return Returns the converted string for Qt.
 */
inline QString stringToQString(const wc3lib::string &value)
{
	return QString::fromStdString(value);
}

/**
 * Converts a Boost filesystempath into a QString object.
 * \param value The string which is converted into a QString object.
 * \return Returns the converted string for Qt.
 * \note On Windows the path uses wchar for strings.
 */
inline QString pathToQString(const boost::filesystem::path &value)
{
	return QString::fromStdString(value.string());
}

/**
 * Converts ARGB color \p argb into a BLP RGBA color.
 * \return Returns BLP RGBA color.
 * \ingroup colors
 */
inline blp::color argbToColor(QRgb argb)
{
	return (argb << 8) | qAlpha(argb);
}

/**
 * Converts BLP RGBA color \p c into a Qt ARGB color.
* \return Returns Qt ARGB color.
* \ingroup colors
*/
inline QRgb colorToArgb(blp::color c)
{
	return (c >> 8) | (blp::alpha(c) << 24);
}

namespace
{

inline QList<QVariant> stringList(const map::List &list)
{
	QList<QVariant> result;

	for (std::size_t i = 0; i < list.size(); ++i)
	{
		// Warcraft III usually works with UTF-8 strings
		result << QVariant(QString::fromUtf8(list[i].c_str()));
	}

	return result;
}

}

/**
 * Converts a Warcraft III object data value into a QVariant keeping the type.
 *
 * \param value The value which is converted into a QVariant.
 *
 * \return Returns a QVariant with the value's content keeping its underlying type.
 *
 * \sa valueToString()
 */
inline QVariant valueToVariant(const map::Value &value)
{
	switch (value.type())
	{
		case map::Value::Type::Integer:
		{
			return QVariant(value.toInteger());
		}

		case map::Value::Type::Real:
		case map::Value::Type::Unreal:
		{
			return QVariant(value.toReal());
		}

		case map::Value::Type::String:
		case map::Value::Type::RegenerationType:
		case map::Value::Type::AttackType:
		case map::Value::Type::WeaponType:
		case map::Value::Type::TargetType:
		case map::Value::Type::MoveType:
		case map::Value::Type::DefenseType:
		case map::Value::Type::PathingTexture:
		case map::Value::Type::MissileArt:
		case map::Value::Type::AttributeType:
		case map::Value::Type::AttackBits:
		{
			// Warcraft III usually works with UTF-8 strings
			return QVariant(QString::fromUtf8(value.toString().c_str()));
		}

		case map::Value::Type::Boolean:
		{
			return QVariant(value.toBoolean());
		}

		case map::Value::Type::Character:
		{
			return QVariant(value.toCharacter());
		}

		case map::Value::Type::UnitList:
		case map::Value::Type::ItemList:
		case map::Value::Type::UpgradeList:
		case map::Value::Type::StringList:
		case map::Value::Type::AbilityList:
		case map::Value::Type::HeroAbilityList:
		{
			return QVariant(stringList(value.toList()));
		}
	}

	return QVariant();
}

/**
 * Converts a Warcraft III object data value into a QString.
 *
 * \param value The value which is converted into a QString.
 *
 * \return Returns a QString with the value's content.
 *
 * \sa valueToVariant()
 */
inline QString valueToString(const map::Value &value)
{
	switch (value.type())
	{
		case map::Value::Type::Integer:
		{
			return QString::number(value.toInteger());
		}

		case map::Value::Type::Real:
		case map::Value::Type::Unreal:
		{
			return QString::number(value.toReal());
		}

		case map::Value::Type::String:
		case map::Value::Type::RegenerationType:
		case map::Value::Type::AttackType:
		case map::Value::Type::WeaponType:
		case map::Value::Type::TargetType:
		case map::Value::Type::MoveType:
		case map::Value::Type::DefenseType:
		case map::Value::Type::PathingTexture:
		case map::Value::Type::MissileArt:
		case map::Value::Type::AttributeType:
		case map::Value::Type::AttackBits:
		{
			return QString::fromUtf8(value.toString().c_str());
		}

		case map::Value::Type::Boolean:
		{
			return QString::number(value.toBoolean());
		}

		case map::Value::Type::Character:
		{
			return QString(value.toCharacter());
		}

		case map::Value::Type::UnitList:
		case map::Value::Type::ItemList:
		case map::Value::Type::UpgradeList:
		case map::Value::Type::StringList:
		case map::Value::Type::AbilityList:
		case map::Value::Type::HeroAbilityList:
		{
			QString result;
			int i = 0;

			foreach (string value, value.toList())
			{
				if (i > 0)
				{
					result += ",";
				}

				result += QString::fromUtf8(value.c_str());
				++i;
			}

			return result;
		}
	}

	return QString();
}

/**
 * \defgroup teamcolors Team Colors
 *
 * Team colors are used to identify the owner of a Unit.
 * Usually parts of the texture are reserved for the team color and are colored depending on the owning player
 * of the unit.
 * The enum specifies all team colors which are available in Warcraft III.
 *
 * Use \ref MaxTeamColors to get the number of team colors.
 */
enum class TeamColor
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
};

static const int MaxTeamColors = static_cast<int>(TeamColor::Black) + 1;

/**
 * Converts team color enumeration value into Qt RGB color.
 * \param teamColor Team color which is converted.
 * \return Returns corresponding Qt RGB color.
 * \sa teamColor(const QColor&)
 *
 * \ingroup teamcolors
 */
inline QColor teamColor(TeamColor teamColor)
{
	switch (teamColor)
	{
		case TeamColor::Red:
		{
			return QColor(Qt::red);
		}

		case TeamColor::Blue:
		{
			return QColor(Qt::blue);
		}

		case TeamColor::Teal:
		{
			return QColor(0x1CB619);
		}

		case TeamColor::Purple:
		{
			return QColor(0x800080);
		}

		case TeamColor::Yellow:
		{
			return QColor(Qt::yellow);
		}

		case TeamColor::Orange:
		{
			return QColor(0xFF8000);
		}

		case TeamColor::Green:
		{
			return QColor(Qt::green);
		}

		case TeamColor::Pink:
		{
			return QColor(0xFF80C0);
		}

		case TeamColor::Gray:
		{
			return QColor(0xC0C0C0);
		}

		case TeamColor::LightBlue:
		{
			return QColor(0x0080FF);
		}

		case TeamColor::DarkGreen:
		{
			return QColor(0x106246);
		}

		case TeamColor::Brown:
		{
			return QColor(0x804000);
		}

		case TeamColor::Black:
		{
			return QColor(Qt::black);
		}
	}

	return QColor(Qt::red);
}

/**
 * Converts Qt RGB color into team color enumeration value.
 * \param color Qt RGB color which is converted.
 * \return Returns corresponding team color enumeration value.
 * \sa teamColor(TeamColor)
 *
 * \ingroup teamcolors
 */
inline TeamColor teamColor(const QColor &color)
{
	if (color == Qt::red)
	{
		return TeamColor::Red;
	}
	else if (color == Qt::blue)
	{
		return TeamColor::Blue;
	}
	else if (color == 0x1CB619)
	{
		return TeamColor::Teal;
	}
	else if (color == 0x800080)
	{
		return TeamColor::Purple;
	}
	else if (color == Qt::yellow)
	{
		return TeamColor::Yellow;
	}
	else if (color == 0xFF8000)
	{
		return TeamColor::Orange;
	}
	else if (color == Qt::green)
	{
		return TeamColor::Green;
	}
	else if (color == 0xFF80C0)
	{
		return TeamColor::Pink;
	}
	else if (color == 0xC0C0C0)
	{
		return TeamColor::Gray;
	}
	else if (color == 0x0080FF)
	{
		return TeamColor::LightBlue;
	}
	else if (color == 0x106246)
	{
		return TeamColor::DarkGreen;
	}
	else if (color == 0x804000)
	{
		return TeamColor::Brown;
	}
	else if (color == Qt::black)
	{
		return TeamColor::Black;
	}

	return TeamColor::Red;
}

/**
 * Required by textures which use replaceable id \ref mdlx::ReplaceableId::TeamColor.
 * \sa mdlx::ReplaceableId, mdlx::Texture
 */
inline QUrl teamColorUrl(TeamColor teamColor)
{
	QString number = QString::number((int)teamColor);

	if (number.size() == 1)
	{
		number.prepend('0');
	}

	return QUrl("ReplaceableTextures/TeamColor/TeamColor" + number + ".blp");
}

/**
 * Required by textures which use replaceable id \ref mdlx::ReplaceableId::TeamGlow.
 * \sa mdlx::ReplaceableId, mdlx::Texture
 */
inline QUrl teamGlowUrl(TeamColor teamGlow)
{
	QString number = QString::number((int)teamGlow);

	if (number.size() == 1)
	{
		number.prepend('0');
	}

	return QUrl("ReplaceableTextures/TeamGlow/TeamGlow" + number + ".blp");
}

/**
 * Reads Windows registry entry with \p key and returns the result.
 * If wc3lib is compiled on a Unix system this will try to load the Windows registry provided by wine.
 * Otherwise it returns just QVariant().
 * This function is mostly used for World Editor settings which are tried to reproduce by the editor module.
 * \todo Fix wine implementation
 * http://www.c-plusplus.de/forum/292394
 */
inline QVariant registryEntry(const QString &key)
{
#ifdef Q_OS_UNIX
	/*
	QProcess process(this);

	if (process.execute("wine reg query " + key) != 0)
		return QVariant();

	QByteArray output = process.readAll();
	*/
	QString fileName;
	int index = key.indexOf('\\');
	QString realKey = key;

	// if there is no HKEY at the beginning we use user registry by default
	if (index == -1)
	{
		fileName = "/.wine/user.reg";
	}
	else
	{
		QString start = key.mid(index);

		if (start == "HKEY_CURRENT_USER")
		{
			fileName = "/.wine/user.reg";
		}
		else if (index != 0)  // TODO support other registry files
		{
			return QVariant();
		}

		realKey = realKey.mid(index + 2); /* skip key root plus \\ */

		if (realKey.isEmpty())
		{
			return QVariant();
		}
	}

	QFileInfo info(QDir::homePath() + fileName);

	if (!info.isFile() || !info.isReadable())
	{
		return QVariant();
	}

	QFile file(info.absolutePath());

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return QVariant();
	}

	while (file.canReadLine())
	{
		QString line = file.readLine();

		if (line.isEmpty())
		{
			continue;
		}

		int index = line.indexOf(key);

		if (index != -1 && line[0] == '[')
		{
			/*
			int start = index + key.length + 2; // + length of "] "

			if (line.length <= start) // there is no value/invalid entry
				return QVariant();


			return QVariant(line.mid(start));
			*/
			if (!file.canReadLine())
			{
				return QVariant();
			}

			return file.readLine();
		}
	}

	return QVariant();
#else
#ifndef Q_OS_WIN32
#warning Unsupported platform.
#endif
	// read from native Windows registry
	QSettings settings(key, QSettings::NativeFormat);

	return settings.value("Default"); // TODO always key "Default"?
#endif
}

inline QUrl installUrl()
{
	return registryEntry("Software\\Blizzard Entertainment\\Warcraft III\\Install Path").toUrl();
}

inline QUrl installXUrl()
{
	return registryEntry("Software\\Blizzard Entertainment\\Warcraft III\\InstallPathX").toUrl();
}

inline QUrl war3Url()
{
	QUrl url(installUrl());

	if (url.isEmpty())
	{
		return QUrl();
	}

	return QUrl(url.toString() + "/war3.mpq");
}

inline QUrl war3XUrl()
{
	QUrl url(installXUrl());

	if (url.isEmpty())
	{
		return QUrl();
	}

	return QUrl(url.toString() + "/war3x.mpq");
}

inline QUrl war3PatchUrl()
{
	QUrl url(installUrl());

	if (url.isEmpty())
	{
		return QUrl();
	}

	return QUrl(url.toString() + "/War3Patch.mpq");
}

inline QUrl war3XLocalUrl()
{
	QUrl url(installXUrl());

	if (url.isEmpty())
	{
		return QUrl();
	}

	return QUrl(url.toString() + "/War3xlocal.mpq");
}

/**
 * \defgroup dialogfilters Dialog Filters
 *
 * \brief Filters for file types in GUI dialogs.
 */

/**
 * \brief Returns the filter string for GUI dialogs which is required for Warcraft III map files.
 *
 * \note Includes all files.
 *
 * \ingroup dialogfilters
 */
inline QString mapFilter()
{
	return QObject::tr("Warcraft III: Reign of Chaos map (*.w3m);;Warcraft III: The Frozen Throne map (*.w3x)");
}

/**
 * \brief Returns the filter string for GUI dialogs which is required for Warcraft III object collection files
 *
 * \note Includes all files.
 *
 * \ingroup dialogfilters
 */
inline QString objectsCollectionFilter()
{
	return QObject::tr("Warcraft III Objects Collection (*.w3o)");
}

/**
 * \brief Returns the filter string for GUI dialogs which is required for Warcraft III trigger files.
 *
 * \note Includes all files.
 *
 * \ingroup dialogfilters
 */
inline QString triggersFilter()
{
	return QObject::tr("Warcraft III Triggers (*.wtg)");
}

/**
 * \brief Returns the filter string for GUI dialogs which is required for Warcraft III custom text trigger files.
 *
 * \note Includes all files.
 *
 * \ingroup dialogfilters
 */
inline QString customTextTriggersFilter()
{
	return QObject::tr("Warcraft III Custom Text Triggers (*.wtc)");
}

// TODO Use image open URL function but MIME type of BLP is not usable on debugging
// TODO MIME filters do not work ("all/allfiles").
//i18n("*|All Files\n*.blp|Blizzard Pictures\n*.png|Portable Network Graphics\n*.jpg|JPEG Files"), this, i18n("Open texture"));

/**
 * wc3lib brings some standard listfiles which are installed in a directory.
 * These can be used if an MPQ archive does not contain its own listfile.
 */
inline QFileInfoList installedListfiles()
{
#ifndef Q_OS_UNIX
#warning Only Unix systems do support automatic listfiles yet.
	return QFileInfoList();
#else
	const QDir dir("/usr/share/wc3lib/listfiles/");
	QFileInfoList result;

	foreach (QFileInfo fileInfo, dir.entryInfoList())
	{
		if (fileInfo.suffix() == "txt")
		{
			result.append(fileInfo);
		}
	}

	return result;
#endif
}

// http://stackoverflow.com/questions/11050977/removing-a-non-empty-folder-in-qt
// TODO Qt 5 QDir::removeRecursively()
inline bool removeDirRecursively(const QString & dirName)
{
	QDir dir(dirName);

	return dir.removeRecursively();
}

}

}

#endif
