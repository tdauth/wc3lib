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
#include <QtWidgets/QMessageBox>

#include "variabledialog.hpp"
#include "triggereditor.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

VariableDialog::VariableDialog(TriggerEditor *triggerEditor, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), m_triggerEditor(triggerEditor)
{
	setupUi(this);

	connect(m_typeComboBox, SIGNAL(activated(int)), this, SLOT(activatedType(int)));
	connect(m_startValueLabel, SIGNAL(linkActivated(QString)), this, SLOT(setDefaultValue(QString)));
}

void VariableDialog::activatedType(int index)
{
	const QString typeCode = m_typeComboBox->itemData(index).toString();
	map::TriggerData::Types::const_iterator iterator =  m_triggerEditor->source()->sharedData()->triggerData()->types().find(typeCode.toStdString());

	if (iterator != m_triggerEditor->source()->sharedData()->triggerData()->types().end())
	{
		const map::TriggerData::Type *type = iterator->second;

		// TODO default value, possible value etc.
		//type->

		m_startValue->setText(tr("<a href=\"type\">%1</a>").arg(type->defaultValue().c_str()));
	}
}

void VariableDialog::setDefaultValue(QString value)
{
	// TODO popup window with value selection
}


void VariableDialog::showVariable(map::Variable *variable)
{
	if (m_triggerEditor->source()->sharedData()->triggerData().get() == 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("Missing trigger data."));

		return;
	}

	m_typeComboBox->clear();

	foreach (map::TriggerData::Types::const_reference ref, m_triggerEditor->source()->sharedData()->triggerData()->types())
	{
		const map::TriggerData::Type *type = ref->second;

		if (type->canBeGlobal())
		{
			const QString typeName = this->m_triggerEditor->source()->sharedData()->tr(type->displayText().c_str());
			m_typeComboBox->addItem(typeName, type->name().c_str());
		}
	}

	m_nameLineEdit->setText(variable->name().c_str());
	const int index = m_typeComboBox->findData(variable->type().c_str());

	if (index != -1)
	{
		m_typeComboBox->setCurrentIndex(index);
	}
	else
	{
		qDebug() << "Missing type " << variable->type().c_str();
	}

	m_arrayCheckBox->setChecked(variable->isArray());
	m_arraySizeLabel->setEnabled(variable->isArray());
	m_arraySizeSpinBox->setEnabled(variable->isArray());
	m_arraySizeSpinBox->setValue(boost::numeric_cast<int>(variable->number()));
	m_startValue->setText(TriggerEditor::variableInitialValueText(m_triggerEditor->source()->sharedData().get(), *variable));
	m_deleteValuePushButton->setEnabled(!variable->initialValue().empty());
}

void VariableDialog::apply(map::Variable* variable)
{
	variable->setName(m_nameLineEdit->text().toStdString());
	variable->setType(m_typeComboBox->itemData(m_typeComboBox->currentIndex()).toString().toStdString());
	variable->setArray(m_arrayCheckBox->isChecked());
	variable->setInitialized(!m_startValue->text().isEmpty()); // TODO store initial value separately for example unit type etc.
	variable->setInitialValue(m_startValue->text().toStdString());
}

}

}