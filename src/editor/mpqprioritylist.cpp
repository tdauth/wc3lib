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

#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

MpqPriorityListEntry::MpqPriorityListEntry(const KUrl &url, Priority priority) : m_priority(priority), m_url(url)
{
}

bool MpqPriorityList::addEntry(const KUrl &url, MpqPriorityListEntry::Priority priority)
{
	// proper URLs must refer to directories or archives
	/// \todo Support all archive properties and remote directories (smb).
	if (!QFileInfo(url.toLocalFile()).isDir() && (url.protocol() != "mpq" || url.protocol() != "tar"))
		return false;
	
	index_const_iterator<KUrl>::type iterator = get<KUrl>().find(url);
	
	if (iterator != get<KUrl>().end())
		return false;
	
	MpqPriorityListEntryPtr ptr(new MpqPriorityListEntry(url, priority));
	this->push_back(ptr);

	return true;
}

bool MpqPriorityList::removeEntry(const KUrl &url)
{
	index_iterator<KUrl>::type iterator = get<KUrl>().find(url);
	
	if (iterator != get<KUrl>().end())
		return false;
	
	get<KUrl>().erase(iterator);
	
	return true;
}

bool MpqPriorityList::download(const KUrl &src, QString &target, QWidget *window)
{
	if (!src.isValid())
		return false;
	
	if (!src.isRelative())
		return KIO::NetAccess::download(src, target, window);
	
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_FOREACH(const MpqPriorityListEntryPtr entry, get<MpqPriorityListEntry::Priority>())
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
	BOOST_FOREACH(const MpqPriorityListEntryPtr entry, get<MpqPriorityListEntry::Priority>())
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

QString MpqPriorityList::tr(const QString &key, const QString &group, BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale) const
{
	/// \todo open editor and other string files and get corresponding entries
	return group + "[" + key + "]";
}

}

}
