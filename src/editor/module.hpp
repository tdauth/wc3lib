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

#ifndef WC3LIB_EDITOR_MODULE_HPP
#define WC3LIB_EDITOR_MODULE_HPP

#include <typeinfo>

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMenu>
#include <QAction>
#include <QMenuBar>

#include <kdemacros.h>
#include <KComponentData>
#include <KAboutData>

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ModuleMenu;
class ModuleToolBar;
class Editor;
class WindowsMenu;
class SettingsInterface;
class Map;
class SourcesDialog;

/**
 * \brief Abstract class for module implementation such as model or terrain editors.
 * Implement the pure virtual member functions to customize your module's menu, actions and tool buttons.
 * \note Modules should work independently without an Editor instance. They only need a data source.
 * \note Since Module is based on KXMLGUIClient it provides an extensible system using specific XML GUI files.
 */
class KDE_EXPORT Module : public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \param source Source where files are loaded from when requested. Should not be 0. At least you should create an empty \ref MpqPriorityList instance. To emulate Blizzard's World Editor you can use an \ref Editor instance.
		 * \param parent Parent widget for which the module widget is created.
		 * \param f Window flags for the module widget.
		 */
		Module(MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~Module();
		MpqPriorityList* source() const;
		/**
		 * \warning Don't use in constructor since it calls virtual function \ref moduleAboutData() on initialization.
		 */
		KComponentData& componentData();
		/**
		 * \copydoc Module::componentData()
		 */
		const KComponentData& componentData() const;
		QMenu* fileMenu() const;
		QMenu* editMenu() const;
		ModuleMenu* moduleMenu() const;
		QMenuBar* menuBar() const;
		WindowsMenu* windowsMenu() const;
		ModuleToolBar* toolBar() const;

		/**
		 * This function uses dynamic type checking to get the source's type.
		 * \return Returns true if \ref source() is an Editor instance. Otherwise it returns false.
		 * \sa source(), editor()
		 */
		bool hasEditor() const;
		/**
		 * \return Returns casted Editor instance (from \ref source()).
		 * \throw std::bad_cast Throws an exception when \ref hasEditor() returns false and therefore source isn't an Editor instance.
		 * \sa source(), hasEditor()
		 */
		Editor* editor() const;

		/**
		 * Name of corresponding action in module menu of editor's action collection.
		 * \sa moduleMenu()
		 */
		virtual QString actionName() const = 0;

		/**
		 * The module's icon is usually used as window icon as well as action icon by the World Editor and \ref ModuleToolBar.
		 * \return Returns the corresponding icon of the module.
		 */
		virtual QIcon icon() = 0;

		QString settingsGroup() const;

		/**
		 * Configures the settings and sources before showing the module.
		 * \note You should load translations from \ref retranslateUi() after loading the sources successfully.
		 *
		 * The default implementation opens a source dialog if the module is started as stand-alone application and the sources are empty.
		 * If accepted it retranslates the UI and returns true. Otherwise it returns false.
		 *
		 * When overriding this element function you can refresh required files after making sure the sources has been configured.
		 * Besides you could prevent opening the GUI if this method returns false.
		 *
		 * The GUI should always be shown AFTER calling this element function.
		 *
		 * \return Returns true if the configuration has been successful. For example, when the user rejects the sources dialog it returns false.
		 */
		virtual bool configure();
		/**
		 * Updates the translated strings in the GUI.
		 */
		virtual void retranslateUi();

	public slots:
		void showSourcesDialog();
		void aboutQt();
		void aboutKde();

	protected:
		/**
		 * Calls this member function to setup the usual module user interface which contains some default menus.
		 * \note If you overwrite this function you should consider calling \ref moduleAboutData().
		 */
		virtual void setupUi();

		virtual void createFileActions(QMenu *menu) = 0;
		virtual void createEditActions(QMenu *menu) = 0;
		virtual void createMenus(QMenuBar *menuBar) = 0;
		virtual void createWindowsActions(WindowsMenu *menu) = 0;
		/**
		 * Use \ref ModuleToolBar::addCustomAction() or \ref ModuleToolBar::addCustomSeparator() to add stuff before the modules buttons.
		 */
		virtual void createToolButtons(ModuleToolBar *toolBar) = 0;
		virtual SettingsInterface* settings() = 0;
		/**
		 * About data which is assigned to the module and can be accessed via \ref componentData().
		 * Default implementation returns \ref Editor::wc3libAboutData() with app and catalog name using \ref actionName().
		 * \note Assignment runs in \ref setupUi().
		 */
		virtual KAboutData moduleAboutData() const;

		QVBoxLayout* topLayout() const;
		QGridLayout* centerLayout() const;

		/**
		 * This event occurs when module has an \ref Editor and its opened map is changed.
		 */
		virtual void onSwitchToMap(Map *map) = 0;

		virtual void changeEvent(QEvent *event);



		/**
		 * Reads settings of the module.
		 * This has to be called manually in the derived class.
		 * By default it reads the source list of the module.
		 */
		virtual void readSettings();
		virtual void writeSettings();

	protected slots:
		/**
		 * This slot is called whenever the module is selected in the Editor's "module" menu.
		 */
		virtual void onEditorActionTrigger();
		virtual void triggered(QAction *action);

	private:
		MpqPriorityList *m_source;
		typedef QScopedPointer<KComponentData> ComponentDataPtr;
		ComponentDataPtr m_componentData;
		QMenu *m_fileMenu;
		QMenu *m_editMenu;
		ModuleMenu *m_moduleMenu;
		QMenuBar *m_menuBar;
		WindowsMenu *m_windowsMenu;
		QMenu *m_helpMenu;
		ModuleToolBar *m_toolBar;
		QAction *m_closeAction;
		QAction *m_sourcesAction;

		QAction *m_aboutQtAction;
		QAction *m_aboutKdeAction;

		QVBoxLayout *m_topLayout;
		QGridLayout *m_centerLayout;

		SourcesDialog *m_sourcesDialog;

	private slots:
		void switchToMap(Map *map);
};

inline MpqPriorityList* Module::source() const
{
	return this->m_source;
}

inline KComponentData& Module::componentData()
{
	if (m_componentData.isNull())
	{
		const_cast<Module*>(this)->m_componentData.reset(new KComponentData(moduleAboutData()));
	}

	return *m_componentData;
}

inline const KComponentData& Module::componentData() const
{
	if (m_componentData.isNull())
	{
		const_cast<Module*>(this)->m_componentData.reset(new KComponentData(moduleAboutData()));
	}

	return *m_componentData;
}

inline QMenu* Module::fileMenu() const
{
	return m_fileMenu;
}

inline QMenu* Module::editMenu() const
{
	return m_editMenu;
}

inline ModuleMenu* Module::moduleMenu() const
{
	return m_moduleMenu;
}

inline QMenuBar* Module::menuBar() const
{
	return m_menuBar;
}

inline WindowsMenu* Module::windowsMenu() const
{
	return m_windowsMenu;
}

inline ModuleToolBar* Module::toolBar() const
{
	return m_toolBar;
}

inline QVBoxLayout* Module::topLayout() const
{
	return this->m_topLayout;
}

inline QGridLayout* Module::centerLayout() const
{
	return this->m_centerLayout;
}

}

}

#endif
