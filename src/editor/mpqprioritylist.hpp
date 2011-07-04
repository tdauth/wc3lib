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

#ifndef WC3LIB_EDITOR_MPQPRIORITYLIST_HPP
#define WC3LIB_EDITOR_MPQPRIORITYLIST_HPP

#include <list>
#include <algorithm>

#include <boost/operators.hpp>
#include <boost/foreach.hpp>

#include <QFileInfo>

#include <KUrl>

#include "../mpq.hpp"
#include "../core.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Entry of an MPQ priority list.
 * An entry can either be an archive or a URL of a directory.
 * Each entry has its own priority. Entries with higher priority will be returned more likely than those with less priority.
 */
class MpqPriorityListEntry : public boost::operators<MpqPriorityListEntry>
{
	public:
		typedef MpqPriorityListEntry self;
		typedef std::size_t Priority;
		typedef boost::scoped_ptr<mpq::Mpq> MpqPtr;

		/**
		* If url is the URL of an archive it will be set with "mpq:" protocol.
		* Otherwise it has to be a directory.
		*/
		MpqPriorityListEntry(const KUrl &url, Priority priority);
		
		bool download(const KUrl &src, QString &target, QWidget *window);
		bool upload(const QString &src, const KUrl &target, QWidget *window);

		Priority priority() const;
		bool isDirectory() const;
		bool isArchive() const;
		const KUrl& url() const;

		bool operator<(const self& other) const;
		bool operator==(const self& other) const;
	protected:
		Priority m_priority;
		KUrl m_url;
		MpqPtr m_mpq;
};

inline MpqPriorityListEntry::Priority MpqPriorityListEntry::priority() const
{
	return this->m_priority;
}

inline bool MpqPriorityListEntry::isDirectory() const
{
	return url.isLocalFile() && QFileInfo(url.toLocalFile()).isDir();
}

inline bool MpqPriorityListEntry::isArchive() const
{
	return !isDirectory();
}

inline const KUrl& MpqPriorityListEntry::url() const
{
	return this->m_url;
}

inline bool MpqPriorityListEntry::operator<(const self& other) const
{
	return this->priority() < other.priority();
}

inline bool MpqPriorityListEntry::operator==(const self& other) const
{
	return this->priority() == other.priority();
}

typedef boost::shared_ptr<MpqPriorityListEntry> MpqPriorityListEntryPtr;
typedef boost::multi_index_container<MpqPriorityListEntryPtr,
boost::multi_index::indexed_by<
// simple list
boost::multi_index::sequenced<>,
// ordered by itself
boost::multi_index::ordered_unique<boost::multi_index::tag<MpqPriorityListEntry>, boost::multi_index::identity<MpqPriorityListEntry> >,
// ordered by its corresponding priority
boost::multi_index::ordered_non_unique<boost::multi_index::tag<MpqPriorityListEntry>, boost::multi_index::const_mem_fun<MpqPriorityListEntry, MpqPriorityListEntry::Priority, &MpqPriorityListEntry::priority> >,
// ordered by its corresponding URL
boost::multi_index::ordered_non_unique<boost::multi_index::tag<KUrl>, boost::multi_index::const_mem_fun<MpqPriorityListEntry, const KUrl&, &MpqPriorityListEntry::url> >

> >

MpqPriorityListBase;

/**
 * MPQ priority lists can be used to make several data sources usable considering MPQ locales and sources' priorities (for patch archives etc.).
 */
class MpqPriorityList : public MpqPriorityListBase
{
	public:
		typedef MpqPriorityListBase base;
		typedef MpqPriorityList self;

		void setLocale(mpq::MpqFile::Locale locale);
		mpq::MpqFile::Locale locale() const;

		/**
		 * Adds an entry with URL \p url and priorioty \p priority to the priority list.
		 * Each URL should be unique and refer to an archive (mostly MPQ) or directory.
		 * \param priority 0 is default value. Priorities needn't to be unique.
		 * \return Returns true if the URL has been added to the list (this doesn't happen if there already is an entry with the given URL or if it refers to an absolute file path which is no archive or directory).
		 * \todo Improve archive detection.
		 */
		bool addEntry(const KUrl &url, MpqPriorityListEntry::Priority priority = 0);
		/**
		 * Removes an entry by its corresponding URL.
		 * \return Returns true if URL corresponds to some entry and that entry has been removed properly.
		 */
		bool removeEntry(const KUrl &url);

		/**
		 * \copydoc KIO::NetAccess::download()
		 * Considers all entries if it's an relative URL. Otherwise it will at least consider the priority list's locale if none is given.
		 * \todo If it's an "mpq:/" URL and there is no locale given add one considering \ref locale().
		 */
		bool download(const KUrl &src, QString &target, QWidget *window);
		bool upload(const QString &src, const KUrl &target, QWidget *window);

	protected:
		mpq::MpqFile::Locale m_locale;
};

inline void MpqPriorityList::setLocale(mpq::MpqFile::Locale locale)
{
	this->m_locale = locale;
}

inline mpq::MpqFile::Locale MpqPriorityList::locale() const
{
	return this->m_locale;
}

}

}

#endif
