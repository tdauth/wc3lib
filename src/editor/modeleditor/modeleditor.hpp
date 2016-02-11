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

#include <boost/ptr_container/ptr_list.hpp>

#include <QDebug>
#include <QMessageBox>

#include <kdemacros.h>

#include <Ogre.h>

#include "../../mdlx.hpp"
#include "../module.hpp"
#include "../ogremdlx.hpp"
#include "../ogremdlxentity.hpp"
#include "../root.hpp"
#include "../platform.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ModelEditorView;
class ModelEditorSettings;
class ModelEditorSettingsDialog;
class RenderStatsWidget;
class TeamColorDialog;
class CollisionShape;

/**
 * We have model SLK entries listet at the tree view.
 */
class KDE_EXPORT ModelEditor : public Module
{
	Q_OBJECT

	public:
		typedef boost::ptr_list<OgreMdlx> Models;

		typedef boost::bimap<const OgreMdlx*, OgreMdlxEntity*> Entities;

		typedef boost::bimap<QAction*, const mdlx::Camera*> CameraActions;
		typedef boost::bimap<const CollisionShape*, Ogre::SceneNode*> CollisionShapeNodes;

		ModelEditor(Root *root, MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~ModelEditor();

		virtual bool configure() override;
		virtual void retranslateUi() override;

		virtual void show();

		void hideCollisionShapes();

		ModelEditorView* modelView() const;
		Models& models();
		const Models& models() const;
		const Entities& entities() const;
		const CameraActions& cameraActions() const;
		const CollisionShapeNodes& collisionShapeNodes() const;

		/**
		 * Assigns the team color \p teamColor to all open models in model editor.
		 */
		void setTeamColor(TeamColor teamColor);
		TeamColor teamColor() const;
		/**
		 * Assigns the team glow \p teamGlow to all open models in model editor.
		 */
		void setTeamGlow(TeamColor teamGlow);
		TeamColor teamGlow() const;

	public slots:
		void openFile();
		bool openUrl(const KUrl &url);
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
		friend ModelEditorSettings;

		virtual void createFileActions(QMenu *menu) override;
		virtual void createEditActions(QMenu *menu) override;
		virtual void createMenus(QMenuBar *menuBar) override;
		virtual void createWindowsActions(WindowsMenu *menu) override;
		virtual void createToolButtons(ModuleToolBar *toolBar) override;
		virtual SettingsInterface* settings() override;
		virtual void onSwitchToMap(Map *map) override;
		virtual QString actionName() const override;
		virtual QIcon icon() override;

		// load file events
		virtual void dragEnterEvent(QDragEnterEvent *event) override;
		virtual void dropEvent(QDropEvent *event) override;

		void removeModel(const OgreMdlx &ogreModel);
		void removeModel(Models::iterator iterator);

		void addCameraActions(const OgreMdlx &ogreModel);
		void removeCameraActions(const OgreMdlx &ogreModel);

		ModelEditorView *m_modelView;
		ModelEditorSettingsDialog *m_settingsDialog;
		KUrl m_recentUrl;
		Models m_models;
		Entities m_entities;
		CameraActions m_cameraActions;
		CollisionShapeNodes m_collisionShapeNodes;

		QMenu *m_viewMenu;
		RenderStatsWidget *m_renderStatsWidget;
		TeamColorDialog *m_teamColorDialog;
		TeamColorDialog *m_teamGlowDialog;

		QAction *m_showStatsAction;
		QAction *m_showCollisionShapesAction;

		TeamColor m_teamColor;
		TeamColor m_teamGlow;
};

inline ModelEditorView* ModelEditor::modelView() const
{
	return this->m_modelView;
}

inline ModelEditor::Models& ModelEditor::models()
{
	return m_models;
}

inline const ModelEditor::Entities& ModelEditor::entities() const
{
	return m_entities;
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

inline void ModelEditor::setTeamColor(TeamColor teamColor)
{
	m_teamColor = teamColor;
	qDebug() << "Changing team color to " << static_cast<int>(teamColor);

	BOOST_FOREACH(Models::reference value, this->models())
	{
		try
		{
			value.setTeamColor(this->teamColor());
		}
		catch (const Exception &exception)
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to assign team color %1 and to model \"%2\".\nException \"%3\".").arg(static_cast<int>(this->teamColor())).arg(value.url().toEncoded().constData()).arg(exception.what()));
		}
	}
}

inline TeamColor ModelEditor::teamColor() const
{
	return m_teamColor;
}

inline void ModelEditor::setTeamGlow(TeamColor teamGlow)
{
	m_teamGlow = teamGlow;
	qDebug() << "Changing team glow to " << static_cast<int>(teamGlow);

	BOOST_FOREACH(Models::reference value, this->models())
	{
		try
		{
			value.setTeamGlow(this->teamGlow());
		}
		catch (const Exception &exception)
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to assign team glow %1 and to model \"%2\".\nException \"%3\".").arg(static_cast<int>(this->teamGlow())).arg(value.url().toEncoded().constData()).arg(exception.what()));
		}
	}
}

inline TeamColor ModelEditor::teamGlow() const
{
	return m_teamGlow;
}

inline QString ModelEditor::actionName() const
{
	return "modeleditor";
}

}

}

#endif
