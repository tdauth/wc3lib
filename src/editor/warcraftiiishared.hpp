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

#ifndef WC3LIB_EDITOR_WARCRAFTIIISHARED_HPP
#define WC3LIB_EDITOR_WARCRAFTIIISHARED_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include <QWidget>
#include <QHash>
#include <QIcon>

#include <kdemacros.h>
#include <KUrl>

#include "platform.hpp"
#include "texture.hpp"
#include "metadata.hpp"
#include "sharedobjectdata.hpp"
#include "../map/txt.hpp"
#include "../map/triggerdata.hpp"
#include "../map/triggerstrings.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

/**
 * \brief Provides shared data such as standard textures, TXT and SLK files which are used by Warcraft III.
 *
 * Team colors can be accessed via \ref teamColorTexture().
 *
 * Team glows can be accessed via \ref teamGlowTexture().
 *
 * For any other icons \ref icon() can be used to load it.
 * For specific World Editor icons \ref worldEditDataIcon() might also be useful.
 *
 * For strings it does provide the method \ref tr() as well which searches for translations of strings
 * in the standard files.
 * Before calling the method you need to load the string files using \ref refreshWorldEditorStrings() for example.
 *
 * For trigger data and strings \ref triggerData() and \ref triggerStrings() exist which have to be loaded first
 * using \ref refreshTriggerData() and \ref refreshTriggerStrings().
 *
 * Instead of refreshing all default files manually you can use \ref refreshDefaultFiles() which does the same job.
 */
class KDE_EXPORT WarcraftIIIShared
{
	public:
		/**
		 * Shared icons are stored in a hash for fast access.
		 * Store pixmaps and do not create icons with file names since then the files are loaded on demand.
		 * Store the pixmaps in memory which should be much faster.
		 */
		typedef QHash<KUrl, QPixmap> Icons;

		/**
		 * Team colors are stored similar to \ref Icons in a map using their team color value from \ref TeamColor
		 * as key.
		 */
		typedef boost::ptr_map<TeamColor, Texture> TeamColorTextures;

		/**
		 * The strings of the current map or campaign.
		 */
		typedef boost::scoped_ptr<MetaData> MapStringsPtr;
		/**
		 * The World Editor strings are accessed via \ref wc3lib::editor::MetaData which allows fast access at runtime to all strings.
		 */
		typedef boost::scoped_ptr<MetaData> WorldEditorStringsPtr;
		typedef boost::scoped_ptr<MetaData> WorldEditorGameStringsPtr;
		typedef boost::scoped_ptr<MetaData> WorldEditDataPtr;
		typedef boost::scoped_ptr<map::TriggerData> TriggerDataPtr;
		typedef boost::scoped_ptr<map::TriggerStrings> TriggerStringsPtr;

		typedef boost::scoped_ptr<SharedObjectData> SharedObjectDataPtr;

		WarcraftIIIShared(MpqPriorityList *source);

		MpqPriorityList* source() const;

		/**
		 * Refreshes all shared files automatically.
		 *
		 * \throws Exception Throws an exception if refreshing fails for one file.
		 */
		void refreshDefaultFiles(QWidget *window);

		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		Texture* teamColorTexture(TeamColor teamColor) const;
		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		Texture* teamGlowTexture(TeamColor teamGlow) const;

		void refreshMapStrings(QWidget *window, const KUrl url = KUrl("war3map.wts"));
		const MapStringsPtr& mapStrings() const;

		/**
		 * \param window Widget which is used for KIO download.
		 * \param url The URL of the World Editor strings file.
		 */
		void refreshWorldEditorStrings(QWidget *window, const KUrl &url = KUrl("UI/WorldEditStrings.txt"));
		/**
		 * World Editor strings are shared between maps usually.
		 * \note Call \ref refreshWorldEditorStrings() before using world editor strings.
		 */
		const WorldEditorStringsPtr& worldEditorStrings() const;

		/**
		 * \param window Widget which is used for KIO download.
		 * \param url The URL of the World Editor game strings file.
		 */
		void refreshWorldEditorGameStrings(QWidget *window, const KUrl &url = KUrl("UI/WorldEditGameStrings.txt"));
		/**
		 * World Editor game strings are shared between maps usually.
		 * \note Call \ref refreshWorldEditorGameStrings() before using world editor game strings.
		 */
		const WorldEditorStringsPtr& worldEditorGameStrings() const;

		/**
		 * Returns localized string under key \p key in group \p group.
		 * Call tr("WESTRING_APPNAME", "WorldEditStrings") to get the text "WARCRAFT III - Welt-Editor" from file "UI/WorldEditStrings.txt" of MPQ archive "War3xlocal.mpq" (Frozen Throne), for instance.
		 * Localized keyed and grouped strings are found under following paths of current MPQ with the highest priority:
		 * <ul>
		 * <li>war3map.wts</li>
		 * <li>UI/WorldEditStrings.txt</li>
		 * <li>UI/CampaignStrings.txt</li>
		 * <li>UI/TipStrings.txt</li>
		 * <li>UI/TriggerStrings.txt</li>
		 * <li>UI/WorldEditGameStrings.txt</li>
		 * <li>UI/TriggerStrings.txt</li>
		 * </ul>
		 * They are checked in the listed order from top to bottom.
		 *
		 * \param key The key of the translated string in one of the TXT files in the the corresponding \p group.
		 * \param group They group in one of the TXT files which contains \p key.
		 * \param defaultValue If corresponding key entry could not be found (e. g. files are not available or it simply does not exist) this value is shown as string if its length is bigger than 0.
		 *
		 * \todo Check other files (other than UI/WorldEditStrings.txt).
		 */
		QString tr(const QString &key, const QString &group = "WorldEditStrings", const QString &defaultValue = "") const;

		/**
		 * Creates a shared pixmap object.
		 *
		 * \todo Implement caching mechanism which increases speed of loading pixmaps multiple times.
		 *
		 * \sa worldEditDataPixmap()
		 */
		QPixmap pixmap(const KUrl &url, QWidget *window);
		/**
		 * As for the World Editor most icons are defined in the file "UI/WorldEditData.txt" this member function
		 * allows you to directly retrieve an icon from an entry of that file.
		 *
		 * \sa refreshWorldEditData() WorldEditData() pixmap()
		 */
		QPixmap worldEditDataPixmap(const QString &key, const QString &group, QWidget *window);

		/**
		 * Creates a shared icon object.
		 *
		 * \todo Implement caching mechanism which increases speed of loading icons multiple times.
		 *
		 * \sa worldEditDataIcon()
		 */
		QIcon icon(const KUrl &url, QWidget *window);
		/**
		 * As for the World Editor most icons are defined in the file "UI/WorldEditData.txt" this member function
		 * allows you to directly retrieve an icon from an entry of that file.
		 *
		 * \sa refreshWorldEditData() WorldEditData() icon()
		 */
		QIcon worldEditDataIcon(const QString &key, const QString &group, QWidget *window);

		/**
		 * \return Returns all internally stored icons.
		 */
		const Icons& icons() const;

		void refreshWorldEditData(QWidget *window, const KUrl &url = KUrl("UI/WorldEditData.txt"));
		const WorldEditDataPtr& worldEditData() const;

		/**
		 * \param window Widget which is used for KIO download.
		 * \param url The URL of the trigger data file.
		 * \sa triggerData()
		 */
		void refreshTriggerData(QWidget *window, const KUrl &url = KUrl("UI/TriggerData.txt"));
		/**
		 * Trigger data which is shared between maps usually.
		 * \note Call \ref refreshTriggerData() before using trigger data.
		 */
		const TriggerDataPtr& triggerData() const;

		/**
		 * \param window Widget which is used for KIO download.
		 * \param url The URL of the trigger strings file.
		 * \sa triggerStrings()
		 */
		void refreshTriggerStrings(QWidget *window, const KUrl &url = KUrl("UI/TriggerStrings.txt"));
		/**
		 * Trigger strings which are shared between maps usually.
		 * \note Call \ref refreshTriggerStrings() before using trigger strings.
		 */
		const TriggerStringsPtr& triggerStrings() const;

		SharedObjectDataPtr& sharedObjectData();
		const SharedObjectDataPtr& sharedObjectData() const;

	private:
		MpqPriorityList *m_source;

		Icons m_icons;
		// team color and glow textures
		mutable TeamColorTextures m_teamColorTextures;
		mutable TeamColorTextures m_teamGlowTextures;
		MapStringsPtr m_mapStrings;
		WorldEditorStringsPtr m_worldEditorStrings;
		WorldEditorGameStringsPtr m_worldEditorGameStrings;
		WorldEditDataPtr m_worldEditData;
		TriggerDataPtr m_triggerData;
		TriggerStringsPtr m_triggerStrings;
		SharedObjectDataPtr m_sharedObjectData;
};

inline MpqPriorityList* WarcraftIIIShared::source() const
{
	return this->m_source;
}

inline const WarcraftIIIShared::MapStringsPtr& WarcraftIIIShared::mapStrings() const
{
	return this->m_mapStrings;
}

inline const WarcraftIIIShared::WorldEditorStringsPtr& WarcraftIIIShared::worldEditorStrings() const
{
	return this->m_worldEditorStrings;
}

inline const WarcraftIIIShared::Icons& WarcraftIIIShared::icons() const
{
	return this->m_icons;
}

inline const WarcraftIIIShared::WorldEditorStringsPtr& WarcraftIIIShared::worldEditorGameStrings() const
{
	return this->m_worldEditorGameStrings;
}

inline const WarcraftIIIShared::WorldEditDataPtr& WarcraftIIIShared::worldEditData() const
{
	return this->m_worldEditData;
}

inline const WarcraftIIIShared::TriggerDataPtr& WarcraftIIIShared::triggerData() const
{
	return m_triggerData;
}

inline const WarcraftIIIShared::TriggerStringsPtr& WarcraftIIIShared::triggerStrings() const
{
	return m_triggerStrings;
}

inline WarcraftIIIShared::SharedObjectDataPtr& WarcraftIIIShared::sharedObjectData()
{
	return this->m_sharedObjectData;
}

inline const WarcraftIIIShared::SharedObjectDataPtr& WarcraftIIIShared::sharedObjectData() const
{
	return this->m_sharedObjectData;
}

}

}

#endif
