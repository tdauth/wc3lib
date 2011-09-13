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

#include <map>

#include <boost/filesystem.hpp>

#include <QSettings>
#include <QLinkedList>

#include <KMainWindow>
#include <KAboutData>
#include <KUrl>
//#include <KMessageBox>

#include <Ogre.h>

#include "platform.hpp"
#include "mpqprioritylist.hpp"
#include "resource.hpp"
#include "terraineditor.hpp"
#include "triggereditor.hpp"
#include "soundeditor.hpp"
#include "objecteditor.hpp"
#include "campaigneditor.hpp"
#include "aieditor.hpp"
#include "objectmanager.hpp"
#include "importmanager.hpp"
#include "mpqeditor.hpp"
#include "modeleditor.hpp"
#include "textureeditor.hpp"
#include "newmapdialog.hpp"
#include "ogremdlx.hpp"
#include "texture.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * All modules are stored as pointers and created on request. Therefore their initial value is 0 and they're allocated when needed.
 * Editor holds all resources and required MPQ archives since it inherits from \ref MpqPriorityList.
 * If a resource is required (by its URL) it could theoretically be cached if implemented in class Resource.
 * Just use \ref Editor#resources()[URL] to refer to your required resource.
 * @todo Each Module has it's own tool bar with all other modules.
 */
class Editor : public KMainWindow, public MpqPriorityList
{
	Q_OBJECT

	signals:
		void createdModule(Module *module);
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

		Ogre::Root* root() const;
		class Map* currentMap() const;

		class KActionCollection* actionCollection() const;
		class TerrainEditor* terrainEditor() const;
		class TriggerEditor* triggerEditor() const;
		class SoundEditor* soundEditor() const;
		class ObjectEditor* objectEditor() const;
		class CampaignEditor* campaignEditor() const;
		class AiEditor* aiEditor() const;
		class ObjectManager* objectManager() const;
		class ImportManager* importManager() const;
		class MpqEditor* mpqEditor() const;
		class ModelEditor* modelEditor() const;
		class TextureEditor* textureEditor() const;
		class NewMapDialog* newMapDialog() const;

	public slots:
		void newMap();
		void openMap();
		void openMap(const KUrl &url);
		void switchToMap(class Map *map);
		void closeMap(class Map *map);
		void showTerrainEditor();
		void showTriggerEditor();
		void showSoundEditor();
		void showObjectEditor();
		void showCampaignEditor();
		void showAiEditor();
		void showObjectManager();
		void showImportManager();
		void showMpqEditor();
		void showModelEditor();
		void showTextureEditor();

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

		/**
		 * Template get element function for pointers which can be 0 and should be set/allocated when being get.
		 */
		template<class T>
		T* module(T* const &module) const;

		static KAboutData m_aboutData;
		static KAboutData m_wc3libAboutData;

		Ogre::Root *m_root;
		class Map *m_currentMap;

		class KActionCollection *m_actionCollection;
		Modules m_modules;
		class TerrainEditor *m_terrainEditor;
		class TriggerEditor *m_triggerEditor;
		class SoundEditor *m_soundEditor;
		class ObjectEditor *m_objectEditor;
		class CampaignEditor *m_campaignEditor;
		class AiEditor *m_aiEditor;
		class ObjectManager *m_objectManager;
		class ImportManager *m_importManager;
		class MpqEditor *m_mpqEditor; // new
		class ModelEditor *m_modelEditor; // new
		class TextureEditor *m_textureEditor; // new
		class NewMapDialog *m_newMapDialog;
};

template<class T>
T* Editor::module(T* const &module) const
{
	if (module == 0)
	{
		const_cast<T*&>(module) = new T(const_cast<self*>(this), const_cast<self*>(this));
		const_cast<self*>(this)->m_modules.append(const_cast<T*&>(module));
		emit const_cast<self*>(this)->createdModule(const_cast<T*&>(module));
	}

	return module;
}

inline Ogre::Root* Editor::root() const
{
	// setup a renderer
	if (m_root == 0)
	{
		const_cast<class Editor*>(this)->m_root = new Ogre::Root();

		const Ogre::RenderSystemList &renderers = m_root->getAvailableRenderers();
		assert(!renderers.empty()); // we need at least one renderer to do anything useful
		Ogre::RenderSystem *renderSystem = renderers.front();
		assert(renderSystem); // user might pass back a null renderer, which would be bad!
		// configuration is setup automatically by ogre.cfg file
		renderSystem->setConfigOption("Full Screen", "No");
		m_root->setRenderSystem(renderSystem);
		// initialize without creating window
		m_root->saveConfig();
		m_root->initialise(false); // don't create a window
	}
	//else if (m_root->getRenderSystem()->getConfigOptions()["Full Screen"].currentValue == "Yes")
		//KMessageBox::information(this, i18n("Full screen is enabled."));

	return m_root;
}

inline class Map* Editor::currentMap() const
{
	return m_currentMap;
}

inline class KActionCollection* Editor::actionCollection() const
{
	return this->m_actionCollection;
}

inline class TerrainEditor* Editor::terrainEditor() const
{
	return module(this->m_terrainEditor);
}

inline class TriggerEditor* Editor::triggerEditor() const
{
	return module(this->m_triggerEditor);
}

inline class SoundEditor* Editor::soundEditor() const
{
	return module(this->m_soundEditor);
}

inline class ObjectEditor* Editor::objectEditor() const
{
	return module(this->m_objectEditor);
}

inline class CampaignEditor* Editor::campaignEditor() const
{
	return module(this->m_campaignEditor);
}

inline class AiEditor* Editor::aiEditor() const
{
	return module(this->m_aiEditor);
}

inline class ObjectManager* Editor::objectManager() const
{
	return module(this->m_objectManager);
}

inline class ImportManager* Editor::importManager() const
{
	return module(this->m_importManager);
}

inline class MpqEditor* Editor::mpqEditor() const
{
	return module(this->m_mpqEditor);
}

inline class ModelEditor* Editor::modelEditor() const
{
	return module(this->m_modelEditor);
}

inline class TextureEditor* Editor::textureEditor() const
{
	return module(this->m_textureEditor);
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
