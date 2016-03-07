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
#include "../objectdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"
#include "../warcraftiiishared.hpp"

namespace wc3lib
{

namespace editor
{

ObjectValueDialog::ObjectValueDialog(QWidget *parent) : QDialog(parent), m_maximum(-1), m_source(nullptr), m_isImage(false)
{
	setupUi(this);

	connect(this->m_textEdit, SIGNAL(textChanged()), this, SLOT(limitText()));
	connect(this->m_lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(limitTextInLineEdit(const QString&)));
	connect(this->m_lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(showPreviewImage(const QString&)));
}

void ObjectValueDialog::setItemsVisible(bool visible)
{
	this->m_imageFilePreview->setVisible(visible);
	this->m_intSpinBox->setVisible(visible);
	this->m_doubleSpinBox->setVisible(visible);
	this->m_lineEdit->setVisible(visible);
	this->m_textEdit->setVisible(visible);
	this->m_comboBox->setVisible(visible);
	this->m_checkBox->setVisible(visible);

	for (CheckBoxes::iterator iterator = this->m_checkBoxes.begin(); iterator != this->m_checkBoxes.end(); ++iterator)
	{
		iterator.value()->setVisible(visible);
	}
}

int ObjectValueDialog::getValue(QString &result, const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level, ObjectData *objectData, const QString &label, QWidget *parent)
{
	qDebug() << "Original Object Id" << originalObjectId;
	qDebug() << "Custom Object Id" << customObjectId;
	qDebug() << "Field Id" << fieldId;
	const QString type = objectData->metaData()->value(fieldId, "type");
	const QString stringExt = objectData->metaData()->value(fieldId, "stringExt");
	const QString fieldValue = objectData->fieldValue(originalObjectId, customObjectId, fieldId, level);
	const QString fieldReadableValue = objectData->fieldReadableValue(originalObjectId, customObjectId, fieldId, level);
	const QString minValue = objectData->metaData()->value(fieldId, "minVal");
	const QString maxValue = objectData->metaData()->value(fieldId, "maxVal");

	if (getValue(result, type, fieldValue, fieldReadableValue, stringExt, maxValue, minValue, objectData, label, parent) == QDialog::Accepted)
	{
		objectData->modifyField(originalObjectId, customObjectId, fieldId, result);

		return QDialog::Accepted;
	}

	return QDialog::Rejected;
}

int ObjectValueDialog::getValue(QString& result, const QString& fieldType, const QString &fieldValue, const QString &fieldReadableValue, const QString& stringExt, const QString& maxValue, const QString& minValue, ObjectData* objectData, const QString& label, QWidget* parent)
{
	try
	{
		ObjectValueDialog *dialog = new ObjectValueDialog(parent);
		dialog->m_source = objectData->source();
		dialog->m_isImage = false;
		dialog->clearCheckBoxes();
		dialog->setItemsVisible(false);
		QString title = objectData->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE");
		QString titleArg = "";
		dialog->setLabelText(label);
		bool supported = true;

		bool maxOk = true;
		const int maxValueInt = maxValue.toInt(&maxOk);

		if (maxOk)
		{
			dialog->m_maximum = maxValueInt;
		}
		else
		{
			dialog->m_maximum = -1;
		}

		if (fieldType == "int")
		{
			qDebug() << "Min value:" << minValue;

			bool minOk = true;
			const int minValueInt = minValue.toInt(&minOk);

			if (minOk)
			{
				dialog->intSpinBox()->setMinimum(minValueInt);
			}
			else
			{
				qDebug() << "Invalid min value:" << minValue;
				dialog->intSpinBox()->setMinimum(0);
			}

			if (dialog->m_maximum != -1)
			{
				dialog->intSpinBox()->setMaximum(dialog->m_maximum);
			}
			else
			{
				qDebug() << "Invalid max value:" << maxValue;
				dialog->intSpinBox()->setMaximum(1000);
			}

			titleArg = objectData->source()->sharedData()->tr("WESTRING_UE_TYPE_INT");
			dialog->intSpinBox()->setVisible(true);
			dialog->intSpinBox()->setValue(fieldValue.toInt());
			dialog->intSpinBox()->selectAll();
		}
		else if (fieldType == "real" || fieldType == "unreal")
		{
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
			dialog->doubleSpinBox()->selectAll();
		}
		else if (fieldType == "string" || fieldType == "char" || fieldType == "model" || fieldType == "icon" || fieldType == "texture" || fieldType == "pathingTexture" || fieldType == "shadowTexture" || fieldType == "soundLabel")
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

			/*
			 * Limit chars to 1 by default, for example hotkeys.
			 */
			if (fieldType == "char")
			{
				dialog->m_maximum = 1;
			}

			if (fieldType == "icon" || fieldType == "texture" || fieldType == "pathingTexture" || fieldType == "shadowTexture")
			{
				dialog->m_isImage = true;
				dialog->m_imageFilePreview->setVisible(true);
				dialog->showPreviewImage(fieldValue);
			}
		}
		else if (fieldType == "bool")
		{
			titleArg = objectData->source()->sharedData()->tr("WESTRING_UE_TYPE_BOOL");
			dialog->checkBox()->setVisible(true);
			dialog->checkBox()->setChecked(fieldValue.toInt());
		}
		else if (fieldType == "abilCode")
		{
			// TODO allow standard and custom objects
			dialog->comboBox()->clear();
			dialog->comboBox()->setVisible(true);

			foreach (QString abilityId, objectData->source()->sharedData()->sharedObjectData()->abilityData()->standardObjectIds())
			{
				dialog->comboBox()->addItem(objectData->source()->sharedData()->sharedObjectData()->abilityData()->objectName(abilityId, ""), abilityId);
				// TODO set ability icon?
			}

			// we need the actual field value not the readable value to select the correct data
			dialog->comboBox()->setCurrentIndex(dialog->comboBox()->findData(fieldValue));
		}
		else if (fieldType == "unitCode")
		{
			// TODO allow standard and custom objects
			dialog->comboBox()->clear();
			dialog->comboBox()->setVisible(true);

			foreach (QString unitId, objectData->source()->sharedData()->sharedObjectData()->unitData()->standardObjectIds())
			{
				dialog->comboBox()->addItem(objectData->source()->sharedData()->sharedObjectData()->unitData()->objectName(unitId, ""), unitId);
				// TODO set ability icon?
			}

			// we need the actual field value not the readable value to select the correct data
			dialog->comboBox()->setCurrentIndex(dialog->comboBox()->findData(fieldValue));
		}
		else
		{
			const QString valueTypeDisplayString = "WESTRING_UE_TYPE_" + fieldType.toUpper();
			titleArg = objectData->source()->sharedData()->tr(valueTypeDisplayString);

			const ObjectData::ObjectTabEntries entries = objectData->objectTabEntries(fieldType);

			/*
			 * Some types need a list of check boxes if multiple values are allowed.
			 */
			if (objectData->fieldTypeAllowsMultipleSelections(fieldType))
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

				if (fieldType == "int")
				{
					value = QString::number(dialog->intSpinBox()->value());
				}
				else if (fieldType == "real" || fieldType == "unreal")
				{
					value = QString::number(dialog->doubleSpinBox()->value());
					qDebug() << "Real value:" << value;
				}
				else if (fieldType == "string" || fieldType == "char")
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
				else if (fieldType == "model" || fieldType == "icon" || fieldType == "texture" || fieldType == "pathingTexture" || fieldType == "shadowTexture")
				{
					value = dialog->lineEdit()->text();
				}
				else if (fieldType == "bool")
				{
					value = QString::number(dialog->checkBox()->isChecked());
				}
				else
				{
					/*
					 * Some types need a list of check boxes if multiple values are allowed.
					 */
					if (objectData->fieldTypeAllowsMultipleSelections(fieldType))
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
					delete dialog;
					dialog = 0;

					result = value;

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

void ObjectValueDialog::limitText()
{
	if (m_maximum != -1)
	{
		const QString &text = this->m_textEdit->toPlainText();

		if (text.size() > m_maximum)
		{
			this->m_textEdit->setText(text.mid(0, m_maximum));
		}
	}
}

void ObjectValueDialog::limitTextInLineEdit(const QString& text)
{
	if (m_maximum != -1)
	{
		if (text.size() > m_maximum)
		{
			this->m_lineEdit->setText(text.mid(0, m_maximum));
		}
	}
}

void ObjectValueDialog::showPreviewImage(const QString &filePath)
{
	qDebug() << "Show preview with " << filePath;
	const KUrl url = KUrl(MetaData::fromFilePath(filePath));
	qDebug() << "Preview URL " << url.toLocalFile();
	qDebug() << "Preview is image " << m_isImage;

	if (m_isImage && this->m_source->exists(url, this))
	{
		qDebug() << "Preview exists and is image";
		QString target;

		if (this->m_source->download(url, target, this))
		{
			this->m_imageFilePreview->setPixmap(QPixmap(target));
			this->m_source->removeTempFile(target);
		}
	}
}

}

}
