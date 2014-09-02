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

#ifndef WC3LIB_EDITOR_OBJECTVALUEDIALOG_HPP
#define WC3LIB_EDITOR_OBJECTVALUEDIALOG_HPP

#include <QDialog>
#include <QDoubleSpinBox>

#include <KIntSpinBox>
#include <KLineEdit>
#include <KTextEdit>
#include <KComboBox>
#include <KEditListWidget>

#include "ui_objectvaluedialog.h"

namespace wc3lib
{

namespace editor
{

class ObjectValueDialog : public QDialog, protected Ui::ObjectValueDialog
{
	public:
		ObjectValueDialog(QWidget *parent = 0);

		void setLabelText(const QString &text);

		void setItemsVisible(bool visible);

		KIntSpinBox* intSpinBox() const;
		QDoubleSpinBox* doubleSpinBox() const;
		QLineEdit* lineEdit() const;
		KTextEdit* textEdit() const;
		KComboBox* comboBox() const;
		QCheckBox* checkBox() const;
		KEditListWidget* editListWidget() const;
};

inline void ObjectValueDialog::setLabelText(const QString& text)
{
	this->m_label->setText(text);
}

inline KIntSpinBox* ObjectValueDialog::intSpinBox() const
{
	return this->m_intSpinBox;
}

inline QDoubleSpinBox* ObjectValueDialog::doubleSpinBox() const
{
	return this->m_doubleSpinBox;
}

inline QLineEdit* ObjectValueDialog::lineEdit() const
{
	return this->m_lineEdit;
}

inline KTextEdit* ObjectValueDialog::textEdit() const
{
	return this->m_textEdit;
}

inline KComboBox* ObjectValueDialog::comboBox() const
{
	return this->m_comboBox;
}

inline QCheckBox* ObjectValueDialog::checkBox() const
{
	return this->m_checkBox;
}

inline KEditListWidget* ObjectValueDialog::editListWidget() const
{
	return this->m_editListWidget;
}

}

}

#endif
