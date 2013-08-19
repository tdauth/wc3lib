/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include <QtGui>

#include "modeleditorview.hpp"
#include "modeleditor.hpp"
#include "../editor.hpp"
#include "../../core.hpp"

namespace wc3lib
{

namespace editor
{

ModelEditorView::ModelEditorView(class ModelEditor *modelEditor, Qt::WFlags f, Ogre::SceneType ogreSceneType, const Ogre::NameValuePairList *ogreParameters) : ModelView(modelEditor->hasEditor() ? modelEditor->editor()->root() : 0, modelEditor, f, ogreSceneType, ogreParameters), m_hitTest(false) /// TODO m_hitTest(true) should be usual, changed for school only
{
}

ModelEditorView::~ModelEditorView()
{
}

void ModelEditorView::mousePressEvent(QMouseEvent *event)
{
	if (renderWindow() != 0 && hitTest() && event->button() == Qt::LeftButton)
	{
		//QPoint point(event->globalX() - this->modelView()->x(), event->globalY() - this->modelView()->y());
		QPoint point(event->x(), event->y());
		
		qDebug() << "Selection event in model editor trying \"hit test\"!\nCoordinates are (" << point.x() << "|" << point.y() << ")";
		Ogre::Ray mouseRay = this->camera()->getCameraToViewportRay(point.x() / float(this->width()), point.y() / float(this->height()));
		// map of selections results sorted by selection distance
		typedef std::map<Ogre::Real, class OgreMdlx*> Results;
		Results results;
		
		/*
		Ogre::RaySceneQuery *raySceneQuery = this->modelView()->sceneManager()->createRayQuery(mouseRay);
		raySceneQuery->setRay(mouseRay);
		raySceneQuery->setSortByDistance(true);
		Ogre::RaySceneQueryResult &result = raySceneQuery->execute();

		foreach(const Ogre::RaySceneQueryResultEntry &entry, result)
		{
			if (entry.movable)
			{
		*/
		/*
		 * TODO FIXME HIT TEST
		modelEditor()->models().left.empty(); // TEST
		BOOST_FOREACH(ModelEditor::Models::left_const_reference value, modelEditor()->models().left)
		{
			OgreMdlx *ogreMdlx = value.second;
			bool selected = false;
			
			BOOST_FOREACH(OgreMdlx::CollisionShapes::value_type collisionShapeValue, ogreMdlx->collisionShapes())
			{
				OgreMdlx::CollisionShape *collisionShape = collisionShapeValue.second;
				std::pair<bool, Ogre::Real> result;
				
				if (collisionShape->shape == mdlx::CollisionShape::Box)
					result = mouseRay.intersects(*(collisionShape->box));
				// sphere
				else
					result = mouseRay.intersects(*(collisionShape->sphere));
				
				// intersects
				if (result.first)
				{
					selected = true;
					results[result.second] = ogreMdlx;
					
					break;
				}
			}
			
			if (selected)
				qDebug() << "SELECTED MODEL " << ogreMdlx->mdlx()->model()->name();
		}
		
		qDebug() << "Here are all selection results sorted by distance:";
		
		BOOST_FOREACH(Results::const_reference value, results)
			qDebug() << "Name \"" << value.second->mdlx()->model()->name() << "\", distance " << value.first;
				/*
				entry.movable->setVisible(!entry.movable->getVisible());
			}
		}

		delete raySceneQuery;
		raySceneQuery = 0;
		*/
	}
	
	ModelView::mousePressEvent(event);
}

}

}
