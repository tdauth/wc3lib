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

#ifndef WC3LIB_EDITOR_TRIGGEREDITOR_VARIABLESDIALOG_HPP
#define WC3LIB_EDITOR_TRIGGEREDITOR_VARIABLESDIALOG_HPP

#include <QDialog>

#include "../platform.hpp"
#include "ui_variablesdialog.h"
#include "variabledialog.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Simple modal dialog which allows users to edit global variables of triggers.
 */
class VariablesDialog : public QDialog, protected Ui::VariablesDialog
{
	Q_OBJECT

	public:
		explicit VariablesDialog(class TriggerEditor *triggerEditor, Qt::WindowFlags f = 0);

		class TriggerEditor* triggerEditor() const;

		/**
		 * Fills dialog with list of global variables from \p triggers.
		 */
		void showVariables(map::Triggers *triggers);

		map::TriggerData::Type* variableType(const map::Variable &variable);

		VariableDialog* variableDialog() const;

	public slots:
		void variableShown(int,int);

	private:
		class TriggerEditor *m_triggerEditor;
		map::Triggers *m_triggers;
		VariableDialog *m_variableDialog;


};

inline class TriggerEditor* VariablesDialog::triggerEditor() const
{
	return this->m_triggerEditor;
}

inline VariableDialog* VariablesDialog::variableDialog() const
{
	if (m_variableDialog == 0) {
		const_cast<VariablesDialog*>(this)->m_variableDialog = new VariableDialog(this->triggerEditor(), const_cast<VariablesDialog*>(this));
	}

	return m_variableDialog;
}

}

}

#endif
