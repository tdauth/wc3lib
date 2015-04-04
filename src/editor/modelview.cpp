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

#include <QtCore>
#include <QtGui>

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

#include <KLocale>
#include <KMessageBox>

//#include <qogre/ExampleFrameListener.h>

#include "modelview.hpp"
#include "editor.hpp"

namespace wc3lib
{

namespace editor
{

ModelView::ModelView(Root *root, QWidget *parent, Qt::WFlags f, Ogre::SceneType ogreSceneType, const Ogre::NameValuePairList *ogreParameters)
: QWidget(parent, f)
, m_parameters(ogreParameters)
, m_root(root)
, m_renderWindow(0)
, m_sceneType(ogreSceneType)
, m_sceneManager(0)
, m_camera(0)
, m_viewPort(0)
, m_changeFarClip(false)
, m_enableMouseMovement(false)
, m_enableMouseRotation(false)
, m_rotateSpeed(1.0)
, m_moveSpeed(2.0)
, m_scrollSpeed(0.80)
, m_yawValue(0.0)
, m_pitchValue(0.0)
{
}

ModelView::~ModelView()
{
	//if (this->m_frameListener)
		//delete this->m_frameListener;

	if (this->sceneManager() != 0)
	{
		this->root()->destroySceneManager(this->sceneManager()); // destroys all entities automatically?
	}

	if (this->renderWindow() != 0)
	{
		this->root()->destroyRenderTarget(this->renderWindow());
	}
}

void ModelView::centerView()
{
	/*
	From OGRE source of OgreFrustum.cpp
	mFOVy(Radian(Math::PI/4.0f)),
        mFarDist(100000.0f),
        mNearDist(100.0f),
        */
	this->m_camera->setPosition(Ogre::Vector3::ZERO);
	this->m_camera->setFOVy(Ogre::Radian(Ogre::Math::PI/4.0f));
	//this->m_camera->lookAt(Ogre::Vector3(0, 0, 0));
	this->m_camera->setDirection(Ogre::Vector3::ZERO);
	this->m_camera->setNearClipDistance(100.0f);
	this->m_camera->setFarClipDistance(100000.0f);
	this->m_camera->setAspectRatio(static_cast<Ogre::Real>(this->m_viewPort->getActualWidth()) / static_cast<Ogre::Real>(this->m_viewPort->getActualHeight()));
	this->render();
}

void ModelView::setPolygonModePoints()
{
	this->m_camera->setPolygonMode(Ogre::PM_POINTS);
	this->render();
}

void ModelView::setPolygonModeWireframe()
{
	this->m_camera->setPolygonMode(Ogre::PM_WIREFRAME);
	this->render();
}

void ModelView::setPolygonModeSolid()
{
	this->m_camera->setPolygonMode(Ogre::PM_SOLID);
	this->render();
}

void ModelView::render()
{
	qDebug() << "Render";
	this->m_root->_fireFrameStarted();

	if (this->m_renderWindow == 0)
	{
		initRenderWindow();
	}
	else if (this->m_renderWindow->isActive())
	{
		this->m_renderWindow->update();
	}

	this->m_root->_fireFrameRenderingQueued();
	this->m_root->_fireFrameEnded();

	emit onRendered();
}

/// @todo Window initialization is too early? Wrong window id -> exception!!!
void ModelView::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event); // show first since we need window handle id
}

void ModelView::resizeEvent(QResizeEvent *event)
{
	if(this->m_renderWindow)
	{
		qDebug() << QString("Resize to %1|%2").arg(width()).arg(height());
		this->m_renderWindow->resize(width(), height());
		this->m_renderWindow->windowMovedOrResized();

		for(int ct = 0; ct < this->m_renderWindow->getNumViewports(); ++ct)
		{
			Ogre::Viewport* viewport = this->m_renderWindow->getViewport(ct);
			Ogre::Camera* camera = viewport->getCamera();
			camera->setAspectRatio(static_cast<Ogre::Real>(viewport->getActualWidth()) / static_cast<Ogre::Real>(viewport->getActualHeight()));
		}
	}

	QWidget::resizeEvent(event);
}

void ModelView::paintEvent(QPaintEvent *event)
{
	qDebug() << "PAINT!";
	this->render();
	QWidget::paintEvent(event);
}

void ModelView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Control:
			this->m_changeFarClip = true;

			break;
	}

	event->accept();
	QWidget::keyPressEvent(event);
}

void ModelView::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Control:
			this->m_changeFarClip = false;

			break;
	}

	event->accept();
	QWidget::keyReleaseEvent(event);
}

void ModelView::wheelEvent(QWheelEvent *event)
{
	if (renderWindow() != 0)
	{
		// Ctrl is pressed, change far clip distance
		if (this->m_changeFarClip)
		{
			Ogre::Real farClipDistance = this->m_camera->getFarClipDistance() + event->delta() * m_scrollSpeed;

			// enable infinite far clip distance if we can
			if (farClipDistance <= 0)
			{
				if (this->m_root->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
					farClipDistance = 0;
				else
					farClipDistance = 1;
			}
			// TODO maximum far clip?
			this->m_camera->setFarClipDistance(farClipDistance);
			qDebug() << "Changing far clip distance to: " << this->m_camera->getFarClipDistance();
		}
		// move camera
		else
		{
			const Ogre::Vector3 delta(0, 0, -event->delta() * m_scrollSpeed);

			if (checkCameraMovementBounds(delta))
			{
				this->m_camera->moveRelative(delta);
				//qDebug() << "Scroll camera with delta: " << delta.z;
			}
		}

		this->render();
		event->accept();
	}

	QWidget::wheelEvent(event);
}

void ModelView::mouseMoveEvent(QMouseEvent *event)
{
	if (renderWindow() != 0)
	{
		if (this->m_enableMouseMovement)
		{
			// moves camera along to x- and y-axis
			QPoint p(event->x(), event->y());
			p -= this->m_mousePoint;
			const Ogre::Vector3 delta(-p.x() * m_moveSpeed, p.y() * m_moveSpeed, 0);

			if (checkCameraMovementBounds(delta))
			{
				this->m_camera->moveRelative(delta);
				this->render();
				// refresh mouse point
				this->m_mousePoint = QPoint(event->x(), event->y());
				event->accept();
			}
		}

		if (this->m_enableMouseRotation)
		{
			// rotates camera around its current position
			//this->m_camera->roll();
			//qDebug() << "Rotating camera";
			//qDebug() << "X: " << event->x() << "| Y: " << event->y();
			//qDebug() << "Yaw: " << Ogre::Degree(-event->x() * m_rotateSpeed).valueDegrees();
			//qDebug() << "Pitch: " << Ogre::Degree(-event->y() * m_rotateSpeed).valueDegrees();
			/*
			this->m_camera->yaw(Ogre::Degree(-event->x() * m_rotateSpeed));
			this->m_camera->pitch(Ogre::Degree(-event->y() * m_rotateSpeed));
			*/
			QPoint p(event->x(), event->y());
			p -= this->m_mousePoint;

			//Ogre::Ray ray = this->m_camera->getCameraToViewportRay(p.x(), p.y());
			//ray.

			const Ogre::Real radius = this->m_camera->getPosition().length();
			const Ogre::Vector3 delta(0.0, 0.0, radius);

			if (checkCameraMovementBounds(delta))
			{
				this->m_camera->setPosition(this->m_camera->getDirection());
				//this->m_camera->setPosition(0.0, 0.0, 0.0); TODO DISABLE?
				this->m_camera->setOrientation(Ogre::Quaternion::IDENTITY);
				this->m_yawValue -= p.x() * m_rotateSpeed;
				this->m_pitchValue -= p.y() * m_rotateSpeed;
				this->m_camera->yaw(Ogre::Degree(this->m_yawValue)); // -
				this->m_camera->pitch(Ogre::Degree(this->m_pitchValue)); // -
				this->m_camera->moveRelative(Ogre::Vector3(0.0,0.0,radius));
				//this->m_camera->roll(Ogre::Degree(-event->x() * m_rotateSpeed));
				this->render();
				// refresh mouse point
				this->m_mousePoint = QPoint(event->x(), event->y());
				event->accept();
			}
		}
	}

	QWidget::mouseMoveEvent(event);
}

void ModelView::mousePressEvent(QMouseEvent *event)
{
	switch (event->button())
	{
		// selection
		// Only uses bounding boxes which should also be used by Warcraft (see http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Raycasting+to+the+polygon+level).
		case Qt::LeftButton:
		{
			if (renderWindow() != 0 && renderWindow()->isActive())
			{
				Ogre::Ray mouseRay = this->m_camera->getCameraToViewportRay(event->x() / float(this->width()), event->y() / float(this->height()));
				Ogre::RaySceneQuery *raySceneQuery = this->m_sceneManager->createRayQuery(mouseRay);
				raySceneQuery->setRay(mouseRay);
				raySceneQuery->setSortByDistance(true);
				Ogre::RaySceneQueryResult &result = raySceneQuery->execute();

				foreach(const Ogre::RaySceneQueryResultEntry &entry, result)
				{
					if (entry.movable)
						entry.movable->setVisible(!entry.movable->getVisible());
				}

				delete raySceneQuery;
				raySceneQuery = 0;

				this->render();
				event->accept();
			}

			break;
		}

		// When right button is being pressed mouse movement is possible.
		case Qt::RightButton:
			this->m_enableMouseMovement = true;
			this->m_mousePoint = this->mapFromGlobal(QCursor::pos());
			event->accept();

			break;

		// When left button is being pressed selection is possible (in editor only, not in view).
		//case Qt::LeftButton:

		// When middle button is being pressed camera rotation is possible.
		case Qt::MidButton:
			this->m_enableMouseRotation = true;
			this->m_mousePoint = this->mapFromGlobal(QCursor::pos());
			event->accept();

			break;
	}

	QWidget::mousePressEvent(event);
}

void ModelView::mouseReleaseEvent(QMouseEvent *event)
{
	switch (event->button())
	{
		// When right button is being pressed mouse movement is possible.
		case Qt::RightButton:
			this->m_enableMouseMovement = false;
			event->accept();

			break;

		// When left button is being pressed selection is possible (in editor only, not in view).
		//case Qt::LeftButton:

		// When middle button is being pressed camera rotation is possible.
		case Qt::MidButton:
			this->m_enableMouseRotation = false;
			event->accept();

			break;
	}

	QWidget::mouseReleaseEvent(event);
}

void ModelView::initRenderWindow()
{
	if (this->renderWindow() != 0)
	{
		return;
	}

	// Parameters to pass to Ogre::Root::createRenderWindow()
	Ogre::NameValuePairList params;

	// If the user passed in any parameters then be sure to copy them into our own parameter set.
	// NOTE: Many of the parameters the user can supply (left, top, border, etc) will be ignored
	// as they are overridden by Qt. Some are still useful (such as FSAA).
	if(this->m_parameters != 0)
	{
		params.insert(this->m_parameters->begin(), this->m_parameters->end());
	}

	// The external windows handle parameters are platform-specific
	Ogre::String externalWindowHandleParams;

	// Accept input focus
	setFocusPolicy(Qt::StrongFocus);

#if defined(Q_WS_WIN)
	// positive integer for W32 (HWND handle) - According to Ogre Docs
	externalWindowHandleParams = Ogre::StringConverter::toString((unsigned int)(winId()));
#endif

#if defined(Q_WS_X11)
	// poslong:posint:poslong:poslong (display*:screen:windowHandle:XVisualInfo*) for GLX - According to Ogre Docs
	const QX11Info &info = x11Info();
	externalWindowHandleParams = Ogre::StringConverter::toString((unsigned long)(info.display()));
	externalWindowHandleParams += ":";
	externalWindowHandleParams += Ogre::StringConverter::toString(boost::numeric_cast<unsigned int>(info.screen()));
	externalWindowHandleParams += ":";
	externalWindowHandleParams += Ogre::StringConverter::toString(boost::numeric_cast<unsigned long>(winId()));
	//externalWindowHandleParams += ":";
	//externalWindowHandleParams += Ogre::StringConverter::toString((unsigned long)(info.visual()));

	//qDebug() << QString("Display: %1\nScreen: %2\nWindow: %3\nVisual: %4").arg((unsigned long)(info.display())).arg((unsigned long)(info.screen())).arg((unsigned long)(winId())).arg((unsigned long)(info.visual()));

	//qDebug() << "external window handle params look like this: " << externalWindowHandleParams.c_str();
#endif

	// Add the external window handle parameters to the existing params set.
#if defined(Q_WS_WIN)
	params["externalWindowHandle"] = externalWindowHandleParams;
#endif

#if defined(Q_WS_X11)
	params["parentWindowHandle"] = externalWindowHandleParams;
#endif

	const QPoint absolutePos = this->mapToGlobal(pos());
	params["left"] = Ogre::StringConverter::toString(absolutePos.x());
	params["top"] = Ogre::StringConverter::toString(absolutePos.y());

	// Finally create our window.
	try
	{
		qDebug() << "Creating render window with name " << name().c_str() << " width " << width() << " height " << height() << " and params: ";

		for (Ogre::NameValuePairList::const_iterator iterator = params.begin(); iterator != params.end(); ++iterator)
		{
			qDebug() << iterator->first.c_str() << ":" << iterator->second.c_str();
		}

		// TODO segmentation fault occurs with the useflag "threads" in OGRE:
		// http://www.ogre3d.org/forums/viewtopic.php?f=2&t=70021
		this->m_renderWindow = this->m_root->createRenderWindow(name(), width(), height(), false, &params);
	}
	catch (const Ogre::RenderingAPIException &exception) // cancel
	{
		KMessageBox::detailedError(this, i18n("Error during render window creation with rendering engine OGRE."), exception.what());

		return;
	}

	this->renderWindow()->setActive(true);
	this->renderWindow()->setVisible(true);
	// old stuff, added stuff from QtOgreWidget
	//WId ogreWinId = 0x0;
	//this->m_renderWindow->getCustomAttribute( "WINDOW", &ogreWinId);
	//assert(ogreWinId);
/*
OLD!
#if defined(Q_WS_X11)
	WId ogreWinId = 0x0;
	this->m_renderWindow->getCustomAttribute("WINDOW", &ogreWinId);
	assert(ogreWinId);
	this->create(winId());
#endif
*/

	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_OpaquePaintEvent);

	//== Ogre Initialization ==//
	// default scene
	this->m_sceneManager = this->root()->createSceneManager(this->m_sceneType);
	this->m_sceneManager->setAmbientLight(Ogre::ColourValue::White);
	this->m_camera = this->sceneManager()->createCamera("Widget_Cam");
	this->m_viewPort = this->renderWindow()->addViewport(this->camera());
	this->viewPort()->setBackgroundColour(Ogre::ColourValue(0.8, 0.8, 1));
	/*
	Ogre::Light *light = this->m_sceneManager->createLight("Light1");
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(Ogre::Vector3(250, 150, 250));
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue::White);
	*/

	this->centerView();

	// default graphics settings
	// VSync
	// Resolution
	/// @todo Should be configurable for the whole editor (using singleton)
	//Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	//Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(2);

	// if this function is called background becomes blue but program hangs up
	// we use custom rendering calls
	//this->m_root->startRendering(); /// @todo Error!
}

bool ModelView::checkCameraMovementBounds(const Ogre::Vector3 &delta)
{
	// TODO FIXME (bounds don't work)
	return true;
	//return sceneManager()->getRootSceneNode()->_getWorldAABB().contains(camera()->getPosition() + delta);
}

#include "moc_modelview.cpp"

}

}
