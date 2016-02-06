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

#include <boost/format.hpp>

#include <QtGui>

#include <KIO/NetAccess>

#include "warcraftiiishared.hpp"
#include "../exception.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

WarcraftIIIShared::WarcraftIIIShared(MpqPriorityList *source) : m_source(source), m_sharedObjectData(new SharedObjectData(source))
{
}

void WarcraftIIIShared::refreshDefaultFiles(QWidget *window)
{
	this->refreshWorldEditorStrings(window);
	this->refreshWorldEditorGameStrings(window);
	this->refreshWorldEditData(window);
	this->refreshTriggerStrings(window);
	this->refreshTriggerData(window);
}

Texture* WarcraftIIIShared::teamColorTexture(TeamColor teamColor) const
{
	TeamColorTextures::iterator iterator = this->m_teamColorTextures.find(teamColor);

	if (iterator == this->m_teamColorTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamColorUrl(teamColor)));
		texture->setSource(this->source());
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamColorTextures.insert(teamColor, texture);

		return result;
	}

	return this->m_teamColorTextures.find(teamColor)->second;
}

Texture* WarcraftIIIShared::teamGlowTexture(TeamColor teamGlow) const
{
	TeamColorTextures::iterator iterator = this->m_teamGlowTextures.find(teamGlow);

	if (iterator == this->m_teamGlowTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamGlowUrl(teamGlow)));
		texture->setSource(this->source());
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamGlowTextures.insert(teamGlow, texture);

		return result;
	}

	return this->m_teamGlowTextures.find(teamGlow)->second;
}

void WarcraftIIIShared::refreshMapStrings(QWidget* window, const KUrl url)
{
	MapStringsPtr ptr(new MetaData(url));
	ptr->setSource(this->source());
	ptr->load();

	m_mapStrings.swap(ptr); // exception safe
}

void WarcraftIIIShared::refreshWorldEditorStrings(QWidget *window, const KUrl &url)
{
	WorldEditorStringsPtr ptr(new MetaData(url));
	ptr->setSource(this->source());
	ptr->load();

	m_worldEditorStrings.swap(ptr); // exception safe
}

void WarcraftIIIShared::refreshWorldEditorGameStrings(QWidget *window, const KUrl &url)
{
	WorldEditorGameStringsPtr ptr(new MetaData(url));
	ptr->setSource(this->source());
	ptr->load();

	m_worldEditorGameStrings.swap(ptr); // exception safe
}

QString WarcraftIIIShared::tr(const QString &key, const QString &group, const QString &defaultValue) const
{
	QString result;

	if (this->mapStrings().get() != 0 && this->mapStrings()->hasValue(key, ""))
	{
		result = this->mapStrings()->value(key, "");
	}
	else if (this->worldEditorStrings().get() != 0 && this->worldEditorStrings()->hasValue(group, key))
	{
		result = this->worldEditorStrings()->value(group, key);
	}
	else if (this->worldEditorGameStrings().get() != 0 && this->worldEditorGameStrings()->hasValue(group, key))
	{
		result = this->worldEditorGameStrings()->value(group, key);
	}

	if (!result.isEmpty())
	{
		/*
		 * Some values like "WESTRING_UE_UNITRACE_HUMAN" refer to other keys and have to be resolved recursively until no
		 * reference is found anymore.
		 */
		if ((this->mapStrings().get() != 0 && this->mapStrings()->hasValue(key, "")) || (this->worldEditorStrings().get() != 0 && this->worldEditorStrings()->hasValue(group, result)) || (this->worldEditorGameStrings().get() != 0 && this->worldEditorGameStrings()->hasValue(group, result)))
		{
			qDebug() << "Recursion";

			return tr(result, group, defaultValue);
		}

		return result;
	}

	if (!defaultValue.isEmpty())
	{
		return defaultValue;
	}

	return group + "[" + key + "]";
}

QPixmap WarcraftIIIShared::pixmap(const KUrl &url, QWidget *window)
{
	Icons::iterator iterator = m_icons.find(url);

	if (iterator != m_icons.end())
	{
		return iterator.value();
	}

	QString iconFile;

	if (this->source()->download(url, iconFile, window))
	{
		iterator = m_icons.insert(url, QPixmap(iconFile));

		this->source()->removeTempFile(iconFile);

		return iterator.value();
	}
	else
	{
		// Insert a pixmap even if there has been an error on downloading the file. Otherwise calls will try to download a missing file again and again.
		iterator = m_icons.insert(url, QPixmap());

		qDebug() << "Error on downloading icon:" << url << KIO::NetAccess::lastErrorString().toUtf8().constData();
	}

	return QPixmap();
}

QPixmap WarcraftIIIShared::worldEditDataPixmap(const QString &key, const QString &group, QWidget *window)
{
	if (this->worldEditData().get() != 0)
	{
		QString filePath = MetaData::fromFilePath(this->worldEditData()->value(group, key));
		/*
		* In some cases the extension is not present.
		*/
		QFileInfo fileInfo(filePath);

		if (fileInfo.suffix().toLower() != "blp")
		{
			filePath += ".blp";
		}

		return this->pixmap(filePath, window);
	}

	return QPixmap();
}


QIcon WarcraftIIIShared::icon(const KUrl &url, QWidget *window)
{
	return QIcon(pixmap(url, window));
}

QIcon WarcraftIIIShared::worldEditDataIcon(const QString &key, const QString &group, QWidget *window)
{
	return QIcon(worldEditDataPixmap(key, group, window));
}

void WarcraftIIIShared::refreshWorldEditData(QWidget *window, const KUrl &url)
{
	WorldEditDataPtr ptr(new MetaData(url));
	ptr->setSource(this->source());
	ptr->load();

	m_worldEditData.swap(ptr); // exception safe
}

void WarcraftIIIShared::refreshTriggerData(QWidget *window, const KUrl &url)
{
	QString target;

	if (!this->source()->download(url, target, window))
	{
		throw Exception(_("Unable to download file \"UI/TriggerData.txt\"."));
	}

	qDebug() << "Trigger data target: " << target;
	TriggerDataPtr ptr(new map::TriggerData());
	ifstream ifstream(target.toStdString(), std::ios::in);

	if (!ifstream)
	{
		this->source()->removeTempFile(target);

		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toStdString());
	}

	ptr->read(ifstream);
	this->source()->removeTempFile(target);

	m_triggerData.swap(ptr); // exception safe
}

void WarcraftIIIShared::refreshTriggerStrings(QWidget *window, const KUrl &url)
{
	QString target;

	if (!this->source()->download(url, target, window))
	{
		throw Exception(_("Unable to download file \"UI/TriggerStrings.txt\"."));
	}

	qDebug() << "Trigger strings target: " << target;
	TriggerStringsPtr ptr(new map::TriggerStrings());
	ifstream ifstream(target.toStdString(), std::ios::in);

	if (!ifstream)
	{
		this->source()->removeTempFile(target);

		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toStdString());
	}

	ptr->read(ifstream);
	this->source()->removeTempFile(target);

	m_triggerStrings.swap(ptr); // exception safe
}

}

}