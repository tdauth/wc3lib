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

class ObjectData;
class MpqPriorityList;

/**
 * \brief GUI dialog which allows selection of an field value for a specific object.
 *
 * The shown input widget depends on the type of the corresponding field.
 * The dialog supports numeric types as well as text, check boxes or combo boxes.
 *
 * Use \ref getValue() to retrieve a new field value from a newly shown object value dialog.
 *
 * \note For list of objects use \ref ObjectListDialog.
 *
 * \ingroup objectdata
 * \sa ObjectListDialog
 */
class ObjectValueDialog : public QDialog, protected Ui::ObjectValueDialog
{
	Q_OBJECT

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

		void addCheckBox(const QString &name, const QString &value);
		void setCheckBoxChecked(const QString &name, bool checked);
		void clearCheckBoxes();
		QString checkedTexts() const;
		QString checkedValues() const;

		/**
		 * Shows an object value dialog for the object with IDs \p originalObjectId and \p customObjectId and for its field \p fieldId which allows the user to select
		 * a new value for the corresponding field.
		 * If the selection was successfull the function returns QDialog::DialogCode::Accepted.
		 *
		 * On success it automatically modifies the field with ID \p fieldId in \p objectData.
		 *
		 * \param result Sets the result to the value selected in the dialog.
		 * \param originalObjectId The original ID of the object.
		 * \param customObjectId The custom ID of the object.
		 * \param fieldId The ID of the field which is modified by the dialog.
		 * \param level The level of the field value which is modified by the dialog.
		 * \param objectData The corresponding object data which contains all modifications and default values.
		 * \param label The dialog's label.
		 * \param parent The dialog's parent widget.
		 *
		 * \return Returns the dialog code QDialog::DialogCode.
		 */
		static int getValue(QString &result, const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level, ObjectData *objectData, const QString &label, QWidget *parent = 0);

		static int getValue(QString &result, const QString &fieldType, const QString &fieldValue, const QString &fieldReadableValue, const QString &stringExt, const QString &maxValue, const QString &minValue, ObjectData *objectData, const QString &label, QWidget *parent = 0);

	private:
		CheckBoxes m_checkBoxes;
		int m_maximum;
		MpqPriorityList *m_source;
		/**
		 * Stores if the field type is an icon and therefore shows a preview image.
		 */
		bool m_isImage;

	private slots:
		void limitText();
		void limitTextInLineEdit(const QString &text);
		/**
		 * This slot is called whenever the file path of an icon changes to update the preview icon.
		 */
		void showPreviewImage(const QString &filePath);
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
