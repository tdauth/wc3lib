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

#include <QObject>
#include <QSettings>
#include <QLinkedList>

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
 * \todo Each Module has it's own tool bar with all other modules.
 */
class Editor : public QObject, public MpqPriorityList
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
		void closedMap(const QUrl &url);

	public:
		typedef Editor self;
		/**
		 * \brief A list of all modules hold by the editor.
		 */
		typedef QLinkedList<Module*> Modules;
		/**
		 * \brief A list of actions belonging each to one single module of the editor.
		 * The action activates the corresponding module.
		 */
		typedef QMap<Module*, QAction*> ModulesActions;
		/**
		 * \note Index is useful for corresponding action.
		 */
		typedef QList<Map*> Maps;
		typedef QMap<Map*, QAction*> MapActions;

		Editor(Root *root, QObject *parent = 0);
		virtual ~Editor();

		/**
		 * Template get element function for pointers which can be 0 and should be set/allocated when being get.
		 * \note Emits \ref createdModule() after module has been added.
		 */
		void addModule(Module *module);

		/**
		 * Similar to \ref Module::configure().
		 *
		 * Pops up a source dialog (\ref SourceDialog) if necessary and no source has been configured and tries to load the default sharad data files.
		 *
		 * \return Returns true if everything has been configured and loaded successfully.
		 */
		virtual bool configure(QWidget *parent) override;

		/**
		 * When being called the first time this functions tries to allocate the OGRE root instance.
		 * \note To load all renderer settings "ogre.cfg" and "plugins.cfg" have to be available in the working directory.
		 * \throw Exception Throws an exception if no available renderer was found.
		 * \todo Support "ogre.cfg" and "plugins.cfg" path options (for \ref ModelView as well).
		 */
		Root* root() const;
		Map* currentMap() const;

		const ModulesActions& modulesActions() const;
		const MapActions& mapActions() const;
		const Modules& modules() const;
		const Maps& maps() const;
		/**
		 * \note Allocated on request!
		 */
		NewMapDialog* newMapDialog(QWidget *parent = 0) const;

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
		void openMap(QWidget *window = 0);
		void openMap(const QUrl &url, bool switchTo = true, QWidget *window = 0);
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
		void saveMapAs(QWidget *window = 0);

		void showSourcesDialog(QWidget *window = 0);

	protected:
		virtual void changeEvent(QEvent *event);

		/// Enables or disables actions which can only be used with an opened map.
		void setMapActionsEnabled(bool enabled);

		ModulesActions& modulesActions();
		MapActions& mapActions();
		Modules& modules();
		Maps& maps();

		Root *m_root;
		Map *m_currentMap;

		ModulesActions m_modulesActions;
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

inline const Editor::ModulesActions& Editor::modulesActions() const
{
	return this->m_modulesActions;
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

inline NewMapDialog* Editor::newMapDialog(QWidget *parent) const
{
	if (this->m_newMapDialog == 0)
	{
		const_cast<self*>(this)->m_newMapDialog = new NewMapDialog(const_cast<self*>(this), parent);
	}

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
