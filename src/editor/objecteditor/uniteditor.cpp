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

#include <KMessageBox>

#include "uniteditor.hpp"
#include "objecttreewidget.hpp"
#include "objecttablewidget.hpp"
#include "objecttablewidgetpair.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

UnitEditor::UnitEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, new MetaData(KUrl("Units/UnitMetaData.slk")), parent, f)
, m_standardUnitsItem(0)
, m_customUnitsItem(0)
, m_humanItem(0)
, m_orcItem(0)
, m_undeadItem(0)
, m_nightElfItem(0)
, m_neutralNagaItem(0)
, m_neutralHostileItem(0)
, m_neutralPassiveItem(0)
, m_unitData(0)
, m_unitUi(0)
{
	this->m_unitData = new MetaData(KUrl("Units/UnitData.slk"));
	this->m_unitData->setSource(this->source());

	try
	{
		this->m_unitData->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(this, i18n("Error on loading file \"%1\": %2", this->m_unitData->url().toEncoded().constData(), e.what()));
	}

	this->m_unitUi = new MetaData(KUrl("Units/unitUI.slk"));
	m_unitUi->setSource(this->source());

	try
	{
		this->m_unitUi->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(this, i18n("Error on loading file \"%1\": %2", this->m_unitUi->url().toEncoded().constData(), e.what()));
	}

	this->m_humanUnitStrings = new MetaData(KUrl("Units/HumanUnitStrings.txt"));
	m_humanUnitStrings->setSource(this->source());

	try
	{
		this->m_humanUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(this, i18n("Error on loading file \"%1\": %2", this->m_humanUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_humanUnitFunc = new MetaData(KUrl("Units/HumanUnitFunc.txt"));
	this->m_humanUnitFunc->setSource(this->source());

	try
	{
		this->m_humanUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(this, i18n("Error on loading file \"%1\": %2", this->m_humanUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_orcUnitStrings = new MetaData(KUrl("Units/OrcUnitStrings.txt"));
	m_orcUnitStrings->setSource(this->source());

	try
	{
		this->m_orcUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(this, i18n("Error on loading file \"%1\": %2", this->m_orcUnitStrings->url().toEncoded().constData(), e.what()));
	}

	setupUi();
}

UnitEditor::~UnitEditor()
{
	delete this->m_humanUnitStrings;
}

class ObjectTreeWidget* UnitEditor::createTreeWidget()
{
	ObjectTreeWidget *treeWidget = new ObjectTreeWidget(this);
	treeWidget->setHeaderLabel(source()->sharedData()->tr("WESTRING_OBJTAB_UNITS"));

	QList<QTreeWidgetItem*> topLevelItems;
	this->m_standardUnitsItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_UE_STANDARDUNITS")), 0);
	this->m_customUnitsItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_UE_CUSTOMUNITS")), 0);
	topLevelItems << m_standardUnitsItem;
	topLevelItems << m_customUnitsItem;

	treeWidget->addTopLevelItems(topLevelItems);

	// Races
	m_humanItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_HUMAN")), 0);
	m_orcItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_ORC")), 0);
	m_undeadItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_UNDEAD")), 0);
	m_nightElfItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_NIGHTELF")), 0);
	m_neutralNagaItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_NEUTRAL_NAGA")), 0);
	m_neutralHostileItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_HOSTILE")), 0);
	m_neutralPassiveItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_PASSIVE")), 0);

	m_standardUnitsItem->addChild(m_humanItem);
	m_standardUnitsItem->addChild(m_orcItem);
	m_standardUnitsItem->addChild(m_undeadItem);
	m_standardUnitsItem->addChild(m_nightElfItem);
	m_standardUnitsItem->addChild(m_neutralNagaItem);
	m_standardUnitsItem->addChild(m_neutralHostileItem);
	m_standardUnitsItem->addChild(m_neutralPassiveItem);

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (!this->m_unitData->isEmpty())
	{
		// skip the first row which defines the column names, start with 1
		for (map::Slk::Table::size_type row = 1; row < this->m_unitData->slk().rows(); ++row)
		{
			QTreeWidgetItem *unitItem = new QTreeWidgetItem();

			try
			{
				qDebug() << "Row:" << row;
				const QString rawData = QString::fromStdString(this->m_unitData->slk().cell(row, 0));
				const QString txtRawData = rawData.mid(1, rawData.size() - 2);

				//QString

				unitItem->setText(0, txtRawData);
				unitItem->setData(0, Qt::UserRole, txtRawData);
				qDebug() << "Adding unit" << txtRawData;

				bool ok = false;
				int inEditor = this->m_unitUi->value(rawData, "\"inEditor\"").toInt(&ok);
				int hiddenInEditor = this->m_unitUi->value(rawData, "\"hiddenInEditor\"").toInt(&ok);

				if (!inEditor || hiddenInEditor)
				{
					qDebug() << "Hiding unit " << rawData;

					continue;
				}

				int special = this->m_unitUi->value(rawData, "\"special\"").toInt(&ok);
				// Frozen Throne
				//int campaign = QString::fromStdString(this->m_unitUi->value(rawData.toStdString(), "\"campaign\"")).toInt(&ok);

				QString race = this->m_unitData->value(row, "\"race\"");


				if (race == "\"human\"")
				{
					qDebug() << "Human txt raw data" << txtRawData;
					m_humanItem->addChild(unitItem);
					unitItem->setText(0, m_humanUnitStrings->value(txtRawData, "Name"));
					QString art = m_humanUnitFunc->value(txtRawData, "Art");
					art.replace('\\', '/');
					QString iconFile;

					if (source()->download(art, iconFile, this))
					{
						unitItem->setIcon(0, QIcon(iconFile));
					}
				}
				else if (race == "\"orc\"")
				{
					m_orcItem->addChild(unitItem);
					unitItem->setText(0, m_orcUnitStrings->value(txtRawData, "Name"));
				}
				else if (race == "\"nightelf\"")
				{
					m_nightElfItem->addChild(unitItem);
				}
				else if (race == "\"undead\"")
				{
					m_undeadItem->addChild(unitItem);
				}
				else if (race == "\"creeps\"")
				{
					m_neutralNagaItem->addChild(unitItem);
				}
				else if (race == "\"other\"")
				{
					m_neutralPassiveItem->addChild(unitItem);
				}
				else if (race == "\"demon\"")
				{
					m_neutralHostileItem->addChild(unitItem);
				}
				else if (race == "\"naga\"")
				{
					m_neutralNagaItem->addChild(unitItem);
				}
				else if (race == "\"critters\"")
				{
					m_neutralPassiveItem->addChild(unitItem);
				}
				else if (race == "\"commoner\"")
				{
					m_neutralPassiveItem->addChild(unitItem);
				}
				else
				{
					m_standardUnitsItem->addChild(unitItem);
				}
			}
			catch (Exception &e)
			{
				qDebug() << "Error on creating Unit " << e.what();

				delete unitItem;
				unitItem = 0;
			}
		}
	}

	return treeWidget;
}

class ObjectTableWidget* UnitEditor::createTableWidget()
{
	return new ObjectTableWidget(this);
}

QString UnitEditor::getDataValue(const QString &objectId, const QString &fieldId) const
{
	try
	{
		const QString slkObjectId = QChar('\"') + objectId + '"';
		const QString txtObjectId = objectId;
		const QString slkField = this->metaData()->value(QChar('\"') + fieldId + '"', "\"field\"");
		const QString txtField = slkField.mid(1, slkField.size() - 2);

		qDebug() << "SLK Object ID" << slkObjectId;
		qDebug() << "TXT Object ID" << txtObjectId;
		qDebug() << "SLK Field" << slkField;
		qDebug() << "TXT Field" << txtField;

		if (this->m_unitData->hasValue(slkObjectId, slkField))
		{
			return this->m_unitData->value(slkObjectId, slkField);
		}

		if (this->m_unitUi->hasValue(slkObjectId, slkField))
		{
			return this->m_unitUi->value(slkObjectId, slkField);
		}

		if (this->m_humanUnitStrings->hasValue(txtObjectId, txtField))
		{
			return this->m_humanUnitStrings->value(txtObjectId, txtField);
		}

		if (this->m_humanUnitFunc->hasValue(txtObjectId, txtField))
		{
			return this->m_humanUnitFunc->value(txtObjectId, txtField);
		}

		if (this->m_orcUnitStrings->hasValue(txtObjectId, txtField))
		{
			return this->m_orcUnitStrings->value(txtObjectId, txtField);
		}
	}
	catch (Exception &e)
	{
		qDebug() << "Exception occured";
		qDebug() << e.what();
		//QMessageBox::warning(this, tr("Error"), e.what());
	}

	return "";
}

void UnitEditor::onSwitchToMap(class Map *map)
{
	if (map->isW3x())
	{
	}
	// W3M
	else
	{
		BOOST_FOREACH(map::CustomUnits::Table::const_reference unit, map->map()->customUnits()->originalTable())
		{
			QTreeWidgetItem *unitItem = new QTreeWidgetItem(QStringList(tr("%1").arg(unit.originalId())), 0); // TEST usually you should get the unit's name
			m_standardUnitsItem->addChild(unitItem); // TODO add to category item
		}
	}
}

void UnitEditor::onNewObject()
{
}

void UnitEditor::onRenameObject()
{
}

void UnitEditor::onDeleteObject()
{
}

void UnitEditor::onResetObject()
{
}

void UnitEditor::onResetAllObjects()
{
}

void UnitEditor::onExportAllObjects()
{
}

void UnitEditor::onImportAllObjects()
{
}

void UnitEditor::onCopyObject()
{
}

void UnitEditor::onPasteObject()
{
}

void UnitEditor::activateObject(QTreeWidgetItem* item, int column, const QString& rawDataId)
{
	qDebug() << "Activated" << rawDataId;

	/*
	 * Is folder item.
	 * Hide everything.
	 */
	if (item->childCount() > 0)
	{
		this->tableWidget()->hideColumn(0);
		this->tableWidget()->hideColumn(1);
	}
	/*
	 * Is object item.
	 * TODO Update table widget.
	 */
	else
	{
		this->tableWidget()->showColumn(0);
		this->tableWidget()->showColumn(1);

		for (ObjectTableWidget::Pairs::iterator iterator = this->tableWidget()->pairs().begin(); iterator != this->tableWidget()->pairs().end(); ++iterator)
		{
			if (showRawData())
			{
				qDebug() << "Raw data id" << rawDataId;
				qDebug() << "Field id" << iterator.key();
				iterator.value()->descriptionItem()->setText(iterator.key());
			}

			qDebug() << "Raw data id" << rawDataId;
			qDebug() << "Field id" << iterator.key();

			iterator.value()->valueItem()->setText(getDataValue(rawDataId, iterator.key()));
		}
	}
	//this->tableWidget()->activateObject
}

void UnitEditor::activateFolder(QTreeWidgetItem* item, int column)
{
	this->tableWidget()->hideColumn(0);
	this->tableWidget()->hideColumn(1);
}

void UnitEditor::onUpdateCollection(const map::CustomObjects& objects)
{
	ObjectEditorTab::onUpdateCollection(objects);

	/*

	this->treeWidget()->clear();

	QList<QTreeWidgetItem*> topLevelItems;
	QTreeWidgetItem *standardItem = new QTreeWidgetItem(QStringList(source()->tr("WESTRING_UE_STANDARDUNITS", "UI/WorldEditStrings.txt")), 0);
	QTreeWidgetItem *customItem = new QTreeWidgetItem(QStringList(source()->tr("WESTRING_UE_CUSTOMUNITS"), "UI/WorldEditStrings.txt"), 0);
	topLevelItems << standardItem;
	topLevelItems << customItem;

	this->treeWidget()->addTopLevelItems(topLevelItems);
	*/
}




}

}

