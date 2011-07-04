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

#include <boost/foreach.hpp>

#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

MpqPriorityListEntry::MpqPriorityListEntry(const KUrl &url, Priority priority) : m_priority(priority), m_isDirectory(false), m_isValid(false)
{
	// if url is directory, directory will be set true
	// if directory is readable it will be set valid
	// else if url is MPQ file it will be set correctly with mpq protocol!
	// else it will stay invalid
}

const KUrl& MpqPriorityList::findFile(const KUrl &url) const
{
	return this->findFile(url, this->locale());
}

const KUrl& MpqPriorityList::findFile(const KUrl &url, mpq::MpqFile::Locale locale) const
{
	return url; // TEST

	if (!url.isValid() || !url.isLocalFile()) /// @todo Support non-local files (mpq: protocol).
		return KUrl("");

	std::list<const MpqPriorityListEntry*> validEntries;

	BOOST_FOREACH(EntryType entry, *this)
	{
		if (!entry.second->isValid())
			continue;

		validEntries.push_back(entry.second);
	}

	// usually map should be sorted in correct order
	// this is an additional, optional safety sorting statement!
	//std::sort(validEntries.begin(), validEntries.end());
	std::map<KUrl, MpqPriorityListEntry::Priority> urlEntries;

	BOOST_FOREACH(const MpqPriorityListEntry *entry, validEntries)
	{
		if (entry->isDirectory())
		{
			//if (url.protocol() != "file")
				continue;

			if (url.isLocalFile())
			{
				// "Textures/Gna.blp" could be located in directory
				if (url.isRelative())
				{
					if (!entry->url().isParentOf(url)) /// @todo Probably wrong check, checks not on file system, checks only path strings!!!
						continue;
				}
				// "/dir path/Textures/Gna.blp" contains directory path at beginning
				else if (url.toLocalFile().left(entry->url().toLocalFile().length()) == entry->url().toLocalFile())
				{
					QString relativePath = url.toLocalFile().mid(entry->url().toLocalFile().length());

					/// @todo Check if directory (entry->url()) contains relative path as valid file
					//if (
					//
				}
			}
		}
		/// @todo Implement MPQ protocol later
		/*
		else
		{
			if (url.protocol() != "mpq")
				continue;
		}
		*/
	}


	if (urlEntries.empty())
		return KUrl("");

	return KUrl(""); // TODO Return first matching entry.
}

}

}
