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

#ifndef WC3LIB_EDITOR_MODELVIEW_HPP
#define WC3LIB_EDITOR_MODELVIEW_HPP

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
 * Model views are usual widgets which can render 3d graphics.
 * They are required to provide a simple display for MDLX files.
 * Therefore well known rendering engine OGRE is used in this class.
 * The rendering viewport should be scaled correctly automatically since Qt GUI events are implemented.
 * MDLX files can be converted into OGRE entities by creating an \ref OgreMdlx instance which manages an OGRE mesh and sub mesh instance.
 * <a href="http://qt-apps.org/content/show.php/QtOgre+Framework?content=92912">source 1</a>, <a href="http://www.ogre3d.org/tikiwiki/QtOgre">source 2</a>
 * \sa mdlx::Mdlx, OgreMdlx
 */
class ModelView : public QWidget
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
		ModelView(Root *root, QWidget *parent = 0, Qt::WindowFlags f = 0, Ogre::SceneType ogreSceneType = Ogre::ST_EXTERIOR_CLOSE, const Ogre::NameValuePairList *ogreParameters = 0);
		virtual ~ModelView();

		//virtual void show();
		// test actions for one single view port/camera
		/**
		 * Centers view of the current view port camera (\ref camera()).
		 */
		void centerView();
		void setPolygonModePoints();
		void setPolygonModeWireframe();
		void setPolygonModeSolid();

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
		 * As there is no continuous rendering loop (\ref Ogre::Root::startRendering) this member function has to be called each time the rendering should be refreshed.
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

		// key events
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void keyReleaseEvent(QKeyEvent *event);
		virtual void wheelEvent(QWheelEvent *event);

		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);

		/**
		 * If there is no render window this member function initializes one which can be used by member function \ref renderWindow().
		 * If render window is not equal to 0 nothing happens when calling this function.
		 */
		virtual void initRenderWindow();

		bool checkCameraMovementBounds(const Ogre::Vector3 &delta);

		const Ogre::NameValuePairList *m_parameters;
		Root *m_root;
		Ogre::RenderWindow *m_renderWindow;

		// default scene
		Ogre::SceneType m_sceneType;
		Ogre::SceneManager *m_sceneManager;
		Ogre::Camera *m_camera;
		Ogre::Viewport *m_viewPort;

		//Ogre::ExampleFrameListener *m_frameListener;

		// event values
		bool m_changeFarClip;
		bool m_enableMouseMovement;
		bool m_enableMouseRotation;
		float m_rotateSpeed; // speed for rotating with mouse
		float m_moveSpeed; // speed for moving with mouse
		float m_scrollSpeed; // speed for changing far clip and scrolling distance

		QPoint m_mousePoint; // mouse movement orientation point (last mouse position)
		Ogre::Real m_yawValue;
		Ogre::Real m_pitchValue;
};

inline Root* ModelView::root() const
{
	return this->m_root;
}

inline Ogre::RenderWindow* ModelView::renderWindow() const
{
	return this->m_renderWindow;
}

inline Ogre::SceneManager* ModelView::sceneManager() const
{
	return this->m_sceneManager;
}

inline void ModelView::setCamera(Ogre::Camera *camera)
{
	this->m_camera = camera;
	this->m_viewPort->setCamera(camera);
	this->render();
}

inline Ogre::Camera* ModelView::camera() const
{
	return this->m_camera;
}

inline Ogre::Viewport* ModelView::viewPort() const
{
	return this->m_viewPort;
}

inline Ogre::String ModelView::name() const
{
	std::basic_ostringstream<Ogre::String::value_type> sstream;
	sstream << "ModelViewWindow";
	sstream << this;

	return sstream.str();
}

inline QSize ModelView::sizeHint() const
{
	return QSize(640, 480);
}

}

}

#endif
