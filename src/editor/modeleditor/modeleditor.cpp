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

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <QtGui>
#include <QtWidgets/QtWidgets>

#include <boost/foreach.hpp>

#include "modeleditor.hpp"
#include "modeleditorview.hpp"
#include "../editor.hpp"
#include "../../utilities.hpp"
#include "../resource.hpp"
#include "modeleditorsettings.hpp"
#include "../renderstatswidget.hpp"
#include "../teamcolordialog.hpp"

namespace wc3lib
{

namespace editor
{

ModelEditor::ModelEditor(Root *root, class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f)
: Module(source, parent, f)
, m_modelView(new ModelEditorView(root, this))
, m_recentUrl("")
, m_models()
, m_viewMenu(0)
, m_renderStatsWidget(0)
, m_teamColorDialog(0)
, m_teamGlowDialog(0)
, m_showStatsAction(0)
, m_showCollisionShapesAction(0)
, m_teamColor(TeamColor::Red)
, m_teamGlow(TeamColor::Red)
{
	Module::setupUi();
	//this->modelView()->setMinimumSize(QSize(640, 480));
	//this->setAcceptDrops(true); // enable drag & drop
	this->modelView()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // NOTE otherwise vertical policy won't expand to free space!
	centerLayout()->addWidget(modelView());
	//this->m_modelView->setLayout(this->m_horizontalLayout);
	/*
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(this->m_modelView);
	this->m_modelViewWidget->setLayout(mainLayout);
	*/
	/// @todo Model view should be bigger than modules buttons
	//QGridLayout *layout = new QGridLayout(this);
	//layout->addWidget(this->m_modelView);
	//this->topLayout()->addWidget(this->m_modelView);
	/*
	QStandardItemModel *standardItemModel = new QStandardItemModel(this);
	QModelIndex modelIndex;
	standardItemModel->insertRows(0, 10, modelIndex);
	standardItemModel->setData(modelIndex, tr("Standard-Modelle"), Qt::DisplayRole);
	standardItemModel->setData(modelIndex, QIcon(), Qt::DecorationRole);
	this->m_treeView->setModel(standardItemModel);
	*/
}

ModelEditor::~ModelEditor()
{
	hideCollisionShapes();
	closeAllFiles();
}

void ModelEditor::show()
{
	Module::show();
	this->modelView()->show();
	/// @todo FIXME: Either create model view data in constructor or get the right position for this function call.
	//readSettings(); // read settings when model view is being shown since its render window is also being created at that moment
}

bool ModelEditor::configure()
{
	readSettings();

	if (!this->modelView()->root()->configure())
	{
		return false;
	}

	// Update required files if started as stand-alone module
	if (!hasEditor())
	{
		try
		{
			source()->sharedData()->refreshWorldEditorStrings(this);
		}
		catch (wc3lib::Exception &e)
		{
			QMessageBox::critical(this, tr("Error"), tr("Error when loading default files: %1").arg(e.what()));

			return false;
		}
	}

	return true;
}

void ModelEditor::retranslateUi()
{
	Module::retranslateUi();
}

void ModelEditor::hideCollisionShapes()
{
	this->m_showCollisionShapesAction->setText(tr("Show Collision Shapes"));

	BOOST_FOREACH(CollisionShapeNodes::left_value_type value, m_collisionShapeNodes.left)
	{
		qDebug() << "Deleting movable object of collision shape: " << value.second->getAttachedObject(0)->getName().c_str();
		modelView()->sceneManager()->destroyMovableObject(value.second->getAttachedObject(0)); // only one object should be attached per node
		modelView()->sceneManager()->destroySceneNode(value.second); // destroy node
	}

	m_collisionShapeNodes.left.clear();
}

void ModelEditor::openFile()
{
	const QList<QUrl> urls = QFileDialog::getOpenFileUrls(this, tr("Open Model"), this->m_recentUrl, tr("*.mdl|Blizzard Model (*.mdl)\n*.mdx|Compressed Blizzard Model (*.mdx)\n*|All files (*)"));

	if (urls.isEmpty())
	{
		return;
	}

	foreach (const QUrl &url, urls)
	{
		this->openUrl(url);
	}
}

void ModelEditor::saveFile()
{
	if (this->m_models.empty())
	{
		QMessageBox::critical(this, tr("Error"), tr("There is no model to export."));

		return;
	}

	const QUrl url = QFileDialog::getSaveFileUrl(this, tr("Save File"), this->m_recentUrl, tr("*.mdl|Blizzard Model (*.mdl)\n*.mdx|Compressed Blizzard Model (*.mdx)\n*.mesh|OGRE mesh (*.mesh)\n*|All files (*)"));

	if (url.isEmpty())
	{
		return;
	}

	try
	{
		(*this->models().begin()).save(url);
	}
	catch (Exception exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to write file \"%1\".\nException \"%2\".").arg(url.toEncoded().constData()).arg(exception.what()));
	}
}

void ModelEditor::closeAllFiles()
{
	for (Models::iterator iterator = m_models.begin(); iterator != m_models.end(); )
	{
		Models::iterator next = iterator;
		++iterator;
		removeModel(next);
	}
}

void ModelEditor::showSettings()
{
	/*
	if (this->m_settingsDialog == 0)
		this->m_settingsDialog = new ModelEditorSettingsDialog(this, tr("Model Editor Settings"), 0);

	this->m_settingsDialog.show();
	*/
}

void ModelEditor::centerView()
{
	this->modelView()->centerView();
}

void ModelEditor::setPolygonModePoints()
{
	this->modelView()->setPolygonModePoints();
}

void ModelEditor::setPolygonModeWireframe()
{
	this->modelView()->setPolygonModeWireframe();
}

void ModelEditor::setPolygonModeSolid()
{
	this->modelView()->setPolygonModeSolid();
}

void ModelEditor::showStats()
{
	if (this->m_renderStatsWidget == 0)
	{
		this->m_renderStatsWidget = new RenderStatsWidget(this->modelView(), this);
		centerLayout()->addWidget(this->m_renderStatsWidget);
	}

	if (this->m_renderStatsWidget->isVisible())
	{
		m_showStatsAction->setText(tr("Show Stats"));
	}
	else
	{
		m_showStatsAction->setText(tr("Hide Stats"));
	}


	this->m_renderStatsWidget->setVisible(!this->m_renderStatsWidget->isVisible());
}
/*
namespace
{

Ogre::ManualObject* createSphere(const std::string &strName, const float r, const int nRings = 16, const int nSegments = 16)
{
	using namespace Ogre;
	ManualObject * manual = new Ogre::ManualObject(strName);
	manual->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_LIST);

	float fDeltaRingAngle = (Math::PI / nRings);
	float fDeltaSegAngle = (2 * Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
		float x0 = r0 * sinf(seg * fDeltaSegAngle);
		float z0 = r0 * cosf(seg * fDeltaSegAngle);

		// Add one vertex to the strip which makes up the sphere
		manual->position( x0, y0, z0);
		manual->normal(Vector3(x0, y0, z0).normalisedCopy());
		manual->textureCoord((float) seg / (float) nSegments, (float) ring / (float) nRings);

		if (ring != nRings) {
			// each vertex (except the last) has six indicies pointing to it
			manual->index(wVerticeIndex + nSegments + 1);
			manual->index(wVerticeIndex);
			manual->index(wVerticeIndex + nSegments);
			manual->index(wVerticeIndex + nSegments + 1);
			manual->index(wVerticeIndex + 1);
			manual->index(wVerticeIndex);
			wVerticeIndex ++;
			}
		}; // end for seg
	} // end for ring
	manual->end();
	MeshPtr mesh = manual->convertToMesh(name);
	mesh->_setBounds( AxisAlignedBox( Vector3(-r, -r, -r), Vector3(r, r, r) ), false );

	mesh->_setBoundingSphereRadius(r);
	unsigned short src, dest;

	if (!mesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
	{
		mesh->buildTangentVectors(VES_TANGENT, src, dest);
	}

	return manual;
}


}
*/

void ModelEditor::showCollisionShapes()
{
	if (collisionShapeNodes().left.empty())
	{
		this->m_showCollisionShapesAction->setText(tr("Hide Collision Shapes"));
		qDebug() << "Showing collision shapes";

		BOOST_FOREACH(Models::reference value, models())
		{
			std::size_t i = 0;

			BOOST_FOREACH(OgreMdlx::CollisionShapes::const_reference collisionShape, value.collisionShapes())
			{
				std::string name = boost::str(boost::format("%1%.CollisionShape%2%") % value.namePrefix().toStdString() % i);
				Ogre::SceneNode *sceneNode = value.sceneNode()->createChildSceneNode(name.c_str());
				qDebug() << "Showing shape of type " << static_cast<mdlx::long32>(collisionShape.second->shape) << " with name " << name.c_str();

				switch (collisionShape.second->shape)
				{
					case mdlx::CollisionShape::Shape::Box:
					{
						Ogre::ManualObject *manualObject = new Ogre::ManualObject(name.c_str());
						manualObject->setBoundingBox(*collisionShape.second->box);

						sceneNode->attachObject(manualObject);
						sceneNode->showBoundingBox(true);
						qDebug() << "Bounding box";

						break;
					}

					/// \todo Create sphere
					case mdlx::CollisionShape::Shape::Sphere:
					{
						//Ogre::ManualObject *manualObject = new Ogre::ManualObject(name.c_str());
						//Ogre::MeshPtr mesh = manualObject->convertToMesh(name.c_str());
						//mesh->_setBoundingSphereRadius(collisionShape.second->sphere->getRadius());

						//Ogre::ManualObject *manualObject = createSphere(name, collisionShape.second->sphere->getRadius());
						//manualObject->se
						//manualObject->_s


						Ogre::Entity *entity = modelView()->sceneManager()->createEntity(name.c_str(), Ogre::SceneManager::PT_SPHERE);

						qDebug() << "Sphere center (" << collisionShape.second->sphere->getCenter().x << '|' << collisionShape.second->sphere->getCenter().y << '|' << collisionShape.second->sphere->getCenter().z << ")";
						qDebug() << "Sphere radius " << collisionShape.second->sphere->getRadius();

						sceneNode->attachObject(entity);
						sceneNode->setPosition(collisionShape.second->sphere->getCenter());
						sceneNode->setScale(Ogre::Vector3(collisionShape.second->sphere->getRadius())); // Radius, in theory.
						qDebug() << "Bounding sphere";

						break;
					}
				}

				m_collisionShapeNodes.left.insert(CollisionShapeNodes::left_value_type(collisionShape.second, sceneNode));

				++i;
			}
		}
	}
	else
	{
		hideCollisionShapes();
	}
}

void ModelEditor::changeTeamColor()
{
	if (m_teamColorDialog == 0)
	{
		m_teamColorDialog = new TeamColorDialog(this);
	}

	m_teamColorDialog->setTeamColor(teamColor());

	if (m_teamColorDialog->exec() == TeamColorDialog::Accepted)
	{
		setTeamColor(m_teamColorDialog->teamColor());
	}

	/*
	QColor color;

	if (KColorDialog::getColor(color, OgreMdlx::teamColor(this->m_models.left.begin()->second->teamColor()), this) == KColorDialog::Ok)
		this->m_models.left.begin()->second->setTeamColor(OgreMdlx::teamColor(color));
	*/
}

void ModelEditor::changeTeamGlow()
{
	if (m_teamGlowDialog == 0)
	{
		m_teamGlowDialog = new TeamColorDialog(this);
	}

	m_teamGlowDialog->setTeamColor(teamGlow());

	if (m_teamGlowDialog->exec() == TeamColorDialog::Accepted)
	{
		setTeamGlow(m_teamColorDialog->teamColor());
	}

	/*
	QColor color;

	if (KColorDialog::getColor(color, OgreMdlx::teamColor(this->m_models.left.begin()->second->teamGlow()), this) == KColorDialog::Ok)
		this->m_models.left.begin()->second->setTeamGlow(OgreMdlx::teamColor(color));
	*/
}

void ModelEditor::viewCamera()
{
	QAction *action = boost::polymorphic_cast<QAction*>(QObject::sender());
	//this->m_modelView->setCamera(this->m_cameraActions.left.at(action));
	OgreMdlx::updateCamera(*this->m_cameraActions.left.at(action), this->m_modelView->camera()); // updates current camera to camera's position
	//this->m_modelView->camera()->setAspectRatio(static_cast<Ogre::Real>(this->modelView()->viewPort()->getActualWidth()) / static_cast<Ogre::Real>(this->modelView()->viewPort()->getActualHeight()));
}

void ModelEditor::dragEnterEvent(QDragEnterEvent *event)
{
	/// @todo If it's an MDLX file event->acceptProposedAction();
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void ModelEditor::dropEvent(QDropEvent *event)
{
	/// @todo If it's an MDLX file event->acceptProposedAction();
	if (event->mimeData()->hasUrls())
	{
		foreach (QUrl url, event->mimeData()->urls())
		{
			this->openUrl(url);
		}

		event->accept();
	}
}

bool ModelEditor::openUrl(const QUrl &url)
{
	//const Ogre::Vector3 position(0.0, 0.0, 0.0);
	std::auto_ptr<OgreMdlx> ogreModel(new OgreMdlx(url, this->m_modelView));

	try
	{
		ogreModel->setSource(source());
		ogreModel->load();
		QMessageBox::information(this, tr("Open File"), tr("Read file \"%1\" successfully..").arg(url.toEncoded().constData()));
	}
	catch (const Exception &exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read file \"%1\".\nException \"%2\".").arg(url.toEncoded().constData()).arg(exception.what()));

		return false;
	}

	OgreMdlx *model = ogreModel.get();
	this->m_models.push_back(ogreModel);
	this->m_modelView->root()->addFrameListener(model);
	addCameraActions(*model);

	try
	{
		model->setTeamColor(teamColor());
		model->setTeamGlow(teamGlow());
	}
	catch (const Exception &exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to assign team color %1 and team glow %2 to model \"%3\".\nException \"%4\".").arg(static_cast<int>(teamColor())).arg(static_cast<int>(teamGlow())).arg(url.toEncoded().constData()).arg(exception.what()));
	}

	Ogre::String entityName = Ogre::String(QString(model->namePrefix() +"Entity").toStdString());
	qDebug() << "Scene manager for entity " << entityName.c_str() << " = " << this->modelView()->sceneManager();
	m_entities.left.insert(std::make_pair(model, new OgreMdlxEntity(entityName, model, this->modelView()->sceneManager()))); // create entity

	return true;
}

void ModelEditor::removeModel(const OgreMdlx &ogreModel)
{
	for (Models::iterator model = models().begin(); model != models().end(); ++model)
	{
		if (&(*model) == &ogreModel)
		{
			removeModel(model);

			return;
		}
	}

	qDebug() << "Error: Model " << ogreModel.namePrefix() << " was not found.";
}

void ModelEditor::removeModel(Models::iterator iterator)
{
	qDebug() << "Removing model " << iterator->namePrefix();

	Entities::left_iterator entityIterator = m_entities.left.find(&*iterator);

	if (entityIterator != m_entities.left.end())
	{
		OgreMdlxEntity *entity = entityIterator->second;
		m_entities.left.erase(entityIterator);
		qDebug() << "Removing model entity " << entity->sceneNode()->getName().c_str();
		delete entity;
	}

	this->m_modelView->root()->removeFrameListener(&(*iterator));
	removeCameraActions(*iterator);
	m_models.erase(iterator);
}

void ModelEditor::addCameraActions(const OgreMdlx &ogreModel)
{
	if (this->m_viewMenu != 0)
	{
		BOOST_FOREACH(OgreMdlx::Cameras::const_reference iterator, ogreModel.cameras())
		{
			QAction *action = new QAction(QIcon(":/actions/viewcamera.png"), tr("Camera: %1", iterator.first->name()), this);
			connect(action, SIGNAL(triggered()), this, SLOT(viewCamera()));
			this->m_viewMenu->addAction(action);
			this->m_cameraActions.left.insert(CameraActions::left_value_type(action, iterator.first));
		}
	}
}

void ModelEditor::removeCameraActions(const OgreMdlx &ogreModel)
{
	if (this->m_viewMenu != 0)
	{
		BOOST_FOREACH(OgreMdlx::Cameras::const_reference camera, ogreModel.cameras())
		{
			CameraActions::right_iterator iterator = m_cameraActions.right.find(camera.first);

			if (iterator != m_cameraActions.right.end())
			{
				delete iterator->second; // delete action
				m_cameraActions.right.erase(iterator);
			}
		}
	}
}

void ModelEditor::createFileActions(QMenu *menu)
{
	QAction *action = 0;

	action = new QAction(QIcon(":/actions/openmodel.png"), tr("Open model"), this);
	//action->setShortcut(KShortcut(tr("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(openFile()));
	menu->addAction(action);

	action = new QAction(QIcon(":/actions/savemodel.png"), tr("Save model"), this);
	//action->setShortcut(KShortcut(tr("Ctrl+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveFile()));
	menu->addAction(action);

	action = new QAction(QIcon(":/actions/closeallmodels.png"), tr("Close all models"), this);
	//action->setShortcut(KShortcut(tr("
	connect(action, SIGNAL(triggered()), this, SLOT(closeAllFiles()));
	menu->addAction(action);

	action = new QAction(QIcon(":/actions/settings.png"), tr("Settings"), this);
	//action->setShortcut(KShortcut(tr("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(settings()));
	menu->addAction(action);
}

void ModelEditor::createEditActions(QMenu *menu)
{
	qDebug() << "Edit actions";
}

void ModelEditor::createMenus(QMenuBar *menuBar)
{
	QMenu *viewMenu = new QMenu(tr("View"), this);
	this->m_viewMenu = viewMenu;
	menuBar->addMenu(viewMenu);

	// test actions for one single view port/camera

	QAction *action = new QAction(QIcon(":/actions/centerview.png"), tr("Center View"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(centerView()));
	viewMenu->addAction(action);

	action = new QAction(QIcon(":/actions/polygonmodepoints.png"), tr("Polygon Mode Points"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(setPolygonModePoints()));
	viewMenu->addAction(action);

	action = new QAction(QIcon(":/actions/polygonmodewireframe.png"), tr("Polygon Mode Wireframe"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(setPolygonModeWireframe()));
	viewMenu->addAction(action);

	action = new QAction(QIcon(":/actions/polygonmodesolid.png"), tr("Polygon Mode Solid"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(setPolygonModeSolid()));
	viewMenu->addAction(action);

	action = new QAction(QIcon(":/actions/showstats.png"), tr("Show Stats"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(showStats()));
	viewMenu->addAction(action);
	m_showStatsAction = action;

	action = new QAction(QIcon(":/actions/showcollisionshapes.png"), tr("Show Collision Shapes"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(showCollisionShapes()));
	viewMenu->addAction(action);
	m_showCollisionShapesAction = action;

	action = new QAction(QIcon(":/actions/changeteamcolor.png"), tr("Change team color"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(changeTeamColor()));
	viewMenu->addAction(action);

	action = new QAction(QIcon(":/actions/changeteamglow.png"), tr("Change team glow"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(changeTeamGlow()));
	viewMenu->addAction(action);

	qDebug() << "Menus";
}

void ModelEditor::createWindowsActions(WindowsMenu *menu)
{
	qDebug() << "Windows";
}

void ModelEditor::createToolButtons(ModuleToolBar *toolBar)
{
	qDebug() << "Tool buttons";
}

class SettingsInterface* ModelEditor::settings()
{
	return new ModelEditorSettings(this);
}

void ModelEditor::onSwitchToMap(Map *map)
{
}

QIcon ModelEditor::icon()
{
	return this->source()->sharedData()->icon(QUrl("ReplaceableTextures/WorldEditUI/Editor-ImportManager.blp"), this);
}

#include "moc_modeleditor.cpp"

}

}
