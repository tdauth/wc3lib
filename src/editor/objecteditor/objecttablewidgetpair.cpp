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

#include <KLineEdit>

#include "objecttablewidgetpair.hpp"
#include "objecteditor.hpp"
#include "objectvaluedialog.hpp"
#include "../mpqprioritylist.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableWidgetPair::ObjectTableWidgetPair(QTableWidget *tableWidget, ObjectEditorTab *tab, int row, const QString &originalObjectId, const QString &customObjectId, const QString &fieldId)
: QObject(tableWidget)
, m_tab(tab)
, m_descriptionItem(new QTableWidgetItem())
, m_valueItem(new QTableWidgetItem())
, m_originalObjectId(originalObjectId)
, m_customObjectId(customObjectId)
, m_fieldId(fieldId)
{
	const QString displayName = MetaData::fromSlkString(tab->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("displayName")));
	// cut "
	descriptionItem()->setText(tab->source()->sharedData()->tr(displayName));
	descriptionItem()->setData(Qt::UserRole, fieldId);
	valueItem()->setData(Qt::UserRole, fieldId);

	descriptionItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); // not editable!
	valueItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable); // editable!
	//descriptionItem()->setText(this->metaData()->displayName().c_str());

	tableWidget->setItem(row, 0, descriptionItem());
	tableWidget->setItem(row, 1, valueItem());
}

ObjectTableWidgetPair::~ObjectTableWidgetPair()
{
}

void ObjectTableWidgetPair::edit()
{
	try
	{
		const QString originalObjectId = this->originalObjectId();
		const QString customObjectId = this->customObjectId();
		const QString fieldId = this->fieldId();
		qDebug() << "Original Object Id" << originalObjectId;
		qDebug() << "Custom Object Id" << customObjectId;
		qDebug() << "Field Id" << fieldId;
		const QString type = MetaData::fromSlkString(tab()->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("type")));
		const QString stringExt = MetaData::fromSlkString(tab()->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("stringExt")));
		const QString fieldValue = this->tab()->objectData()->fieldValue(originalObjectId, customObjectId, fieldId);
		qDebug() << "Type:" << type;

		ObjectValueDialog *dialog = new ObjectValueDialog(this->tableWidget());
		dialog->clearCheckBoxes();
		dialog->setItemsVisible(false);
		QString title = tab()->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE");
		QString titleArg = "";
		dialog->setLabelText(QString(tr("%1:")).arg(this->descriptionItem()->text()));
		bool supported = true;

		if (type == "int")
		{
			const QString minValue = MetaData::fromSlkString(tab()->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("minVal")));
			const QString maxValue = MetaData::fromSlkString(tab()->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("maxVal")));

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

			titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_INT");
			dialog->intSpinBox()->setVisible(true);
			dialog->intSpinBox()->setValue(fieldValue.toInt());
		}
		else if (type == "real" || type == "unreal")
		{
			const QString minValue = MetaData::fromSlkString(tab()->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("minVal")));
			const QString maxValue = MetaData::fromSlkString(tab()->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("maxVal")));

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

			titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_REAL");
			dialog->doubleSpinBox()->setVisible(true);
			dialog->doubleSpinBox()->setValue(fieldValue.toDouble());
		}
		else if (type == "string")
		{
			titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_STRING");

			if (stringExt == "1")
			{
				dialog->textEdit()->setVisible(true);
				dialog->textEdit()->setText(fieldValue);
			}
			else
			{
				dialog->lineEdit()->setVisible(true);
				dialog->lineEdit()->setText(fieldValue);
			}
		}
		else if (type == "bool")
		{
			titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_BOOL");
			dialog->checkBox()->setVisible(true);
			dialog->checkBox()->setChecked(fieldValue.toInt());
		}
		else if (type == "stringList")
		{
			//titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_BOOL");
			dialog->editListWidget()->setVisible(true);

			const QStringList list = fieldValue.split(',');
			dialog->editListWidget()->setItems(list);
		}
		else
		{
			const QString valueTypeDisplayString = "WESTRING_UE_TYPE_" + type.toUpper();
			titleArg = tab()->source()->sharedData()->tr(valueTypeDisplayString);

			const ObjectData::ObjectTabEntries entries = this->tab()->objectData()->objectTabEntries(type);

			/*
			 * Some types need a list of check boxes if multiple values are allowed.
			 */
			if (tab()->objectData()->fieldTypeAllowsMultipleSelections(fieldId))
			{
				foreach (ObjectData::ObjectTabEntry entry, entries)
				{
					dialog->addCheckBox(entry.first, tab()->source()->sharedData()->tr(entry.second));
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
					dialog->comboBox()->addItem(tab()->source()->sharedData()->tr(entry.second), entry.first);
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
				else if (type == "stringList")
				{
					value = dialog->editListWidget()->items().join(QChar(','));
					qDebug() << "String list value:" << value;
				}
				else
				{
					/*
					 * Some types need a list of check boxes if multiple values are allowed.
					 */
					if (tab()->objectData()->fieldTypeAllowsMultipleSelections(fieldId))
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
					this->descriptionItem()->setForeground(Qt::magenta);
					this->valueItem()->setForeground(Qt::magenta);

					this->tab()->objectData()->modifyField(this->originalObjectId(), this->customObjectId(), this->fieldId(), value);

					valueItem()->setText(this->tab()->objectData()->fieldReadableValue(originalObjectId, customObjectId, fieldId));
				}
			}
		}

		delete dialog;
		dialog = 0;
	}
	catch (Exception &e)
	{
		QMessageBox::warning(this->tableWidget(), tr("Error"), e.what());
	}
}

QString ObjectTableWidgetPair::defaultValue() const
{
	return this->tab()->objectData()->defaultFieldValue(this->originalObjectId(), this->fieldId());
}

void ObjectTableWidgetPair::reset()
{
	const QString dataValue = this->defaultValue();
	this->valueItem()->setText(dataValue);
	this->descriptionItem()->setForeground(Qt::black);
	this->valueItem()->setForeground(Qt::black);
	this->tab()->objectData()->resetField(this->originalObjectId(), this->customObjectId(), this->fieldId());
}

}

}
