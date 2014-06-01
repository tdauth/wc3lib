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

#include <KUrl>

#include "platform.hpp"
#include "texture.hpp"
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
 * For strings it does provide the method \ref tr() as well which searches for translations of strings
 * in the standard files.
 * Before calling the method you need to load the string files using \ref refreshWorldEditorStrings() for example.
 *
 * For trigger data and strings \ref triggerData() and \ref triggerStrings() exist which have to be loaded first
 * using \ref refreshTriggerData() and \ref refreshTriggerStrings().
 *
 * Instead of refreshing all default files manually you can use \ref refreshDefaultFiles() which does the same job.
 */
class WarcraftIIIShared
{
	public:
		typedef boost::ptr_map<TeamColor, Texture> TeamColorTextures;
		typedef boost::scoped_ptr<map::Txt> WorldEditorStringsPtr;
		typedef boost::scoped_ptr<map::TriggerData> TriggerDataPtr;
		typedef boost::scoped_ptr<map::TriggerStrings> TriggerStringsPtr;

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
		Texture* teamColorTexture(TeamColor teamColor) const throw (class Exception);
		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		Texture* teamGlowTexture(TeamColor teamGlow) const throw (class Exception);

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
		 * Returns localized string under key \p key in group \p group.
		 * Call tr("WESTRING_APPNAME", "WorldEditStrings") to get the text "WARCRAFT III - Welt-Editor" from file "UI/WorldEditStrings.txt" of MPQ archive "War3xlocal.mpq" (Frozen Throne), for instance.
		 * Localized keyed and grouped strings are found under following paths of current MPQ with the highest priority:
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
		QString tr(const QString &key, const QString &group = "", const QString &defaultValue = "") const;

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

	private:
		MpqPriorityList *m_source;

		// team color and glow textures
		mutable TeamColorTextures m_teamColorTextures;
		mutable TeamColorTextures m_teamGlowTextures;
		WorldEditorStringsPtr m_worldEditorStrings;
		TriggerDataPtr m_triggerData;
		TriggerStringsPtr m_triggerStrings;
};

inline MpqPriorityList* WarcraftIIIShared::source() const
{
	return this->m_source;
}

inline const WarcraftIIIShared::WorldEditorStringsPtr& WarcraftIIIShared::worldEditorStrings() const
{
	return m_worldEditorStrings;
}

inline const WarcraftIIIShared::TriggerDataPtr& WarcraftIIIShared::triggerData() const
{
	return m_triggerData;
}

inline const WarcraftIIIShared::TriggerStringsPtr& WarcraftIIIShared::triggerStrings() const
{
	return m_triggerStrings;
}

}

}

#endif
