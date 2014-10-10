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

#include <QMainWindow>
#include <QSettings>
#include <QLinkedList>

#include <kdemacros.h>
#include <KAboutData>
#include <KActionCollection>

#include "platform.hpp"
#include "mpqprioritylist.hpp"
#include "newmapdialog.hpp"
#include "root.hpp"

namespace wc3lib
{

namespace editor
{

class WarcraftIIIShared;
class Map;
class Module;
class SourcesDialog;

/**
 * \brief This class emulates the Warcraft III World Editor. It provides all necessary editor modules.
 *
 * Editor holds all resources and required MPQ archives since it inherits from \ref MpqPriorityList.
 * If a resource is required (by its URL) it could theoretically be cached if implemented in class Resource.
 * Just use \ref Editor#resources()[URL] to refer to your required resource.
 * @todo Each Module has it's own tool bar with all other modules.
 */
class KDE_EXPORT Editor : public QMainWindow, public MpqPriorityList
{
	Q_OBJECT

	signals:
		/**
		 * Is emitted when a new module has been added via \ref addModule().
		 */
		void createdModule(Module *module);
		/**
		 * This signal is being emitted when a map has been opened.
		 */
		void openedMap(Map *map);
		/**
		 * Since one Editor instance allows you to edit multiple maps this signal is emitted when the currently opened map is changed.
		 * This signal can be important to all editor's modules because they're oftenly showing map-related data
		 */
		void switchedToMap(Map *map);
		/**
		 * Is emitted when a map is going to be closed.
		 */
		void aboutToCloseMap(Map *map);
		/**
		 * Is emitted when a map has finally been closed.
		 * Therefore there is no Map parameter since the object has already been released.
		 * \p url URL of the corresponding map file.
		 */
		void closedMap(const KUrl &url);

	public:
		typedef Editor self;
		typedef QLinkedList<Module*> Modules;
		typedef QMap<Module*, QAction*> ModulesActions;
		/**
		 * \note Index is useful for corresponding action.
		 */
		typedef QList<Map*> Maps;
		typedef QMap<Map*, QAction*> MapActions;

		static const KAboutData& aboutData();
		static const KAboutData& wc3libAboutData();

		Editor(Root *root, QWidget *parent = 0, Qt::WindowFlags f = Qt::Window);
		virtual ~Editor();

		/**
		 * Template get element function for pointers which can be 0 and should be set/allocated when being get.
		 * \note Emits \ref createdModule() after module has been added.
		 */
		void addModule(Module *module);

		/**
		 * When being called the first time this functions tries to allocate the OGRE root instance.
		 * \note To load all renderer settings "ogre.cfg" and "plugins.cfg" have to be available in the working directory.
		 * \throw Exception Throws an exception if no available renderer was found.
		 * \todo Support "ogre.cfg" and "plugins.cfg" path options (for \ref ModelView as well).
		 */
		Root* root() const;
		Map* currentMap() const;

		/**
		 * Contains many default actions shared by all module tool bars like "Open Map" etc.
		 */
		KActionCollection* actionCollection() const;
		/**
		 * Contains actions of all registered modules of the editor.
		 * \ref addModule()
		 */
		KActionCollection* modulesActionCollection() const;
		const ModulesActions& modulesActions() const;
		KActionCollection* mapsActionCollection() const;
		const MapActions& mapActions() const;
		const Modules& modules() const;
		const Maps& maps() const;
		/**
		 * \note Allocated on request!
		 */
		NewMapDialog* newMapDialog() const;

	public slots:
		/**
		 * Shows corresponding \ref NewMapDialog.
		 * \sa newMapDialog()
		 */
		void newMap();
		/**
		 * Opens file dialog with filter for all supported map types (\ref mapFilter()).
		 * The created file dialog allows you to select multiple URLs and therefore to open several maps.
		 * The editor switches automatically to the last opened map.
		 */
		void openMap();
		void openMap(const KUrl &url, bool switchTo = true);
		/**
		 * Emits signal \ref switchedToMap() with \p map as parameter.
		 */
		void switchToMap(Map *map);
		void closeMap(Map *map);
		/**
		 * Closes current map.
		 * \sa currentMap()
		 */
		void closeMap();
		void saveMap();
		void saveMapAs();

		void showSourcesDialog();

	protected:
		virtual void changeEvent(QEvent *event);

		/// Enables or disables actions which can only be used with an opened map.
		void setMapActionsEnabled(bool enabled);

		ModulesActions& modulesActions();
		MapActions& mapActions();
		Modules& modules();
		Maps& maps();

		static KAboutData m_aboutData;
		static KAboutData m_wc3libAboutData;

		Root *m_root;
		Map *m_currentMap;

		KActionCollection *m_actionCollection;
		KActionCollection *m_modulesActionCollection;
		ModulesActions m_modulesActions;
		KActionCollection *m_mapsActionCollection;
		MapActions m_mapActions;
		Modules m_modules;
		Maps m_maps;
		NewMapDialog *m_newMapDialog;
		SourcesDialog *m_sourcesDialog;
};

inline Map* Editor::currentMap() const
{
	return m_currentMap;
}

inline KActionCollection* Editor::actionCollection() const
{
	return this->m_actionCollection;
}

inline KActionCollection* Editor::modulesActionCollection() const
{
	return this->m_modulesActionCollection;
}

inline const Editor::ModulesActions& Editor::modulesActions() const
{
	return this->m_modulesActions;
}

inline KActionCollection* Editor::mapsActionCollection() const
{
	return m_mapsActionCollection;
}

inline const Editor::MapActions& Editor::mapActions() const
{
	return m_mapActions;
}

inline const Editor::Modules& Editor::modules() const
{
	return this->m_modules;
}

inline const Editor::Maps& Editor::maps() const
{
	return m_maps;
}

inline NewMapDialog* Editor::newMapDialog() const
{
	if (this->m_newMapDialog == 0)
		const_cast<self*>(this)->m_newMapDialog = new NewMapDialog(const_cast<self*>(this), const_cast<self*>(this));

	return this->m_newMapDialog;
}

inline Editor::MapActions& Editor::mapActions()
{
	return m_mapActions;
}

inline Editor::ModulesActions& Editor::modulesActions()
{
	return m_modulesActions;
}

inline Editor::Modules& Editor::modules()
{
	return m_modules;
}

inline Editor::Maps& Editor::maps()
{
	return m_maps;
}

}

}

#endif
