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

MpqPriorityList::MpqPriorityList() : m_sharedData(new WarcraftIIIShared(this))
{

}

MpqPriorityList::~MpqPriorityList()
{
}


bool MpqPriorityList::addSource(const KUrl &url, MpqPriorityListEntry::Priority priority)
{
	// proper URLs must refer to directories or archives
	/// \todo Support all archive properties and remote directories (smb).
	if (!QFileInfo(url.toLocalFile()).isDir() && url.protocol() != "mpq" && url.protocol() != "tar")
	{
		return false;
	}

	std::auto_ptr<MpqPriorityListEntry> ptr(new MpqPriorityListEntry(url, priority));
	sources().insert(ptr);

	return true;
}

bool MpqPriorityList::addWar3Source()
{
	KUrl url = war3Url();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 20);
}

bool MpqPriorityList::addWar3PatchSource()
{
	KUrl url = war3PatchUrl();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 22);
}

bool MpqPriorityList::addWar3XSource()
{
	KUrl url = war3XUrl();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 21);
}

bool MpqPriorityList::addWar3XLocalSource()
{
	KUrl url = war3XLocalUrl();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 21);
}

bool MpqPriorityList::addDefaultSources()
{
	return addWar3Source() &&
	addWar3PatchSource() &&
	addWar3Source() &&
	addWar3XLocalSource();
}

bool MpqPriorityList::removeSource(const KUrl& url)
{
	Sources::iterator iterator = std::find_if(this->sources().begin(), this->sources().end(), [&url](Sources::const_reference ref){ return ref.url() == url; });

	if (iterator == this->sources().end())
	{
		return false;
	}

	this->m_sources.erase(iterator);

	return true;
}

bool MpqPriorityList::removeSource(Source &source)
{
	Sources::iterator iterator = sources().find(source);

	if (iterator == sources().end())
	{
		return false;
	}

	sources().erase(iterator);

	return true;
}

bool MpqPriorityList::removeWar3Source()
{
	KUrl url = war3Url();

	if (url.isEmpty())
	{
		return false;
	}

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
	qDebug() << "Download: " << src.url();

	if (!src.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::download(src, target, window))
		{
			qDebug() << "Downloaded successfully into file " << target;

			return true;
		}
	}

	qDebug() << "Sources size: " << sources().size();

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(Sources::const_reference entry, sources())
	{
		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteSource = entry.url();
		absoluteSource.addPath(src.toLocalFile());

		qDebug() << "entry url: " << entry.url();
		qDebug() << "local file: " << src.toLocalFile();
		qDebug() << "Trying " << absoluteSource.url();

		if (KIO::NetAccess::download(absoluteSource, target, window))
		{
			qDebug() << "Downloaded successfully into file " << target;

			return true;
		}
	}

	qDebug() << "Downloaded failed";

	return false;
}

bool MpqPriorityList::upload(const QString &src, const KUrl &target, QWidget *window)
{
	if (!target.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::upload(src, target, window))
			return true;
	}

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(Sources::const_reference entry, sources())
	{
		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteTarget = entry.url();
		absoluteTarget.addPath(target.toLocalFile());

		if (KIO::NetAccess::upload(src, absoluteTarget, window))
		{
			return true;
		}
	}

	return false;
}

bool MpqPriorityList::mkdir(const KUrl &target, QWidget *window)
{
	if (!target.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::mkdir(target, window))
		{
			return true;
		}
	}

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(Sources::const_reference entry, sources())
	{
		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteTarget = entry.url();
		absoluteTarget.addPath(target.toLocalFile());

		if (KIO::NetAccess::mkdir(absoluteTarget, window))
		{
			return true;
		}
	}

	return false;
}

void MpqPriorityList::readSettings(const QString& group)
{
	qDebug() << "Reading settings for group " << group;

	QSettings settings("wc3editor", "wc3editor");
	qDebug() << "Settings file name: " << settings.fileName();
	settings.beginGroup(group);
	const int size = settings.beginReadArray("entries");
	qDebug() << "Number of MPQ priority list entries: " << size;

	for (int i = 0; i < size; ++i)
	{
		qDebug() << "i is" << i;
		settings.setArrayIndex(i);
		const KUrl url = KUrl(settings.value("url").toByteArray());
		const int priority = settings.value("priority").toInt();
		qDebug() << "Loading source url " << url;
		qDebug() << "With priority " << priority;
		this->addSource(url, priority);
		qDebug() << "After adding source";
	}

	settings.endArray();
	settings.endGroup();
}

void MpqPriorityList::writeSettings(const QString& group)
{
	qDebug() << "Writing settings for group " << group;

	QSettings settings("wc3editor", "wc3editor");
	settings.beginGroup(group);
	settings.beginWriteArray("entries", sources().size());
	int i = 0;

	qDebug() << "Size 1: " << sources().size();

	BOOST_FOREACH(Sources::const_reference entry, sources())
	{
		settings.setArrayIndex(i);
		qDebug() << "Storing url " << entry.url();
		settings.setValue("url", entry.url().toEncoded());
		const int priority = boost::numeric_cast<int>(entry.priority());
		qDebug() << "Storing priority " << priority;
		settings.setValue("priority", priority);
		++i;
	}

	settings.endArray();
	settings.endGroup();
}

void MpqPriorityList::clear()
{
	sources().clear();
}

MpqPriorityList::Sources& MpqPriorityList::sources()
{
	return m_sources;
}

}

}
