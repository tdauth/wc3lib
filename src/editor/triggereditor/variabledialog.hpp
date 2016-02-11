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

#ifndef WC3LIB_EDITOR_TRIGGEREDITOR_VARIABLEDIALOG_HPP
#define WC3LIB_EDITOR_TRIGGEREDITOR_VARIABLEDIALOG_HPP

#include <QDialog>

#include "../platform.hpp"
#include "../../map.hpp"
#include "ui_variabledialog.h"

namespace wc3lib
{

namespace editor
{

class TriggerEditor;

/**
 * Dialog for a single global variable of triggers.
 * Allows you to change its name, type and initial value.
 */
class VariableDialog : public QDialog, protected Ui::VariableDialog
{
	Q_OBJECT

	public:
		explicit VariableDialog(class TriggerEditor *triggerEditor, QWidget* parent = 0, Qt::WindowFlags f = 0);

		void showVariable(map::Variable *variable);

		/**
		 * Applies all settings of dialog to \p variable which means updating its name, type etc.
		 */
		void apply(map::Variable *variable);

	public slots:
		void activatedType(int index);
		void setDefaultValue(QString);

	private:
		TriggerEditor *m_triggerEditor;
};

}

}

#endif // WC3LIB_EDITOR_TRIGGEREDITOR_VARIABLEDIALOG_HPP
