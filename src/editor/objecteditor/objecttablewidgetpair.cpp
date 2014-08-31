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
	const QString displayName = MetaData::fromSlkString(tab->metaData()->metaData()->value(row + 1, MetaData::toSlkString("displayName")));
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
		const QString type = MetaData::fromSlkString(tab()->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("type")));
		const QString stringExt = MetaData::fromSlkString(tab()->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("stringExt")));
		qDebug() << "Type:" << type;

		ObjectValueDialog *dialog = new ObjectValueDialog(this->tableWidget());
		dialog->setItemsVisible(false);
		QString title = tab()->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE");
		QString titleArg = "";
		dialog->setLabelText(QString(tr("%1:")).arg(this->descriptionItem()->text()));
		bool supported = true;

		if (type == "int" || type == "unit")
		{
			const QString minValue = MetaData::fromSlkString(tab()->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("minVal")));
			const QString maxValue = MetaData::fromSlkString(tab()->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("maxVal")));

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
			dialog->intSpinBox()->setValue(valueItem()->text().toInt());
		}
		else if (type == "real" || type == "unreal")
		{
			const QString minValue = MetaData::fromSlkString(tab()->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("minVal")));
			const QString maxValue = MetaData::fromSlkString(tab()->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("maxVal")));

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
			dialog->doubleSpinBox()->setValue(valueItem()->text().toDouble());
		}
		else if (type == "string")
		{
			titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_STRING");

			if (stringExt == "1")
			{
				dialog->textEdit()->setVisible(true);
				dialog->textEdit()->setText(valueItem()->text());
			}
			else
			{
				dialog->lineEdit()->setVisible(true);
				dialog->lineEdit()->setText(valueItem()->text());
			}
		}
		else if (type == "bool")
		{
			titleArg = tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_BOOL");
			dialog->checkBox()->setVisible(true);
			dialog->checkBox()->setChecked(valueItem()->text().toInt());
		}
		else
		{
			supported = false;
			QMessageBox::warning(this->tableWidget(), tr("Error"), tr("Unsupported type."));
		}

		if (supported)
		{
			title.sprintf(title.toUtf8().constData(), titleArg.toUtf8().constData());
			dialog->setWindowTitle(title);
			dialog->resize(dialog->minimumSize());

			if (dialog->exec() == QDialog::Accepted)
			{
				bool successOnEdit = true;

				if (type == "int" || type == "unit")
				{
					valueItem()->setText(QString::number(dialog->intSpinBox()->value()));
				}
				else if (type == "real" || type == "unreal")
				{
					valueItem()->setText(QString::number(dialog->doubleSpinBox()->value()));
				}
				else if (type == "string")
				{
					if (stringExt == "1")
					{
						valueItem()->setText(dialog->textEdit()->toPlainText());
					}
					else
					{
						valueItem()->setText(dialog->lineEdit()->text());
					}
				}
				else if (type == "bool")
				{
					valueItem()->setText(QString::number(dialog->checkBox()->isChecked()));
				}
				else
				{
					QMessageBox::warning(this->tableWidget(), tr("Error"), tr("Unsupported type."));
					successOnEdit = false;
				}

				if (successOnEdit)
				{
					this->descriptionItem()->setForeground(Qt::magenta);
					this->valueItem()->setForeground(Qt::magenta);

					this->tab()->modifyField(this->originalObjectId(), this->customObjectId(), this->fieldId(), this->modification());
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

map::Value ObjectTableWidgetPair::customValue() const
{
	return value(this->valueItem()->text());
}

map::Value ObjectTableWidgetPair::defaultValue() const
{
	const QString dataValue = this->tab()->metaData()->getDataValue(this->originalObjectId(), this->fieldId());


	return value(dataValue);
}

map::Value ObjectTableWidgetPair::value(const QString& text) const
{
	const map::Value::Type type = this->tab()->metaData()->fieldType(this->fieldId());

	switch (type)
	{
		case map::Value::Type::Integer:
		{
			int32 data = boost::numeric_cast<int32>(text.toInt());

			return map::Value(data);
		}
	}

	return map::Value(0);
}

void ObjectTableWidgetPair::reset()
{
	const QString dataValue = this->tab()->metaData()->getDataValue(this->originalObjectId(), this->fieldId());
	this->valueItem()->setText(dataValue);
	this->descriptionItem()->setForeground(Qt::black);
	this->valueItem()->setForeground(Qt::black);
	this->tab()->resetField(this->originalObjectId(), this->customObjectId(), this->fieldId());
}

map::CustomUnits::Modification ObjectTableWidgetPair::modification() const
{
	// TODO suppoert map::CustomObjects::Modification
	map::CustomUnits::Modification modification;
	modification.setValueId(map::stringToId(this->fieldId().toStdString()));
	modification.value() = this->customValue();

	return modification;
}


}

}
