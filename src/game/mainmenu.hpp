/***************************************************************************
 *   Copyright (C) 2022 by Tamino Dauth                                    *
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

#ifndef WC3LIB_GAME_MAINMENU_HPP
#define WC3LIB_GAME_MAINMENU_HPP

#include <sstream>

#include <QWidget>
#include <QString>

#include <Ogre.h>

#include "root.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * The main menu of Warcraft III which allows you to start games etc.
 */
class MainMenu : public QWidget
{
	Q_OBJECT

	signals:
		void onRendered();

	public:
		/**
		 * \param root The \ref Root instance of the OGRE 3D graphics system.
		 * \param ogreSceneType OGRE scene type which will be set for the scene manager of the widget. Should be changed for terrain (ST_EXTERIOR_FAR, ST_EXTERIOR_REAL_FAR).
		 * \param ogreParameters OGRE window parameters.
		 */
		MainMenu(Root *root, QWidget *parent = 0, Qt::WindowFlags f = 0, Ogre::SceneType ogreSceneType = Ogre::ST_EXTERIOR_CLOSE, const Ogre::NameValuePairList *ogreParameters = 0);
		virtual ~MainMenu();

		//virtual void show();
		// test actions for one single view port/camera
		/**
		 * Centers view of the current view port camera (\ref camera()).
		 */
		void centerView();

		Root* root() const;
		Ogre::RenderWindow* renderWindow() const;
		Ogre::SceneManager* sceneManager() const;
		void setCamera(Ogre::Camera *camera);
		Ogre::Camera* camera() const;
		Ogre::Viewport* viewPort() const;
		/**
		 * This member function is used when render window is created for assigning a unique name to it.
		 * \return Returns the unique name of the corresponding render window (\ref renderWindow()).
		 */
		virtual Ogre::String name() const;

	protected:
		friend class Settings;

		/**
		 * As there is no continuous rendering loop (Ogre::Root::startRendering()) this member function has to be called each time the rendering should be refreshed.
		 * If there is no created render window, \ref initRenderWindow is called automatically to create one.
		 */
		virtual void render();

		virtual QSize sizeHint() const;

		//virtual void paintEvent(QPaintEvent* event);
		virtual void showEvent(QShowEvent *event);
		virtual void resizeEvent(QResizeEvent *event);
		/**
		 * Paint event response calles \ref render() automatically.
		 */
		virtual void paintEvent(QPaintEvent *event);

		/**
		 * If there is no render window this member function initializes one which can be used by member function \ref renderWindow().
		 * If render window is not equal to 0 nothing happens when calling this function.
		 */
		virtual void initRenderWindow();

		const Ogre::NameValuePairList *m_parameters;
		Root *m_root;
		Ogre::RenderWindow *m_renderWindow;

		// default scene
		Ogre::SceneType m_sceneType;
		Ogre::SceneManager *m_sceneManager;
		Ogre::Camera *m_camera;
		Ogre::Viewport *m_viewPort;
};

inline Root* MainMenu::root() const
{
	return this->m_root;
}

inline Ogre::RenderWindow* MainMenu::renderWindow() const
{
	return this->m_renderWindow;
}

inline Ogre::SceneManager* MainMenu::sceneManager() const
{
	return this->m_sceneManager;
}

inline void MainMenu::setCamera(Ogre::Camera *camera)
{
	this->m_camera = camera;
	this->m_viewPort->setCamera(camera);
	this->render();
}

inline Ogre::Camera* MainMenu::camera() const
{
	return this->m_camera;
}

inline Ogre::Viewport* MainMenu::viewPort() const
{
	return this->m_viewPort;
}

inline Ogre::String MainMenu::name() const
{
	std::basic_ostringstream<Ogre::String::value_type> sstream;
	sstream << "MainMenuWindow";
	sstream << this;

	return sstream.str();
}

inline QSize MainMenu::sizeHint() const
{
	return QSize(640, 480);
}

}

}

#endif
