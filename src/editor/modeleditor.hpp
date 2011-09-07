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

#ifndef WC3LIB_EDITOR_MODELEDITOR_HPP
#define WC3LIB_EDITOR_MODELEDITOR_HPP

#include <QDebug>

#include <KUrl>
#include <KMessageBox>
#include <KLocale>

#include <Ogre.h>

#include "../mdlx.hpp"
#include "module.hpp"
#include "ui/ui_modeleditor.h"
#include "ogremdlx.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * We have model SLK entries listet at the tree view.
 */
class ModelEditor : public Module, protected Ui::ModelEditor
{
	Q_OBJECT

	public:
		typedef boost::shared_ptr<OgreMdlx> OgreMdlxPtr;
		//typedef boost::bimap<MdlxPtr, OgreMdlxPtr> Models;
		typedef std::list<OgreMdlxPtr> Models;

		typedef boost::bimap<QAction*, const mdlx::Camera*> CameraActions;
		typedef boost::bimap<const OgreMdlx::CollisionShape*, Ogre::SceneNode*> CollisionShapeNodes;

		ModelEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~ModelEditor();

		virtual void show();

		void hideCollisionShapes();

		class ModelEditorView* modelView() const;
		Models& models();
		const Models& models() const;
		const CameraActions& cameraActions() const;
		const CollisionShapeNodes& collisionShapeNodes() const;

		void setTeamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor);
		BOOST_SCOPED_ENUM(TeamColor) teamColor() const;
		void setTeamGlow(BOOST_SCOPED_ENUM(TeamColor) teamGlow);
		BOOST_SCOPED_ENUM(TeamColor) teamGlow() const;

	public slots:
		void openFile();
		void saveFile();
		void closeAllFiles();
		void showSettings();
		// test actions for one single view port/camera
		void centerView();
		void setPolygonModePoints();
		void setPolygonModeWireframe();
		void setPolygonModeSolid();
		void showStats();
		/**
		 * Shows or hides collision shape nodes.
		 * \sa collisionShapeNodes()
		 */
		void showCollisionShapes();
		void changeTeamColor();
		void changeTeamGlow();

	protected slots:
		void viewCamera();

	protected:
		friend class ModelEditorSettings;

		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class KMenu *menu);
		virtual void createToolButtons(class KToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual void onSwitchToMap(class Map *map);
		virtual QString actionName();

		// load file events
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent *event);

		bool openUrl(const KUrl &url);
		void removeModel(OgreMdlxPtr ogreModel);

		void addCameraActions(const OgreMdlxPtr &ogreModel);
		void removeCameraActions(const OgreMdlxPtr &ogreModel);

		class ModelEditorView *m_modelView;
		class ModelEditorSettingsDialog *m_settingsDialog;
		KUrl m_recentUrl;
		Models m_models;
		CameraActions m_cameraActions;
		CollisionShapeNodes m_collisionShapeNodes;

		class KMenu *m_viewMenu;
		class RenderStatsWidget *m_renderStatsWidget;
		class TeamColorDialog *m_teamColorDialog;
		class TeamColorDialog *m_teamGlowDialog;

		class KAction *m_showStatsAction;
		class KAction *m_showCollisionShapesAction;

		BOOST_SCOPED_ENUM(TeamColor) m_teamColor;
		BOOST_SCOPED_ENUM(TeamColor) m_teamGlow;
};

inline class ModelEditorView* ModelEditor::modelView() const
{
	return this->m_modelView;
}

inline ModelEditor::Models& ModelEditor::models()
{
	return m_models;
}

inline const ModelEditor::Models& ModelEditor::models() const
{
	return m_models;
}

inline const ModelEditor::CameraActions& ModelEditor::cameraActions() const
{
	return m_cameraActions;
}

inline const ModelEditor::CollisionShapeNodes& ModelEditor::collisionShapeNodes() const
{
	return m_collisionShapeNodes;
}

inline void ModelEditor::setTeamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor)
{
	m_teamColor = teamColor;
	qDebug() << "Changing team color to " << teamColor;

	BOOST_FOREACH(Models::value_type value, this->models())
	{
		try
		{
			value->setTeamColor(this->teamColor());
		}
		catch (Exception &exception)
		{
			KMessageBox::error(this, i18n("Unable to assign team color %1 and to model \"%2\".\nException \"%3\".", this->teamColor(), value->url().toEncoded().constData(), exception.what().c_str()));
		}
	}
}

inline BOOST_SCOPED_ENUM(TeamColor) ModelEditor::teamColor() const
{
	return m_teamColor;
}

inline void ModelEditor::setTeamGlow(BOOST_SCOPED_ENUM(TeamColor) teamGlow)
{
	m_teamGlow = teamGlow;
	qDebug() << "Changing team glow to " << teamGlow;

	BOOST_FOREACH(Models::value_type value, this->models())
	{
		try
		{
			value->setTeamGlow(this->teamGlow());
		}
		catch (Exception &exception)
		{
			KMessageBox::error(this, i18n("Unable to assign team glow %1 and to model \"%2\".\nException \"%3\".", this->teamGlow(), value->url().toEncoded().constData(), exception.what().c_str()));
		}
	}
}

inline BOOST_SCOPED_ENUM(TeamColor) ModelEditor::teamGlow() const
{
	return m_teamGlow;
}

inline QString ModelEditor::actionName()
{
	return "modeleditor";
}

}

}

#endif
