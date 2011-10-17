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

#ifndef WC3LIB_EDITOR_EDITOR_HPP
#define WC3LIB_EDITOR_EDITOR_HPP

#include <QSettings>
#include <QLinkedList>

#include <KMainWindow>
#include <KAboutData>

#include "platform.hpp"
#include "mpqprioritylist.hpp"
#include "newmapdialog.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Editor holds all resources and required MPQ archives since it inherits from \ref MpqPriorityList.
 * If a resource is required (by its URL) it could theoretically be cached if implemented in class Resource.
 * Just use \ref Editor#resources()[URL] to refer to your required resource.
 * @todo Each Module has it's own tool bar with all other modules.
 */
class Editor : public KMainWindow, public MpqPriorityList
{
	Q_OBJECT

	signals:
		/**
		 * Is emitted when a new module has been added via \ref addModule().
		 * \param action Corresponding action in \ref modulesActionCollection() which focuses the module.
		 */
		void createdModule(class Module *module, class KAction *action);
		/**
		 * This signal is being emitted when a map has been opened.
		 */
		void openedMap(class Map *map);
		/**
		 * Since one Editor instance allows you to edit multiple maps this signal is emitted when the currently opened map is changed.
		 * This signal can be important to all editor's modules because they're oftenly showing map-related data
		 */
		void switchedToMap(class Map *map);
		/**
		 * Is emitted when a map is going to be closed.
		 */
		void aboutToCloseMap(class Map *map);
		/**
		 * Is emitted when a map has finally been closed.
		 * Therefore there is no Map parameter since the object has already been released.
		 * \p url URL of the corresponding map file.
		 */
		void closedMap(const KUrl &url);

	public:
		typedef Editor self;
		typedef QLinkedList<class Module*> Modules;
		typedef QLinkedList<class Map*> Maps;

		static const KAboutData& aboutData();
		static const KAboutData& wc3libAboutData();

		Editor(QWidget *parent = 0, Qt::WindowFlags f = Qt::Window);
		virtual ~Editor();

		/**
		 * Template get element function for pointers which can be 0 and should be set/allocated when being get.
		 * \note Emits \ref createdModule() after module has been added.
		 */
		void addModule(class Module *module);

		Ogre::Root* root() const;
		class Map* currentMap() const;

		class KActionCollection* actionCollection() const;
		const Modules& modules() const;
		class NewMapDialog* newMapDialog() const;

	public slots:
		void newMap();
		void openMap();
		void openMap(const KUrl &url);
		void switchToMap(class Map *map);
		void closeMap(class Map *map);

	protected:
		virtual void changeEvent(QEvent *event);

		/// Enables or disables actions which can only be used with an opened map.
		void setMapActionsEnabled(bool enabled);
		/**
		 * Reads settings from configuration which usually are stored on the user's filesystem and applies them on their corresponding editor modules.
		 * \note This does not necessarily use the same entries as the original World Editor created by Blizzard Entertainment.
		 */
		void readSettings();
		void writeSettings();

		static KAboutData m_aboutData;
		static KAboutData m_wc3libAboutData;

		Ogre::Root *m_root;
		class Map *m_currentMap;

		class KActionCollection *m_actionCollection;
		class KActionCollection *m_modulesActionCollection;
		Modules m_modules;
		class NewMapDialog *m_newMapDialog;
};

inline class Map* Editor::currentMap() const
{
	return m_currentMap;
}

inline class KActionCollection* Editor::actionCollection() const
{
	return this->m_actionCollection;
}

inline const Editor::Modules& Editor::modules() const
{
	return this->m_modules;
}

inline class NewMapDialog* Editor::newMapDialog() const
{
	if (this->m_newMapDialog == 0)
		const_cast<self*>(this)->m_newMapDialog = new NewMapDialog(const_cast<self*>(this), const_cast<self*>(this));

	return this->m_newMapDialog;
}

}

}

#endif
