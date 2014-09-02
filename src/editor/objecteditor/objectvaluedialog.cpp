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

#include "objectvaluedialog.hpp"

namespace wc3lib
{

namespace editor
{

ObjectValueDialog::ObjectValueDialog(QWidget *parent) : QDialog(parent)
{
	setupUi(this);
}

void ObjectValueDialog::setItemsVisible(bool visible)
{
	this->m_intSpinBox->setVisible(visible);
	this->m_doubleSpinBox->setVisible(visible);
	this->m_lineEdit->setVisible(visible);
	this->m_textEdit->setVisible(visible);
	this->m_comboBox->setVisible(visible);
	this->m_checkBox->setVisible(visible);
	this->m_editListWidget->setVisible(visible);
}

}

}
