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

#include <QMessageBox>

#include "objecttablewidget.hpp"
#include "objecttablewidgetpair.hpp"
#include "objectvaluedialog.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableWidget::ObjectTableWidget(ObjectEditorTab *parent) : QTableWidget(parent->metaData()->slk().rows(), 2, parent), m_tab(parent)
{
	const QString name = parent->source()->sharedData()->tr("WESTRING_NAME");
	QTableWidgetItem *nameItem = new QTableWidgetItem(name);
	this->setHorizontalHeaderItem(0, nameItem);

	const QString value = parent->source()->sharedData()->tr("WESTRING_VALUE");
	QTableWidgetItem *valueItem = new QTableWidgetItem(value);
	this->setHorizontalHeaderItem(1, valueItem);

	for (map::Slk::Table::size_type i = 0; i < tab()->metaData()->slk().rows() -  1; ++i)
	{
		QString fieldId = tab()->metaData()->value(i + 1, "\"ID\"");
		fieldId = fieldId.mid(1, fieldId.size() - 2);
		ObjectTableWidgetPair *pair = new ObjectTableWidgetPair(this, parent, i,  "", fieldId);
		m_pairs.insert(fieldId, pair);
		qDebug() << "pair " << i;
	}

	m_contextMenu = new QMenu(this);
	const QString modifyField = parent->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_MODIFY");
	const QString resetField = parent->source()->sharedData()->tr("WESTRING_FIELDLIST_CM_RESET");
	m_modifyField = new QAction(modifyField, this);
	m_resetField = new QAction(resetField, this);
	m_contextMenu->addAction(m_modifyField);
	m_contextMenu->addAction(m_resetField);

	connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(editItem(QTableWidgetItem*)));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));

	this->setShowGrid(false);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setEditTriggers(QTableWidget::NoEditTriggers);
	this->setSelectionBehavior(QTreeWidget::SelectRows);
}

void ObjectTableWidget::editItem(QTableWidgetItem *item)
{
	const QString type = tab()->metaData()->value(item->row(), "\"type\"");

	if (type == "\"int\"" || type == "\"unit\"")
	{
		ObjectValueDialog *dialog = new ObjectValueDialog(this);
		dialog->setItemsVisible(false);
		dialog->intSpinBox()->setVisible(true);
		QString title = tab()->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE").arg(tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_INT"));
		dialog->setWindowTitle(title);
		dialog->setLabelText(this->item(item->row(), 0)->text());

		if (dialog->exec() == QDialog::Accepted)
		{
			item->setText(QString::number(dialog->intSpinBox()->value()));
		}
	}
	else if (type == "\"real\"" || type == "\"unreal\"")
	{
		ObjectValueDialog *dialog = new ObjectValueDialog(this);
		dialog->setItemsVisible(false);
		dialog->doubleSpinBox()->setVisible(true);
		QString title = tab()->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE").arg(tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_REAL"));
		dialog->setWindowTitle(title);
		dialog->setLabelText(this->item(item->row(), 0)->text());

		if (dialog->exec() == QDialog::Accepted)
		{
			item->setText(QString::number(dialog->intSpinBox()->value()));
		}
	}
	else if (type == "\"string\"")
	{
		ObjectValueDialog *dialog = new ObjectValueDialog(this);
		dialog->setItemsVisible(false);
		dialog->lineEdit()->setVisible(true);
		QString title = tab()->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE").arg(tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_STRING"));
		dialog->setWindowTitle(title);
		dialog->setLabelText(this->item(item->row(), 0)->text());
		dialog->lineEdit()->setText(item->text());

		if (dialog->exec() == QDialog::Accepted)
		{
			item->setText(dialog->lineEdit()->text());
		}
	}
	else if (type == "\"bool\"")
	{
		ObjectValueDialog *dialog = new ObjectValueDialog(this);
		dialog->setItemsVisible(false);
		dialog->lineEdit()->setVisible(true);
		QString title = tab()->source()->sharedData()->tr("WESTRING_UE_DLG_EDITVALUE").arg(tab()->source()->sharedData()->tr("WESTRING_UE_TYPE_BOOL"));
		dialog->setWindowTitle(title);
		dialog->setLabelText(this->item(item->row(), 0)->text());
		dialog->checkBox()->setChecked(item->text().toInt());

		if (dialog->exec() == QDialog::Accepted)
		{
			item->setText(QString::number(dialog->checkBox()->isChecked()));
		}
	}
	else
	{
		QMessageBox::warning(this, tr("Error"), tr("Unsupported type."));
	}

	/*
	foreach (ObjectTableWidgetPair *pair, m_pairs)
	{
		if (pair->valueItem() == item)
		{
			switch (pair->metaData()->type())
			{
				case map::Value::Type::Integer:
				{
					ObjectIntegerDialog *dialog = new ObjectIntegerDialog(this);
					dialog->setCaption(pair->metaData()->displayName().c_str());
					dialog->intSpinBox()->setMaximum(pair->metaData()->maxValue().toInteger());
					dialog->intSpinBox()->setMinimum(pair->metaData()->minValue().toInteger());
					dialog->intSpinBox()->setValue(pair->data()->left.find(pair->metaData())->second.toInteger());

					if (dialog->exec() == QDialog::Accepted)
						pair->setCustomValue(map::Value(boost::numeric_cast<int32>(dialog->intSpinBox()->value())));

					break;
				}

				case map::Value::Type::Real:
					break;

				case map::Value::Type::Unreal:
					break;

				case map::Value::Type::String:
					break;

				case map::Value::Type::Boolean:
					break;

				case map::Value::Type::Character:
					break;

				case map::Value::Type::UnitList:
					break;

				case map::Value::Type::ItemList:
					break;

				case map::Value::Type::RegenerationType:
					break;

				case map::Value::Type::AttackType:
					break;

				case map::Value::Type::WeaponType:
					break;

				case map::Value::Type::TargetType:
					break;

				case map::Value::Type::MoveType:
					break;

				case map::Value::Type::DefenseType:
					break;

				case map::Value::Type::PathingTexture:
					break;

				case map::Value::Type::UpgradeList:
					break;

				case map::Value::Type::StringList:
					break;

				case map::Value::Type::AbilityList:
					break;

				case map::Value::Type::HeroAbilityList:
					break;

				case map::Value::Type::MissileArt:
					break;

				case map::Value::Type::AttributeType:
					break;

				case map::Value::Type::AttackBits:
					break;
			}

			break;
		}
	}
	*/
}

void ObjectTableWidget::customContextMenuRequested(QPoint pos)
{
	qDebug() << "custom context menu";
	m_contextMenu->popup(this->viewport()->mapToGlobal(pos));
}

}

}
