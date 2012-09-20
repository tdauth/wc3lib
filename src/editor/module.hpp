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

#include <KComponentData>
#include <KAboutData>
#include <KMenu>

namespace wc3lib
{

namespace editor
{

/**
 * \brief Abstract class for module implementation such as model or terrain editors.
 * Implement the pure virtual member functions to customize your module's menu, actions and tool buttons.
 * \note Modules should work independently without an Editor instance. They only need a data source.
 * \note Use class \ref Plugin to load modules related plugins. Since Module is based on \ref KXMLGUIClient it provides an extensible system using specific XML GUI files.
 */
class Module : public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \param source Source where files are loaded from when requested. Should not be 0. At least you should create an empty \ref MpqPriorityList instance. To emulate Blizzard's World Editor you can use an \ref Editor instance.
		 * \param parent Parent widget for which the module widget is created.
		 */
		Module(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~Module();
		class MpqPriorityList* source() const;
		/**
		 * \warning Don't use in constructor since it calls virtual function \ref moduleAboutData() on initialization.
		 */
		KComponentData& componentData();
		/**
		 * \copydoc Module::componentData()
		 */
		const KComponentData& componentData() const;
		class KMenu* fileMenu() const;
		class KMenu* editMenu() const;
		class ModuleMenu* moduleMenu() const;
		class KMenuBar* menuBar() const;
		class WindowsMenu* windowsMenu() const;
		class ModuleToolBar* toolBar() const;

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
		class Editor* editor() const throw (std::bad_cast);

		/**
		 * Name of corresponding action in module menu of editor's action collection.
		 * \sa moduleMenu()
		 */
		virtual QString actionName() const = 0;

	public slots:
		void showSourcesDialog();

	protected:
		/**
		 * Calls this member function to setup the usual module user interface which contains some default menus.
		 * \note If you overwrite this function you should consider calling \ref moduleAboutData().
		 */
		virtual void setupUi();

		virtual void createFileActions(class KMenu *menu) = 0;
		virtual void createEditActions(class KMenu *menu) = 0;
		virtual void createMenus(class KMenuBar *menuBar) = 0;
		virtual void createWindowsActions(class WindowsMenu *menu) = 0;
		/**
		 * Use \ref ModuleToolBar::addCustomAction() or \ref ModuleToolBar::addCustomSeparator() to add stuff before the modules buttons.
		 */
		virtual void createToolButtons(class ModuleToolBar *toolBar) = 0;
		virtual class SettingsInterface* settings() = 0;
		/**
		 * About data which is assigned to the module and can be accessed via \ref componentData().
		 * Default implementation returns \ref Editor::wc3libAboutData() with app and catalog name using \ref actionName().
		 * \note Assignment runs in \ref setupUi().
		 */
		virtual KAboutData moduleAboutData() const;

		class QVBoxLayout* topLayout() const;
		class QGridLayout* centerLayout() const;

		/**
		 * This event occurs when module has an \ref Editor and its opened map is changed.
		 */
		virtual void onSwitchToMap(class Map *map) = 0;

		virtual void changeEvent(QEvent *event);

		void readSettings();
		void writeSettings();

	protected slots:
		/**
		 * This slot is called whenever the module is selected in the Editor's "module" menu.
		 */
		virtual void onEditorActionTrigger();
		virtual void triggered(QAction *action);

	private:
		class MpqPriorityList *m_source;
		typedef QScopedPointer<KComponentData> ComponentDataPtr;
		ComponentDataPtr m_componentData;
		class KMenu *m_fileMenu;
		class KMenu *m_editMenu;
		class ModuleMenu *m_moduleMenu;
		class KMenuBar *m_menuBar;
		class WindowsMenu *m_windowsMenu;
		class ModuleToolBar *m_toolBar;
		class QAction *m_closeAction;
		class KAction *m_sourcesAction;

		class QVBoxLayout *m_topLayout;
		class QGridLayout *m_centerLayout;

		class SourcesDialog *m_sourcesDialog;

	private slots:
		void switchToMap(class Map *map);
};

inline class MpqPriorityList* Module::source() const
{
	return this->m_source;
}

inline KComponentData& Module::componentData()
{
	if (m_componentData.isNull())
		const_cast<Module*>(this)->m_componentData.reset(new KComponentData(moduleAboutData()));

	return *m_componentData;
}

inline const KComponentData& Module::componentData() const
{
	if (m_componentData.isNull())
		const_cast<Module*>(this)->m_componentData.reset(new KComponentData(moduleAboutData()));

	return *m_componentData;
}

inline class KMenu* Module::fileMenu() const
{
	return m_fileMenu;
}

inline class KMenu* Module::editMenu() const
{
	return m_editMenu;
}

inline class ModuleMenu* Module::moduleMenu() const
{
	return m_moduleMenu;
}

inline class KMenuBar* Module::menuBar() const
{
	return m_menuBar;
}

inline class WindowsMenu* Module::windowsMenu() const
{
	return m_windowsMenu;
}

inline class ModuleToolBar* Module::toolBar() const
{
	return m_toolBar;
}

inline class QVBoxLayout* Module::topLayout() const
{
	return this->m_topLayout;
}

inline class QGridLayout* Module::centerLayout() const
{
	return this->m_centerLayout;
}

}

}

#endif
