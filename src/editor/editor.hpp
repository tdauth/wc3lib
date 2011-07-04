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

#include <QDebug> // debug
#include <QSettings>

#include <KMainWindow>
#include <KAboutData>
#include <KUrl>
//#include <KMessageBox>

#include <Ogre.h>

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
#include "../mpq.hpp"

namespace wc3lib
{

namespace editor
{

/**
* All modules are stored as pointers and created on request. Therefore their initial value is 0 and they're allocated when needed.
* Editor holds all resources and required MPQ archives since it inherits from MpqPriorityList.
* If a resource is required (by its URL) it could theoretically be cached if implemented in class Resource.
* Just use \ref Editor#resources()[URL] to refer to your required resource.
* @todo Each Module has it's own tool bar with all other modules.
*/
class Editor : public KMainWindow, public MpqPriorityList
{
	Q_OBJECT

	signals:
		/**
		 * This signal is being emitted when a map has been opened.
		 */
		void openedMap(const class Map *map);
		/**
		 * Since one Editor instance allows you to edit multiple maps this signal is emitted when the currently opened map is changed.
		 * This signal can be important to all editor's modules because they're oftenly showing map-related data
		 */
		void switchedToMap(const class Map *map);
		/**
		 * Is emitted when a map is going to be closed.
		 */
		void aboutToCloseMap(const class Map *map);
		/**
		 * Is emitted when a map has finally been closed.
		 * Therefore there is no Map parameter since the object has already been released.
		 * \p url URL of the corresponding map file.
		 */
		void closedMap(const KUrl &url);

	public:
		typedef Editor self;
		typedef std::pair<KUrl, class Resource*> ResourceValueType;
		typedef boost::shared_ptr<Texture> TexturePtr;

		static const KAboutData& aboutData();
		static const KAboutData& wc3libAboutData();

		/**
		 * Required by textures which use replaceable id \ref mdlx::ReplaceableId::TeamColor.
		 * \sa mdlx::ReplaceableId, mdlx::Texture
		 */
		static KUrl teamColorUrl(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamColor);
		/**
		 * Required by textures which use replaceable id \ref mdlx::ReplaceableId::TeamGlow.
		 * \sa mdlx::ReplaceableId, mdlx::Texture
		 */
		static KUrl teamGlowUrl(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamGlow);
		
		/// \todo Get install value name!!!
		static KUrl installUrl();
		static KUrl installXUrl();
		static KUrl war3Url();
		static KUrl war3XUrl();
		static KUrl war3PatchUrl();
		static KUrl war3XLocalUrl();

		Editor(QWidget *parent = 0, Qt::WindowFlags f = Qt::Window);
		virtual ~Editor();
		
		Ogre::Root* root() const;

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

		/**
		 * All added resources will also be added to MPQ priority list automaticially.
		 * Therefore there shouldn't occur any problems when you open an external MDL file and need its textures which are contained by the same directory.
		 */
		void addResource(class Resource *resource);
		/**
		 * Removes resource from editor.
		 * \note Deletes resource \p resource.
		 */
		bool removeResource(class Resource *resource);
		bool removeResource(const KUrl &url);
		const std::map<KUrl, class Resource*>& resources() const;
		
		/**
		 * Once requested, the image is kept in memory until it's refreshed manually.
		 */
		const TexturePtr& teamColorTexture(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamColor) const throw (class Exception);
		/**
		* Once requested, the image is kept in memory until it's refreshed manually.
		*/
		const TexturePtr& teamGlowTexture(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamGlow) const throw (class Exception);

		
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
		QString tr(const QString &key, const QString &group = "", BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale = mpq::MpqFile::Locale::Neutral) const;
		
	public slots:
		void newMap();
		void openMap(const KUrl &url);
		void switchToMap(const class Map *map);
		void closeMap(const class Map *map);
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
		
		class KActionCollection *m_actionCollection;
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

		std::map<KUrl, class Resource*> m_resources;

		// team color and glow textures
		mutable std::map<BOOST_SCOPED_ENUM(OgreMdlx::TeamColor), TexturePtr> m_teamColorTextures;
		mutable std::map<BOOST_SCOPED_ENUM(OgreMdlx::TeamColor), TexturePtr> m_teamGlowTextures;
};

inline KUrl Editor::teamColorUrl(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamColor)
{
	QString number = QString::number((int)teamColor);
	
	if (number.size() == 1)
		number.prepend('0');

	return KUrl("ReplaceableTextures/TeamColor/TeamColor" + number + ".blp");
}

inline KUrl Editor::teamGlowUrl(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamGlow)
{
	QString number = QString::number((int)teamGlow);

	if (number.size() == 1)
		number.prepend('0');

	return KUrl("ReplaceableTextures/TeamGlow/TeamGlow" + number + ".blp");
}

inline KUrl Editor::installUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl());
}

inline KUrl Editor::installXUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("InstallPathX");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl());
}

inline KUrl Editor::war3Url()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl()) + KUrl("war3.mpq");
}

inline KUrl Editor::war3XUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("InstallPathX");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl()) + KUrl("war3x.mpq");
}

inline KUrl Editor::war3PatchUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl()) + KUrl("War3Patch.mpq");
}

inline KUrl Editor::war3XLocalUrl()
{
	QSettings settings("Blizzard Entertainment", "Warcraft III");
	settings.beginGroup("Install Path");
	
	if (!settings.contains("???"))
		return KUrl();
	
	return KUrl(settings.value("???").toUrl()) + KUrl("War3xlocal.mpq");
}

template<class T>
T* Editor::module(T* const &module) const
{
	if (module == 0)
		const_cast<T*&>(module) = new T(const_cast<self*>(this));

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
	return module(this->m_newMapDialog);
}

inline void Editor::addResource(class Resource *resource)
{
	this->m_resources.insert(std::make_pair(resource->url(), resource));
	this->addEntry(resource->url());
	qDebug() << "Added resource " << resource->url();
}


inline bool Editor::removeResource(class Resource *resource)
{
	this->removeResource(resource->url()); // resource is deleted here
}

inline bool Editor::removeResource(const KUrl &url)
{
	std::map<KUrl, class Resource*>::iterator iterator = this->m_resources.find(url);

	if (iterator == this->m_resources.end())
		return false;

	qDebug() << "Removed resource " << url.path();
	this->m_resources.erase(iterator);
	delete iterator->second;
	this->removeEntry(url);

	return true;
}

inline const std::map<KUrl, class Resource*>& Editor::resources() const
{
	return this->m_resources;
}

inline const Editor::TexturePtr& Editor::teamColorTexture(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamColor) const throw (class Exception)
{
	if (this->m_teamColorTextures[teamColor].get() == 0)
		this->m_teamColorTextures[teamColor].reset(new Texture(teamColorUrl(teamColor)));

	return this->m_teamColorTextures[teamColor];
}

inline const Editor::TexturePtr& Editor::teamGlowTexture(BOOST_SCOPED_ENUM(OgreMdlx::TeamColor) teamGlow) const throw (class Exception)
{
	if (this->m_teamGlowTextures[teamGlow].get() == 0)
		this->m_teamGlowTextures[teamGlow].reset(new Texture(teamGlowUrl(teamGlow)));

	return this->m_teamGlowTextures[teamGlow];
}

}

}

#endif
