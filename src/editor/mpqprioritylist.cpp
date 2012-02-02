/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include <KIO/NetAccess>
#include <KMessageBox>

#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

MpqPriorityListEntry::MpqPriorityListEntry(const KUrl &url, Priority priority) : m_priority(priority), m_url(url)
{
}

bool MpqPriorityList::addSource(const KUrl &url, MpqPriorityListEntry::Priority priority)
{
	// proper URLs must refer to directories or archives
	/// \todo Support all archive properties and remote directories (smb).
	if (!QFileInfo(url.toLocalFile()).isDir() && (url.protocol() != MpqProtocol::protocol || url.protocol() != "tar"))
		return false;

	Sources::index_const_iterator<KUrl>::type iterator = sources().get<KUrl>().find(url);

	if (iterator != sources().get<KUrl>().end())
		return false;

	Source ptr(new MpqPriorityListEntry(url, priority));
	sources().push_back(ptr);

	return true;
}

bool MpqPriorityList::addWar3Source()
{
	KUrl url = war3Url();

	if (url.isEmpty())
		return false;

	return addSource(url, 20);
}

bool MpqPriorityList::addWar3PatchSource()
{
	KUrl url = war3PatchUrl();

	if (url.isEmpty())
		return false;

	return addSource(url, 22);
}

bool MpqPriorityList::addWar3XSource()
{
	KUrl url = war3XUrl();

	if (url.isEmpty())
		return false;

	return addSource(url, 21);
}

bool MpqPriorityList::addWar3XLocalSource()
{
	KUrl url = war3XLocalUrl();

	if (url.isEmpty())
		return false;

	return addSource(url, 21);
}

bool MpqPriorityList::addDefaultSources()
{
	return addWar3Source() &&
	addWar3PatchSource() &&
	addWar3Source() &&
	addWar3XLocalSource();
}

bool MpqPriorityList::removeSource(const KUrl &url)
{
	Sources::index_iterator<KUrl>::type iterator = sources().get<KUrl>().find(url);

	if (iterator == sources().get<KUrl>().end())
		return false;

	sources().get<KUrl>().erase(iterator);

	return true;
}

bool MpqPriorityList::removeWar3Source()
{
	KUrl url = war3Url();

	if (url.isEmpty())
		return false;

	return removeSource(url);
}


bool MpqPriorityList::removeWar3PatchSource()
{
	KUrl url = war3PatchUrl();

	if (url.isEmpty())
		return false;

	return removeSource(url);
}

bool MpqPriorityList::removeWar3XSource()
{
	KUrl url = war3XUrl();

	if (url.isEmpty())
		return false;

	return removeSource(url);
}

bool MpqPriorityList::removeWar3XLocalSource()
{
	KUrl url = war3XLocalUrl();

	if (url.isEmpty())
		return false;

	return removeSource(url);
}

bool MpqPriorityList::removeDefaultSources()
{
	return removeWar3Source() &&
	removeWar3PatchSource() &&
	removeWar3XSource() &&
	removeWar3XLocalSource();
}

bool MpqPriorityList::download(const KUrl &src, QString &target, QWidget *window)
{
	if (!src.isValid())
		return false;

	if (!src.isRelative())
		return KIO::NetAccess::download(src, target, window);

	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_FOREACH(const Source entry, sources().get<MpqPriorityListEntry::Priority>())
	{
		KUrl absoluteSource;

		if (!src.isRelative())
		{
			absoluteSource = src;
		}
		// entry path can be a directory path or something like tar:/... or mpq:/...
		else
		{
			absoluteSource = entry->url();
			absoluteSource.addPath(src.toLocalFile());
		}

		if (KIO::NetAccess::download(absoluteSource, target, window))
			return true;
	}

	return false;
}

bool MpqPriorityList::upload(const QString &src, const KUrl &target, QWidget *window)
{
	if (!target.isValid())
		return false;

	if (!target.isRelative())
		return KIO::NetAccess::upload(src, target, window);

	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_FOREACH(const Source entry, sources().get<MpqPriorityListEntry::Priority>())
	{
		KUrl absoluteTarget;

		if (!target.isRelative())
		{
			absoluteTarget = target;
		}
		// entry path can be a directory path or something like tar:/... or mpq:/...
		else
		{
			absoluteTarget = entry->url();
			absoluteTarget.addPath(target.toLocalFile());
		}

		if (KIO::NetAccess::upload(src, absoluteTarget, window))
			return true;
	}

	return false;
}

QMap< QString, QString > MpqPriorityList::txtEntries(QWidget *widget, const KUrl &url, const QString &group) const
{
	QString target;

	if (!const_cast<MpqPriorityList*>(this)->download(url, target, widget))
	{
		qDebug() << i18n("Missing file \"%1\".", url.toEncoded().constData());

		return QMap<QString,QString>();
	}

	KConfig config(target);

	return config.entryMap(group);
}

QString MpqPriorityList::tr(QWidget *widget, const QString &key, const QString &group, BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale, const QString &defaultValue) const
{
	QStringList files;
	// all files which contain strings
	files
	<< "UI/WorldEditStrings.txt"
	<< "UI/WorldEditGameStrings.txt"
	<< "UI/TriggerStrings.txt"
	<< "UI/TipStrings.txt"
	<< "UI/CampaignStrings.txt"
	;

	foreach (QString file, files)
	{
		QMap<QString, QString> entries = txtEntries(widget, file, group);

		if (entries.find(key) != entries.end())
			return entries[key];
	}

	if (!defaultValue.isEmpty())
		return defaultValue;

	return group + "[" + key + "]";
}

MpqPriorityList::Sources& MpqPriorityList::sources()
{
	return m_sources;
}

}

}
