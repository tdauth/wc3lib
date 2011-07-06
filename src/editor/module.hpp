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

#include <QWidget>

#include "../core.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Abstract class for module implementation such as model or terrain editors.
 * Implement the pure virtual functions to customize your module's menu, actions and tool buttons.
 * \note Modules should work independently without an Editor instance. They only need a data source.
 */
class Module : public QWidget
{
	Q_OBJECT

	public:
		Module(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~Module();
		class MpqPriorityList* source() const;
		class KMenu* fileMenu() const;
		class KMenu* editMenu() const;
		class ModuleMenu* moduleMenu() const; 
		class KMenuBar* menuBar() const;
		class KMenu* windowsMenu() const;
		class KToolBar* toolBar() const;
		
		/**
		 * This function uses dynamic type checking via "typeid".
		 * \return Returns true if \ref source() is an Editor instance. Otherwise it returns false.
		 * \sa source(), editor()
		 */
		bool hasEditor() const;
		/**
		 * \return Returns casted Editor instance (from \ref source()).
		 * \throw Exception Throws an exception when \ref hasEditor() returns false and therefore source isn't an Editor instance.
		 * \sa source(), hasEditor()
		 */
		class Editor* editor() const throw (Exception);

	protected:
		virtual void setupUi();

		virtual void createFileActions(class KMenu *menu) = 0;
		virtual void createEditActions(class KMenu *menu) = 0;
		virtual void createMenus(class KMenuBar *menuBar) = 0;
		virtual void createWindowsActions(class KMenu *menu) = 0;
		virtual void createToolButtons(class KToolBar *toolBar) = 0;
		virtual class SettingsInterface* settings() = 0;

		class QVBoxLayout* topLayout() const;

		void readSettings();
		void writeSettings();

	protected slots:
		virtual void triggered(QAction *action);

	private:
		class MpqPriorityList *m_source;
		class KMenu *m_fileMenu;
		class KMenu *m_editMenu;
		class ModuleMenu *m_moduleMenu;
		class KMenuBar *m_menuBar;
		class KMenu *m_windowsMenu;
		class KToolBar *m_toolBar;

		class QVBoxLayout *m_topLayout;
};

inline class MpqPriorityList* Module::source() const
{
	return this->m_source;
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

inline class KMenu* Module::windowsMenu() const
{
	return m_windowsMenu;
}

inline class KToolBar* Module::toolBar() const
{
	return m_toolBar;
}

inline class QVBoxLayout* Module::topLayout() const
{
	return this->m_topLayout;
}

}

}

#endif
