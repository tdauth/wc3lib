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

#include <QFileInfo>

#include <KUrl>

#include "platform.hpp"
#include "../mpq.hpp"
#include "resource.hpp"
#include "texture.hpp"
#include "ogremdlx.hpp"

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
};

inline MpqPriorityListEntry::Priority MpqPriorityListEntry::priority() const
{
	return this->m_priority;
}

inline bool MpqPriorityListEntry::isDirectory() const
{
	/// \todo Support remote directories (smb).
	return url().isLocalFile() && QFileInfo(url().toLocalFile()).isDir();
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

/**
 * MPQ priority lists can be used to make several data sources usable considering MPQ locales and sources' priorities (for patch archives etc.).
 */
class MpqPriorityList
{
	public:
		typedef MpqPriorityList self;
		
		typedef boost::shared_ptr<MpqPriorityListEntry> Source;
		typedef boost::multi_index_container<Source,
		boost::multi_index::indexed_by<
		// simple list
		boost::multi_index::sequenced<>,
		// ordered by itself
		boost::multi_index::ordered_unique<boost::multi_index::tag<MpqPriorityListEntry>, boost::multi_index::identity<MpqPriorityListEntry> >,
		// ordered by its corresponding priority
		boost::multi_index::ordered_non_unique<boost::multi_index::tag<MpqPriorityListEntry::Priority>, boost::multi_index::const_mem_fun<MpqPriorityListEntry, MpqPriorityListEntry::Priority, &MpqPriorityListEntry::priority> >,
		// ordered by its corresponding URL
		boost::multi_index::ordered_non_unique<boost::multi_index::tag<KUrl>, boost::multi_index::const_mem_fun<MpqPriorityListEntry, const KUrl&, &MpqPriorityListEntry::url> >

		> >

		Sources;
		
		typedef boost::shared_ptr<Resource> ResourcePtr;
		typedef std::map<KUrl, ResourcePtr> Resources;
		typedef boost::shared_ptr<Texture> TexturePtr;
		typedef std::map<BOOST_SCOPED_ENUM(TeamColor), TexturePtr> TeamColorTextures;

		void setLocale(mpq::MpqFile::Locale locale);
		mpq::MpqFile::Locale locale() const;

		/**
		 * Adds a source with URL \p url and priorioty \p priority to the priority list.
		 * Each URL should be unique and refer to an archive (mostly MPQ) or directory.
		 * \param priority 0 is default value. Priorities needn't to be unique.
		 * \return Returns true if the URL has been added to the list (this doesn't happen if there already is a source with the given URL or if it refers to an absolute file path which is no archive or directory).
		 * \todo Improve archive detection.
		 */
		virtual bool addSource(const KUrl &url, MpqPriorityListEntry::Priority priority = 0);
		/**
		 * Removes a source by its corresponding URL.
		 * \return Returns true if URL corresponds to some source and that source has been removed properly.
		 */
		virtual bool removeSource(const KUrl &url);

		/**
		 * \copydoc KIO::NetAccess::download()
		 * Considers all entries if it's an relative URL. Otherwise it will at least consider the priority list's locale if none is given.
		 * \todo If it's an "mpq:/" URL and there is no locale given add one considering \ref locale().
		 */
		virtual bool download(const KUrl &src, QString &target, QWidget *window);
		virtual bool upload(const QString &src, const KUrl &target, QWidget *window);
		
		const Sources& sources() const;
		
		/**
		 * All added resources will also be added to MPQ priority list automaticially.
		 * Therefore there shouldn't occur any problems when you open an external MDL file and need its textures which are contained by the same directory.
		 */
		virtual void addResource(class Resource *resource);
		/**
		 * Removes resource from editor.
		 * \note Deletes resource \p resource.
		 */
		virtual bool removeResource(class Resource *resource);
		virtual bool removeResource(const KUrl &url);
		const Resources& resources() const;
		
		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		const TexturePtr& teamColorTexture(BOOST_SCOPED_ENUM(TeamColor) teamColor) const throw (class Exception);
		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		const TexturePtr& teamGlowTexture(BOOST_SCOPED_ENUM(TeamColor) teamGlow) const throw (class Exception);

		
		/**
		 * Returns localized string under key \p key in group \p group.
		 * Call tr("WESTRING_APPNAME", "WorldEditStrings", \ref mpq::MpqFile::German) to get the text "WARCRAFT III - Welt-Editor" from file "UI/WorldEditStrings.txt" of MPQ archive "War3xlocal.mpq" (Frozen Throne), for instance.
		 * Localized keyed and grouped strings are found under following paths of current MPQ with the highest priority and corresponding locale \p locale:
		 * <ul>
		 * <li>UI/CampaignStrings.txt</li>
		 * <li>UI/TipStrings.txt</li>
		 * <li>UI/TriggerStrings.txt</li>
		 * <li>UI/WorldEditGameStrings.txt</li>
		 * <li>UI/TriggerStrings.txt</li>
		 * <li>UI/WorldEditStrings.txt</li>
		 * </ul>
		 */
		virtual QString tr(const QString &key, const QString &group = "", BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale = mpq::MpqFile::Locale::Neutral) const;
	protected:
		Sources& sources();
		
		mpq::MpqFile::Locale m_locale;
		
		Sources m_sources;
		Resources m_resources;
		// team color and glow textures
		mutable TeamColorTextures m_teamColorTextures;
		mutable TeamColorTextures m_teamGlowTextures;
};

inline void MpqPriorityList::setLocale(mpq::MpqFile::Locale locale)
{
	this->m_locale = locale;
}

inline mpq::MpqFile::Locale MpqPriorityList::locale() const
{
	return this->m_locale;
}

inline const MpqPriorityList::Sources& MpqPriorityList::sources() const
{
	return this->m_sources;
}

inline void MpqPriorityList::addResource(class Resource *resource)
{
	this->m_resources.insert(std::make_pair(resource->url(), resource));
	this->addSource(resource->url());
}


inline bool MpqPriorityList::removeResource(class Resource *resource)
{
	this->removeResource(resource->url()); // resource is deleted here
}

inline bool MpqPriorityList::removeResource(const KUrl &url)
{
	Resources::iterator iterator = this->m_resources.find(url);

	if (iterator == this->m_resources.end())
		return false;

	this->m_resources.erase(iterator);
	iterator->second.reset();
	this->removeSource(url);

	return true;
}

inline const MpqPriorityList::Resources& MpqPriorityList::resources() const
{
	return this->m_resources;
}

inline const MpqPriorityList::TexturePtr& MpqPriorityList::teamColorTexture(BOOST_SCOPED_ENUM(TeamColor) teamColor) const throw (class Exception)
{
	if (this->m_teamColorTextures[teamColor].get() == 0)
		this->m_teamColorTextures[teamColor].reset(new Texture(const_cast<MpqPriorityList*>(this), teamColorUrl(teamColor)));

	return this->m_teamColorTextures[teamColor];
}

inline const MpqPriorityList::TexturePtr& MpqPriorityList::teamGlowTexture(BOOST_SCOPED_ENUM(TeamColor) teamGlow) const throw (class Exception)
{
	if (this->m_teamGlowTextures[teamGlow].get() == 0)
		this->m_teamGlowTextures[teamGlow].reset(new Texture(const_cast<MpqPriorityList*>(this), teamGlowUrl(teamGlow)));

	return this->m_teamGlowTextures[teamGlow];
}

}

}

#endif
