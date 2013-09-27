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
	if (!QFileInfo(url.toLocalFile()).isDir() && url.protocol() != MpqProtocol::protocol && url.protocol() != "tar")
		return false;

	Sources::index_const_iterator<KUrl>::type iterator = sources().get<KUrl>().find(url);

	if (iterator != sources().get<KUrl>().end())
		return false;

	Source ptr(new MpqPriorityListEntry(url, priority));
	
	qDebug() << "Size before pushing back: " << sources().size();
	sources().push_back(ptr);
	qDebug() << "Size after pushing back: " << sources().size();

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

void MpqPriorityList::refreshDefaultFiles(QWidget* window)
{
	this->refreshWorldEditorStrings(window);
	this->refreshTriggerStrings(window);
	this->refreshTriggerData(window);
}

bool MpqPriorityList::download(const KUrl &src, QString &target, QWidget *window)
{
	qDebug() << "Download: " << src.url();

	if (!src.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::download(src, target, window))
			return true;
	}

	qDebug() << "Sources size: " << sources().get<MpqPriorityListEntry>().size();

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(const Source entry, sources().get<MpqPriorityListEntry>())
	{
		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteSource = entry->url();
		absoluteSource.addPath(src.toLocalFile());

		qDebug() << "entry url: " << entry->url();
		qDebug() << "local file: " << src.toLocalFile();
		qDebug() << "Trying " << absoluteSource.url();

		if (KIO::NetAccess::download(absoluteSource, target, window))
		{
			qDebug() << "Downloaded successfully";
			
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
	BOOST_REVERSE_FOREACH(const Source entry, sources().get<MpqPriorityListEntry>())
	{
		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteTarget = entry->url();
		absoluteTarget.addPath(target.toLocalFile());

		if (KIO::NetAccess::upload(src, absoluteTarget, window))
			return true;
	}

	return false;
}

bool MpqPriorityList::mkdir(const KUrl &target, QWidget *window)
{
	if (!target.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::mkdir(target, window))
			return true;
	}

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(const Source entry, sources().get<MpqPriorityListEntry>())
	{
		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteTarget = entry->url();
		absoluteTarget.addPath(target.toLocalFile());

		if (KIO::NetAccess::mkdir(absoluteTarget, window))
			return true;
	}

	return false;
}

QString MpqPriorityList::tr(QWidget *widget, const QString &key, const QString &group, BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale, const QString &defaultValue) const
{
	// TODO if group is empty, search all sections
	if (worldEditorStrings().get() != 0) {
		qDebug() << "Trying it in WorldEditorStrings.txt";
		
		try {
			const map::Txt::Pairs &pairs = worldEditorStrings()->entries(group.toUtf8().constData());
			
			// TODO linear search
			foreach (map::Txt::Pairs::const_reference ref, pairs) {
				if (ref.first == key.toUtf8().constData()) {
					return ref.second.c_str();
				}
			}
		} catch (Exception &e) {
		}
	}
	
	QStringList files;
	// all files which contain strings
	files
	<< "UI/WorldEditStrings.txt"
	<< "UI/WorldEditGameStrings.txt"
	<< "UI/TriggerStrings.txt"
	<< "UI/TipStrings.txt"
	<< "UI/CampaignStrings.txt"
	;
	
	if (!defaultValue.isEmpty())
		return defaultValue;

	return group + "[" + key + "]";
}

void MpqPriorityList::readSettings(const QString& group)
{
	qDebug() << "Reading settings for group " << group;
	
	QSettings settings("wc3editor", "wc3editor");
	qDebug() << "Settings file name: " << settings.fileName();
	settings.beginGroup(group);
	const int size = settings.beginReadArray("entries");

	for (int i = 0; i < size; ++i)
	{
		settings.setArrayIndex(i);
		const KUrl url = KUrl(settings.value("url").toByteArray());
		const int priority = settings.value("priority").toInt();
		qDebug() << "Loading source url " << url;
		qDebug() << "With priority " << priority;
		this->addSource(url, priority);
		++i;
	}

	settings.endArray();
	settings.endGroup();
}

void MpqPriorityList::writeSettings(const QString& group)
{
	qDebug() << "Writing settings for group " << group;
	
	QSettings settings("wc3editor", "wc3editor");
	settings.beginGroup(group);
	settings.beginWriteArray("entries");
	int i = 0;
	
	qDebug() << "Size 1: " << sources().size();
	qDebug() << "Size 2: " << sources().get<MpqPriorityListEntry>().size();

	BOOST_FOREACH(const Source &entry, sources().get<MpqPriorityListEntry>())
	{
		settings.setArrayIndex(i);
		qDebug() << "Storing url " << entry->url();
		settings.setValue("url", entry->url().toEncoded());
		const int priority = boost::numeric_cast<int>(entry->priority());
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
