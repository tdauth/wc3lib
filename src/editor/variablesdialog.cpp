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

#include "variablesdialog.hpp"
#include "triggereditor.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

VariablesDialog::VariablesDialog(class TriggerEditor *triggerEditor, Qt::WindowFlags f): m_triggerEditor(triggerEditor), QDialog(triggerEditor, f)
{
	setupUi(this);
}

void VariablesDialog::showVariables(map::Triggers *triggers)
{
	m_tableWidget->setRowCount(triggers->variables().size());

	for (map::int32 i = 0; i < triggers->variables().size(); ++i)
	{
		QTableWidgetItem *item = new QTableWidgetItem(triggers->variables()[i]->name().c_str());
		m_tableWidget->setItem(i, 0, item);
		//map::TriggerData::Type *type = variableType(*ref.second.get());
		item = new QTableWidgetItem(triggers->variables()[i]->type().c_str());
		m_tableWidget->setItem(i, 1, item);
		item = new QTableWidgetItem(triggers->variables()[i]->initialValue().c_str());
		m_tableWidget->setItem(i, 2, item);
	}
}

map::TriggerData::Type* VariablesDialog::variableType(const wc3lib::map::Variable &variable) const
{
	BOOST_FOREACH(map::TriggerData::Types::left_const_reference ref, const_cast<const map::TriggerData*>(triggerEditor()->source()->triggerData().get())->types().left)
	{
		if (ref.second->name() == variable.type())
			return ref.second.get();
	}

	return 0;
}

}

}
