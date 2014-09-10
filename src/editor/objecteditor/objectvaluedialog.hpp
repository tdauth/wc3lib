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
#include <QCheckBox>
#include <QHash>

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

/**
 * \brief GUI dialog which allows selection of an field value for a specific object.
 *
 * The shown input widget depends on the type of the corresponding field.
 * The dialog supports numeric types as well as text, check boxes or combo boxes.
 */
class ObjectValueDialog : public QDialog, protected Ui::ObjectValueDialog
{
	public:
		typedef QHash<QString, QCheckBox*> CheckBoxes;

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

		void addCheckBox(const QString &name, const QString &value);
		void setCheckBoxChecked(const QString &name, bool checked);
		void clearCheckBoxes();
		QString checkedTexts() const;
		QString checkedValues() const;

	private:
		CheckBoxes m_checkBoxes;
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

inline void ObjectValueDialog::addCheckBox(const QString& name, const QString& value)
{
	QCheckBox *checkBox = new QCheckBox(value, this);
	this->verticalLayout->addWidget(checkBox);
	this->m_checkBoxes.insert(name, checkBox);
}

inline void ObjectValueDialog::setCheckBoxChecked(const QString& name, bool checked)
{
	CheckBoxes::iterator iterator = this->m_checkBoxes.find(name);

	if (iterator != this->m_checkBoxes.end())
	{
		iterator.value()->setChecked(checked);
	}
}

inline void ObjectValueDialog::clearCheckBoxes()
{
	for (CheckBoxes::iterator iterator = this->m_checkBoxes.begin(); iterator != this->m_checkBoxes.end(); ++iterator)
	{
		delete iterator.value();
	}

	this->m_checkBoxes.clear();
}

inline QString ObjectValueDialog::checkedTexts() const
{
	QString result;

	for (CheckBoxes::const_iterator iterator = this->m_checkBoxes.begin(); iterator != this->m_checkBoxes.end(); ++iterator)
	{
		if (iterator.value()->isChecked())
		{
			if (!result.isEmpty())
			{
				result += ",";
			}

			result += iterator.value()->text();
		}
	}

	return result;
}

inline QString ObjectValueDialog::checkedValues() const
{
	QString result;

	for (CheckBoxes::const_iterator iterator = this->m_checkBoxes.begin(); iterator != this->m_checkBoxes.end(); ++iterator)
	{
		if (iterator.value()->isChecked())
		{
			if (!result.isEmpty())
			{
				result += ",";
			}

			result += iterator.key();
		}
	}

	return result;
}


}

}

#endif
