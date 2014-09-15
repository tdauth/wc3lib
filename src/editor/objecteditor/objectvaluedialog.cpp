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

#include "objectvaluedialog.hpp"
#include "objectdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"
#include "../warcraftiiishared.hpp"

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

	for (CheckBoxes::iterator iterator = this->m_checkBoxes.begin(); iterator != this->m_checkBoxes.end(); ++iterator)
	{
		iterator.value()->setVisible(visible);
	}
}

QDialog::DialogCode ObjectValueDialog::show(QString &result, const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, ObjectData *objectData, const QString &label, QWidget *parent)
{
	try
	{
		qDebug() << "Original Object Id" << originalObjectId;
		qDebug() << "Custom Object Id" << customObjectId;
		qDebug() << "Field Id" << fieldId;
		const QString type = objectData->metaData()->value(fieldId, "type");
		const QString stringExt = objectData->metaData()->value(fieldId, "stringExt");
		const QString fieldValue = objectData->fieldValue(originalObjectId, customObjectId, fieldId);
		qDebug() << "Type:" << type;

		ObjectValueDialog *dialog = new ObjectValueDialog(parent);
		dialog->clearCheckBoxes();
		dialog->setItemsVisible(false);
		QString title = objectData->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE");
		QString titleArg = "";
		dialog->setLabelText(label);
		bool supported = true;

		if (type == "int")
		{
			const QString minValue = objectData->metaData()->value(fieldId, "minVal");
			const QString maxValue = objectData->metaData()->value(fieldId, "maxVal");

			qDebug() << "Min value:" << minValue;
			qDebug() << "Max value:" << maxValue;

			bool minOk = true;
			const int minValueInt = minValue.toInt(&minOk);
			bool maxOk = true;
			const int maxValueInt = maxValue.toInt(&maxOk);

			if (minOk)
			{
				dialog->intSpinBox()->setMinimum(minValueInt);
			}
			else
			{
				qDebug() << "Invalid min value:" << minValue;
				dialog->intSpinBox()->setMinimum(0);
			}

			if (maxOk)
			{
				dialog->intSpinBox()->setMaximum(maxValueInt);
			}
			else
			{
				qDebug() << "Invalid max value:" << maxValue;
				dialog->intSpinBox()->setMaximum(1000);
			}

			titleArg = objectData->source()->sharedData()->tr("WESTRING_UE_TYPE_INT");
			dialog->intSpinBox()->setVisible(true);
			dialog->intSpinBox()->setValue(fieldValue.toInt());
		}
		else if (type == "real" || type == "unreal")
		{
			const QString minValue = objectData->metaData()->value(fieldId, "minVal");
			const QString maxValue = objectData->metaData()->value(fieldId, "maxVal");

			qDebug() << "Min value:" << minValue;
			qDebug() << "Max value:" << maxValue;

			bool minOk = true;
			const float32 minValueDouble = minValue.toDouble(&minOk);
			bool maxOk = true;
			const float32 maxValueDouble = maxValue.toDouble(&maxOk);

			if (minOk)
			{
				dialog->doubleSpinBox()->setMinimum(minValueDouble);
			}
			else
			{
				qDebug() << "Invalid min value:" << minValue;
				dialog->doubleSpinBox()->setMinimum(0);
			}

			if (maxOk)
			{
				dialog->doubleSpinBox()->setMaximum(maxValueDouble);
			}
			else
			{
				qDebug() << "Invalid max value:" << maxValue;
				dialog->doubleSpinBox()->setMaximum(1000);
			}

			titleArg = objectData->source()->sharedData()->tr("WESTRING_UE_TYPE_REAL");
			dialog->doubleSpinBox()->setVisible(true);
			dialog->doubleSpinBox()->setValue(fieldValue.toDouble());
		}
		else if (type == "string")
		{
			titleArg = objectData->source()->sharedData()->tr("WESTRING_UE_TYPE_STRING");

			if (stringExt == "1")
			{
				dialog->textEdit()->setVisible(true);
				dialog->textEdit()->setText(fieldValue);
				dialog->textEdit()->selectAll();
			}
			else
			{
				dialog->lineEdit()->setVisible(true);
				dialog->lineEdit()->setText(fieldValue);
				dialog->lineEdit()->selectAll();
			}
		}
		else if (type == "bool")
		{
			titleArg = objectData->source()->sharedData()->tr("WESTRING_UE_TYPE_BOOL");
			dialog->checkBox()->setVisible(true);
			dialog->checkBox()->setChecked(fieldValue.toInt());
		}
		else if (objectData->fieldTypeIsList(type))
		{
			//titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_BOOL");
			const QString fieldReadableValue = objectData->fieldReadableValue(originalObjectId, customObjectId, fieldId);
			dialog->editListWidget()->setVisible(true);

			const QStringList list = fieldReadableValue.split(", ");
			dialog->editListWidget()->setItems(list);
		}
		else
		{
			const QString valueTypeDisplayString = "WESTRING_UE_TYPE_" + type.toUpper();
			titleArg = objectData->source()->sharedData()->tr(valueTypeDisplayString);

			const ObjectData::ObjectTabEntries entries = objectData->objectTabEntries(type);

			/*
			 * Some types need a list of check boxes if multiple values are allowed.
			 */
			if (objectData->fieldTypeAllowsMultipleSelections(fieldId))
			{
				foreach (ObjectData::ObjectTabEntry entry, entries)
				{
					dialog->addCheckBox(entry.first, objectData->source()->sharedData()->tr(entry.second));
				}

				// we need the actual field value not the readable value to select the correct data
				const QStringList fieldValues = fieldValue.split(',');

				foreach (QString value, fieldValues)
				{
					dialog->setCheckBoxChecked(value, true);
				}
			}
			/*
			 * Other types need a combo box if only one specific type can be selected.
			 */
			else
			{
				dialog->comboBox()->clear();
				dialog->comboBox()->setVisible(true);

				foreach (ObjectData::ObjectTabEntry entry, entries)
				{
					dialog->comboBox()->addItem(objectData->source()->sharedData()->tr(entry.second), entry.first);
				}

				// we need the actual field value not the readable value to select the correct data
				dialog->comboBox()->setCurrentIndex(dialog->comboBox()->findData(fieldValue));
			}
		}

		if (supported)
		{
			title.sprintf(title.toUtf8().constData(), titleArg.toUtf8().constData());
			dialog->setWindowTitle(title);
			dialog->resize(dialog->minimumSize());

			if (dialog->exec() == QDialog::Accepted)
			{
				bool successOnEdit = true;
				QString value;

				if (type == "int")
				{
					value = QString::number(dialog->intSpinBox()->value());
				}
				else if (type == "real" || type == "unreal")
				{
					value = QString::number(dialog->doubleSpinBox()->value());
					qDebug() << "Real value:" << value;
				}
				else if (type == "string")
				{
					if (stringExt == "1")
					{
						value = dialog->textEdit()->toPlainText();
					}
					else
					{
						value = dialog->lineEdit()->text();
					}
				}
				else if (type == "bool")
				{
					value = QString::number(dialog->checkBox()->isChecked());
				}
				else if (objectData->fieldTypeIsList(type))
				{
					// TODO join the raw data ids not the displayed text!!!
					value = dialog->editListWidget()->items().join(QChar(','));
					qDebug() << "String list value:" << value;
				}
				else
				{
					/*
					 * Some types need a list of check boxes if multiple values are allowed.
					 */
					if (objectData->fieldTypeAllowsMultipleSelections(fieldId))
					{
						value = dialog->checkedValues();
					}
					/*
					 * Other types need a combo box if only one specific type can be selected.
					 */
					else
					{
						value = dialog->comboBox()->itemData(dialog->comboBox()->currentIndex()).toString();
					}

					qDebug() << "Value: " << value;


					// dialog->comboBox()->itemData(dialog->comboBox()->currentIndex()
					//if (type == "attackBits")
					//{
					// TODO resolve all types
						//value = map::Value(
				}

				if (successOnEdit)
				{
					objectData->modifyField(originalObjectId, customObjectId, fieldId, value);
					delete dialog;
					dialog = 0;

					result = objectData->fieldReadableValue(originalObjectId, customObjectId, fieldId);

					return QDialog::Accepted;
				}
			}
		}

		delete dialog;
		dialog = 0;
	}
	catch (Exception &e)
	{
		QMessageBox::warning(parent, tr("Error"), e.what());
	}

	return QDialog::Rejected;
}

}

}
