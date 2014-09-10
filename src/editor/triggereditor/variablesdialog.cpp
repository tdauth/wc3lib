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

#include <KComboBox>
#include <KMessageBox>

#include "variablesdialog.hpp"
#include "triggereditor.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

VariablesDialog::VariablesDialog(class TriggerEditor *triggerEditor, Qt::WindowFlags f): m_triggerEditor(triggerEditor), QDialog(triggerEditor, f), m_triggers(0)
{
	setupUi(this);

	m_tableWidget->horizontalHeaderItem(0)->setText(triggerEditor->source()->sharedData()->tr("WESTRING_VARIABLENAME"));
	m_tableWidget->horizontalHeaderItem(1)->setText(triggerEditor->source()->sharedData()->tr("WESTRING_VARIABLETYPE"));
	m_tableWidget->horizontalHeaderItem(2)->setText(triggerEditor->source()->sharedData()->tr("WESTRING_INITIALVALUE"));

	connect(m_tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(variableShown(int,int)));
}

void VariablesDialog::variableShown(int row, int column)
{
	map::Variable *variable = &m_triggers->variables()[row];

	this->variableDialog()->showVariable(variable);

	if (this->variableDialog()->exec() == QDialog::Accepted) {
		this->variableDialog()->apply(variable);

		// refresh table widget row
		((QTableWidgetItem*)this->m_tableWidget->cellWidget(row, 0))->setText(variable->name().c_str());

		map::TriggerData::Type *type = variableType(*variable);

		if (type != 0) {
			((QTableWidgetItem*)this->m_tableWidget->cellWidget(row, 1))->setText(type->displayText().c_str());

		} else {
			((QTableWidgetItem*)this->m_tableWidget->cellWidget(row, 1))->setText(variable->type().c_str());
		}

		((QTableWidgetItem*)this->m_tableWidget->cellWidget(row, 2))->setText(variable->initialValue().c_str());
	}
}

void VariablesDialog::showVariables(map::Triggers *triggers)
{
	if (this->triggerEditor()->source()->sharedData()->triggerData().get() == 0) {
		KMessageBox::error(this, tr("No trigger data loaded."));

		return;
	}

	m_triggers = triggers;
	m_tableWidget->setRowCount(triggers->variables().size());

	for (int32 i = 0; i < triggers->variables().size(); ++i)
	{
		const map::Variable *variable = &triggers->variables()[i];
		QTableWidgetItem *item = new QTableWidgetItem(variable->name().c_str());

		m_tableWidget->setItem(i, 0, item);

		map::TriggerData::Type *type = variableType(*variable);

		if (type != 0) {
			item = new QTableWidgetItem(this->triggerEditor()->source()->sharedData()->tr(type->displayText().c_str()));
		} else {
			item = new QTableWidgetItem(variable->type().c_str());
		}
		m_tableWidget->setItem(i, 1, item);

		item = new QTableWidgetItem(TriggerEditor::variableInitialValueText(this->triggerEditor()->source()->sharedData().get(), *variable));
		m_tableWidget->setItem(i, 2, item);
	}
}

map::TriggerData::Type* VariablesDialog::variableType(const map::Variable &variable)
{
	map::TriggerData::Types::iterator iterator = triggerEditor()->source()->sharedData()->triggerData()->types().find(variable.type());

	if (iterator == triggerEditor()->source()->sharedData()->triggerData()->types().end())
		return 0;

	return iterator->second;
}

}

}
