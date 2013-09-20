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

#include <boost/ptr_container/ptr_map.hpp>

#include <QFileInfo>

#include <KUrl>

#include "platform.hpp"
#include "../mpq.hpp"
#include "../map.hpp"
#include "resource.hpp"
#include "texture.hpp"
#include "ogremdlx.hpp"
#include "kio_mpq/mpqprotocol.hpp"

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
		 * \param url Has to refer to an archive or a directory.
		 */
		MpqPriorityListEntry(const KUrl &url, Priority priority);

		bool download(const KUrl &src, QString &target, QWidget *window);
		bool upload(const QString &src, const KUrl &target, QWidget *window);
		bool mkdir(const KUrl &target, QWidget *window);

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
 * Each priority list stores sources (\ref MpqPriorityListEntry) and resources (\ref Resource).
 * Sources are archives, directories etc. where required files are stored.
 * Resources are those files which are required and do have any specific meaning for the application like \ref Map, \ref Texture and all other classes which are derived from \ref Resource.
 * Sources can be accessed by the member function \ref sources() and modified by member functions \ref addSource() and \ref removeSource().
 * \ref Sources is a \ref boost::multi_index based container which stores all sources in various ways to increase the performance on accessing them.
 * \ref addDefaultSources() tries to add all default installed Warcraft III MPQ archives as possible sources including their proper order and priorities.
 * Resources can be accessed by the member function \ref resources() and modified by member functions \ref addResource() and \ref removeResource().
 * \ref Resources is a simple \ref std::map which uses the resource's URL as key.
 * Once you've added your required sources you can use \ref download() and \ref upload() to get and put your resources' files anywhere you want.
 * These functions consider all added sources in proper order and will try to modify your given URL until they've found your file/your target where to upload or return false.
 * By using \ref KIO::NetAccess for file I/O operations you can use all protocols which are provided by your own K Desktop Environment.
 * \sa MpqPriorityListEntry
 * \sa Resource
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
		// ordered by itself (its priority)
		boost::multi_index::ordered_non_unique<boost::multi_index::tag<MpqPriorityListEntry>, boost::multi_index::identity<MpqPriorityListEntry> >,
		// ordered by its corresponding URL
		boost::multi_index::ordered_non_unique<boost::multi_index::tag<KUrl>, boost::multi_index::const_mem_fun<MpqPriorityListEntry, const KUrl&, &MpqPriorityListEntry::url> >

		> >

		Sources;

		typedef std::map<KUrl, Resource*> Resources;
		typedef boost::ptr_map<BOOST_SCOPED_ENUM(TeamColor), Texture> TeamColorTextures;
		typedef boost::scoped_ptr<map::Txt> WorldEditorStringsPtr;
		typedef boost::scoped_ptr<map::TriggerData> TriggerDataPtr;
		typedef boost::scoped_ptr<map::TriggerStrings> TriggerStringsPtr;

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
		 *
		 * \section defaultMPQs Default MPQ archives:
		 * These are the priority values for Warcraft's default MPQ archives:
		 * <ul>
		 * <li>war3.mpq - 20</li>
		 * <li>War3Patch.mpq - 22<li>
		 * <li>war3x.mpq - 21</li>
		 * <li>War3xlocal.mpq - 21</li>
		 * </ul>
		 */
		virtual bool addWar3Source();
		/**
		 * \ref defaultMPQs
		 */
		virtual bool addWar3PatchSource();
		/**
		 * \ref defaultMPQs
		 */
		virtual bool addWar3XSource();
		/**
		 * \ref defaultMPQs
		 */
		virtual bool addWar3XLocalSource();
		/**
		 * Adds all default MPQ archives of Warcraft III as possible sources to the list.
		 * \ref defaultMPQs
		 */
		virtual bool addDefaultSources();
		/**
		 * Removes a source by its corresponding URL.
		 * \return Returns true if URL corresponds to some source and that source has been removed properly.
		 */
		virtual bool removeSource(const KUrl &url);
		virtual bool removeWar3Source();
		virtual bool removeWar3PatchSource();
		virtual bool removeWar3XSource();
		virtual bool removeWar3XLocalSource();
		virtual bool removeDefaultSources();
		
		virtual void refreshDefaultFiles(QWidget *window);

		/**
		 * \copydoc KIO::NetAccess::download()
		 * Considers all entries if it's an relative URL. Otherwise it will at least consider the priority list's locale if none is given.
		 * \note If \p src is an absolute URL it will work just like \ref KIO::NetAccess::download(). Therefore this member function should be a replacement whenever you want to download something to make relative URLs to custom sources available in your application.
		 * \todo If it's an "mpq:/" URL and there is no locale given add one considering \ref locale().
		 */
		virtual bool download(const KUrl &src, QString &target, QWidget *window);
		virtual bool upload(const QString &src, const KUrl &target, QWidget *window);
		virtual bool mkdir(const KUrl &target, QWidget *window);

		const Sources& sources() const;

		/**
		 * All added resources will also be added to MPQ priority list automaticially.
		 * Therefore there shouldn't occur any problems when you open an external MDL file and need its textures which are contained by the same directory.
		 */
		virtual void addResource(class Resource *resource);
		/**
		 * Removes resource from editor. This function is called by \ref Resource::~Resource() autmatically.
		 */
		virtual bool removeResource(class Resource *resource);
		virtual bool removeResource(const KUrl &url);
		const Resources& resources() const;

		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		Texture* teamColorTexture(BOOST_SCOPED_ENUM(TeamColor) teamColor) const throw (class Exception);
		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		Texture* teamGlowTexture(BOOST_SCOPED_ENUM(TeamColor) teamGlow) const throw (class Exception);
		
		
		/**
		 * \param window Widget which is used for KIO download.
		 */
		void refreshWorldEditorStrings(QWidget *window, const KUrl &url = KUrl("UI/WorldEditStrings.txt")) throw (Exception);
		/**
		 * World Editor strings are shared between maps usually.
		 * \note Call \ref refreshWorldEditorStrings() before using world editor strings.
		 */
		const WorldEditorStringsPtr& worldEditorStrings() const;
		
		/**
		 * \param window Widget which is used for KIO download.
		 * \sa triggerData()
		 */
		void refreshTriggerData(QWidget *window, const KUrl &url = KUrl("UI/TriggerData.txt")) throw (Exception);
		/**
		 * Trigger data which is shared between maps usually.
		 * \note Call \ref refreshTriggerData() before using trigger data.
		 */
		const TriggerDataPtr& triggerData() const;
		
		/**
		 * \param window Widget which is used for KIO download.
		 * \sa triggerStrings()
		 */
		void refreshTriggerStrings(QWidget *window, const KUrl &url = KUrl("UI/TriggerStrings.txt")) throw (Exception);
		/**
		 * Trigger strings which are shared between maps usually.
		 * \note Call \ref refreshTriggerStrings() before using trigger strings.
		 */
		const TriggerStringsPtr& triggerStrings() const;

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
		 * \param defaultValue If corresponding key entry could not be found (e. g. files are not available or it simply does not exist) this value is shown as string if its length is bigger than 0.
		 */
		virtual QString tr(QWidget *widget, const QString &key, const QString &group = "", BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale = mpq::MpqFile::Locale::Neutral, const QString &defaultValue = "") const;

		void readSettings(const QString &group);
		void writeSettings(const QString &group);

	protected:
		Sources& sources();

		mpq::MpqFile::Locale m_locale;

		Sources m_sources;
		Resources m_resources;
		// team color and glow textures
		mutable TeamColorTextures m_teamColorTextures;
		mutable TeamColorTextures m_teamGlowTextures;
		mutable WorldEditorStringsPtr m_worldEditorStrings;
		mutable TriggerDataPtr m_triggerData;
		mutable TriggerStringsPtr m_triggerStrings;
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
	this->addSource(resource->sourceUrl());
}


inline bool MpqPriorityList::removeResource(class Resource *resource)
{
	return this->removeResource(resource->url());
}

inline bool MpqPriorityList::removeResource(const KUrl &url)
{
	Resources::iterator iterator = this->m_resources.find(url);

	if (iterator == this->m_resources.end())
		return false;

	const KUrl sourceUrl = iterator->second->sourceUrl();
	this->removeSource(sourceUrl);
	this->m_resources.erase(iterator);

	return true;
}

inline const MpqPriorityList::Resources& MpqPriorityList::resources() const
{
	return this->m_resources;
}

inline Texture* MpqPriorityList::teamColorTexture(BOOST_SCOPED_ENUM(TeamColor) teamColor) const throw (class Exception)
{
	TeamColorTextures::iterator iterator = this->m_teamColorTextures.find(teamColor);

	if (iterator == this->m_teamColorTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamColorUrl(teamColor)));
		texture->setSource(const_cast<self*>(this));
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamColorTextures.insert(teamColor, texture);

		return result;
	}

	return this->m_teamColorTextures.find(teamColor)->second;
}

inline Texture* MpqPriorityList::teamGlowTexture(BOOST_SCOPED_ENUM(TeamColor) teamGlow) const throw (class Exception)
{
	TeamColorTextures::iterator iterator = this->m_teamGlowTextures.find(teamGlow);

	if (iterator == this->m_teamGlowTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamGlowUrl(teamGlow)));
		texture->setSource(const_cast<self*>(this));
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamGlowTextures.insert(teamGlow, texture);

		return result;
	}

	return this->m_teamGlowTextures.find(teamGlow)->second;
}

inline void MpqPriorityList::refreshWorldEditorStrings(QWidget *window, const KUrl &url) throw (Exception)
{
	QString target;

	if (!this->download(url, target, window))
		throw Exception(boost::format(_("Unable to download file \"%1%\".")) % url.toLocalFile().toUtf8().constData());

	qDebug() << "World Editor strings target: " << target;
	WorldEditorStringsPtr ptr(new map::Txt());
	ifstream ifstream(target.toUtf8().constData(), std::ios::binary | std::ios::in);

	if (!ifstream)
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toUtf8().constData());

	ptr->read(ifstream);
	m_worldEditorStrings.swap(ptr); // exception safe
}

inline const MpqPriorityList::WorldEditorStringsPtr& MpqPriorityList::worldEditorStrings() const
{
	return m_worldEditorStrings;
}

inline void MpqPriorityList::refreshTriggerData(QWidget *window, const KUrl &url) throw (Exception)
{
	QString target;

	if (!this->download(url, target, window))
		throw Exception(_("Unable to download file \"UI/TriggerData.txt\"."));

	qDebug() << "Trigger data target: " << target;
	TriggerDataPtr ptr(new map::TriggerData());
	ifstream ifstream(target.toUtf8().constData(), std::ios::binary | std::ios::in);

	if (!ifstream)
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toUtf8().constData());

	ptr->read(ifstream);
	m_triggerData.swap(ptr); // exception safe
}

inline const MpqPriorityList::TriggerDataPtr& MpqPriorityList::triggerData() const
{
	return m_triggerData;
}

inline void MpqPriorityList::refreshTriggerStrings(QWidget *window, const KUrl &url) throw (Exception)
{
	QString target;

	if (!this->download(url, target, window))
		throw Exception(_("Unable to download file \"UI/TriggerStrings.txt\"."));

	qDebug() << "Trigger strings target: " << target;
	TriggerStringsPtr ptr(new map::TriggerStrings());
	ifstream ifstream(target.toUtf8().constData(), std::ios::binary | std::ios::in);

	if (!ifstream)
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toUtf8().constData());

	ptr->read(ifstream);
	m_triggerStrings.swap(ptr); // exception safe
}

inline const MpqPriorityList::TriggerStringsPtr& MpqPriorityList::triggerStrings() const
{
	return m_triggerStrings;
}

}

}

#endif
