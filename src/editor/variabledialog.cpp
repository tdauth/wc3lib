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

#include "variabledialog.hpp"

namespace wc3lib
{

namespace editor
{

VariableDialog::VariableDialog(QWidget *parent, Qt::WindowFlags f): QDialog(parent, f)
{
	setupUi(this);
}

void VariableDialog::showVariable(map::Variable *variable)
{
	m_nameLineEdit->setText(variable->name().c_str());
	/// TODO set combo box entries by corresponding \ref TriggerData.
	m_typeComboBox->setItemText(0, variable->type().c_str());
	m_arrayCheckBox->setEnabled(variable->isArray());
	m_startValue->setText(variable->initialValue().c_str());
}

}

}