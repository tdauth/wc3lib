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
#include <KFileDialog>

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

UnitEditor::UnitEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, new UnitMetaData(source), parent, f)
, m_humanItem(0)
, m_orcItem(0)
, m_undeadItem(0)
, m_nightElfItem(0)
, m_neutralNagaItem(0)
, m_neutralHostileItem(0)
, m_neutralPassiveItem(0)
, m_customHumanItem(0)
, m_customOrcItem(0)
, m_customUndeadItem(0)
, m_customNightElfItem(0)
, m_customNeutralNagaItem(0)
, m_customNeutralHostileItem(0)
, m_customNeutralPassiveItem(0)
{
	setupUi();
}

UnitEditor::~UnitEditor()
{
}

void UnitEditor::addItemAsChild(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item)
{
	const QString race = MetaData::fromSlkString(this->unitMetaData()->unitData()->value(MetaData::toSlkString(originalObjectId), "\"race\""));
	qDebug() << "Race:" << race;

	if (customObjectId.isEmpty())
	{
		if (race == "human")
		{
			m_humanItem->addChild(item);
		}
		else if (race == "orc")
		{
			m_orcItem->addChild(item);
		}
		else if (race == "nightelf")
		{
			m_nightElfItem->addChild(item);
		}
		else if (race == "undead")
		{
			m_undeadItem->addChild(item);
		}
		else if (race == "creeps")
		{
			m_neutralHostileItem->addChild(item);
		}
		else if (race == "other")
		{
			m_neutralPassiveItem->addChild(item);
		}
		else if (race == "demon")
		{
			m_neutralHostileItem->addChild(item);
		}
		else if (race == "naga")
		{
			m_neutralNagaItem->addChild(item);
		}
		else if (race == "critters")
		{
			m_neutralPassiveItem->addChild(item);
		}
		else if (race == "commoner")
		{
			m_neutralPassiveItem->addChild(item);
		}
		else
		{
			m_standardObjectsItem->addChild(item);
		}
	}
	else
	{
		if (race == "human")
		{
			if (m_customHumanItem == 0)
			{
				m_customHumanItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_HUMAN")), 0);
				m_customObjectsItem->addChild(m_customHumanItem);
			}

			m_customHumanItem->addChild(item);
		}
		else if (race == "orc")
		{
			if (m_customOrcItem == 0)
			{
				m_customOrcItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_ORC")), 0);
				m_customObjectsItem->addChild(m_customOrcItem);
			}

			m_customOrcItem->addChild(item);
		}
		else if (race == "nightelf")
		{
			if (m_customNightElfItem == 0)
			{
				m_customNightElfItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_NIGHTELF")), 0);
				m_customObjectsItem->addChild(m_customNightElfItem);
			}

			m_customNightElfItem->addChild(item);
		}
		else if (race == "undead")
		{
			if (m_customUndeadItem == 0)
			{
				m_customUndeadItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_UNDEAD")), 0);
				m_customObjectsItem->addChild(m_customUndeadItem);
			}

			m_customUndeadItem->addChild(item);
		}
		else if (race == "creeps" || race == "demon")
		{
			if (m_customNeutralHostileItem == 0)
			{
				m_customNeutralHostileItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_HOSTILE")), 0);
				m_customObjectsItem->addChild(m_customNeutralHostileItem);
			}

			m_customNeutralHostileItem->addChild(item);
		}
		else if (race == "other" || race == "critters" || race == "commoner")
		{
			if (m_customNeutralPassiveItem == 0)
			{
				m_customNeutralPassiveItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_PASSIVE")), 0);
				m_customObjectsItem->addChild(m_customNeutralPassiveItem);
			}

			m_customNeutralPassiveItem->addChild(item);
		}
		else if (race == "naga")
		{
			if (m_customNeutralNagaItem == 0)
			{
				m_customNeutralNagaItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_NAGA")), 0);
				m_customObjectsItem->addChild(m_customNeutralNagaItem);
			}

			m_customNeutralNagaItem->addChild(item);
		}
		else
		{
			m_customObjectsItem->addChild(item);
		}
	}
}

void UnitEditor::setupTreeWidget(ObjectTreeWidget *treeWidget)
{
	treeWidget->setHeaderLabel(source()->sharedData()->tr("WESTRING_OBJTAB_UNITS"));
	m_standardObjectsItem->setText(0, source()->sharedData()->tr("WESTRING_UE_STANDARDUNITS"));
	m_customObjectsItem->setText(0, source()->sharedData()->tr("WESTRING_UE_CUSTOMUNITS"));

	// Races
	m_humanItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_HUMAN")), 0);
	m_orcItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_ORC")), 0);
	m_undeadItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_UNDEAD")), 0);
	m_nightElfItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_NIGHTELF")), 0);
	m_neutralNagaItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_NEUTRAL_NAGA")), 0);
	m_neutralHostileItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_HOSTILE")), 0);
	m_neutralPassiveItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_PASSIVE")), 0);

	m_standardObjectsItem->addChild(m_humanItem);
	m_standardObjectsItem->addChild(m_orcItem);
	m_standardObjectsItem->addChild(m_undeadItem);
	m_standardObjectsItem->addChild(m_nightElfItem);
	m_standardObjectsItem->addChild(m_neutralNagaItem);
	m_standardObjectsItem->addChild(m_neutralHostileItem);
	m_standardObjectsItem->addChild(m_neutralPassiveItem);

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (!this->unitMetaData()->unitData()->isEmpty())
	{
		// skip the first row which defines the column names, start with 1
		for (map::Slk::Table::size_type row = 1; row < this->unitMetaData()->unitData()->slk().rows(); ++row)
		{
			QTreeWidgetItem *unitItem = new QTreeWidgetItem();
			const QString objectId = MetaData::fromSlkString(this->unitMetaData()->unitData()->slk().cell(row, 0).c_str());
			this->fillTreeItem(objectId, "", unitItem);

			this->treeWidget()->insertStandardItem(objectId, "", unitItem);
		}
	}
}

void UnitEditor::fillTreeItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item)
{
	try
	{
		bool ok = false;
		int inEditor = this->unitMetaData()->unitUi()->value(MetaData::toSlkString(originalObjectId), MetaData::toSlkString("inEditor")).toInt(&ok);
		int hiddenInEditor = this->unitMetaData()->unitUi()->value(MetaData::toSlkString(originalObjectId), MetaData::toSlkString("hiddenInEditor")).toInt(&ok);

		if (!inEditor || hiddenInEditor)
		{
			qDebug() << "Hiding unit " << originalObjectId << ':' << customObjectId;

			return;
		}

		const QString unitName = this->fieldValue(originalObjectId, customObjectId, "unam");

		if (!unitName.isEmpty())
		{
			item->setText(0, unitName);
		}
		else
		{
			item->setText(0, customObjectId);
		}

		const QString art = MetaData::fromFilePath(this->fieldValue(originalObjectId, customObjectId, "uico"));

		if (!art.isEmpty())
		{
			QString iconFile;

			if (source()->download(art, iconFile, this))
			{
				item->setIcon(0, QIcon(iconFile));
			}
		}

		int special = this->unitMetaData()->unitUi()->value(MetaData::toSlkString(originalObjectId), "\"special\"").toInt(&ok);
		// Frozen Throne
		//int campaign = QString::fromStdString(this->m_unitUi->value(rawData.toStdString(), "\"campaign\"")).toInt(&ok);

		addItemAsChild(originalObjectId, customObjectId, item);
	}
	catch (Exception &e)
	{
		qDebug() << "Error on creating Unit " << e.what();
	}
}

class ObjectTableWidget* UnitEditor::createTableWidget()
{
	return new ObjectTableWidget(this);
}

map::CustomUnits::Unit UnitEditor::currentUnit() const
{
	const QString objectId = this->currentObjectId();
	map::CustomUnits::Unit unit;
	// TODO which one is the custom id
	unit.setOriginalId(map::stringToId(objectId.toStdString()));
	unit.setCustomId(map::stringToId(objectId.toStdString()));

	/*
	for (ObjectTableWidget::Pairs::iterator iterator = this->tableWidget()->pairs().begin(); iterator != this->tableWidget()->pairs().end(); ++iterator)
	{
		if (this->isFieldModified(objectId, iterator.key()))
		{
			std::auto_ptr<map::CustomUnits::Modification> modification(iterator.value()->modification());
			unit.modifications().push_back(modification);
		}
	}
	*/

	// TODO is original or custom
	//this->customObjects()->originalTable().push_back(object);

	return unit;
}

map::CustomObjects::Object UnitEditor::currentObject() const
{
	map::CustomObjects::Object object(map::CustomObjects::Type::Units);

	return object;
}

map::CustomUnits UnitEditor::customUnits() const
{
	map::CustomUnits units;

	return units;
}

bool UnitEditor::hasCustomUnits() const
{
	return this->metaData()->isReignOfChaos();
}

map::CustomObjects UnitEditor::customObjects() const
{
	map::CustomObjects objects = map::CustomObjects(map::CustomObjects::Type::Units);

	return objects;
}

bool UnitEditor::hasCustomObjects() const
{
	return !this->metaData()->isReignOfChaos();
}


bool UnitEditor::objectIsBuilding(const QString &objectId) const
{
	const QString levelVar = MetaData::fromSlkString(this->metaData()->getDataValue(objectId, "ulev"));
	bool ok = false;
	levelVar.toInt(&ok);

	return !ok; // buildings have no levels
}

bool UnitEditor::objectIsHero(const QString &objectId) const
{
	const QString strengthVar = MetaData::fromSlkString(this->metaData()->getDataValue(objectId, "ustr"));
	bool ok = false;
	strengthVar.toInt(&ok);

	return ok;
}

bool UnitEditor::objectIsUnit(const QString &objectId) const
{
	return unitMetaData()->unitData()->hasValue(MetaData::toSlkString(objectId), MetaData::toSlkString("unitID")) && !objectIsHero(objectId) && !objectIsBuilding(objectId);
}

void UnitEditor::onSwitchToMap(Map *map)
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
			m_standardObjectsItem->addChild(unitItem); // TODO add to category item
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
	const QString suffix = "w3u";
	const QString customObjectsCollectionSuffix = "w3o";
	const QString mapSuffix = "w3m";

	const KUrl url = KFileDialog::getSaveUrl(KUrl(), QString("*\n*.%1\nCustom Objects Collection *.%2\nMap (*.%3 *.%4)").arg(suffix).arg(customObjectsCollectionSuffix).arg(mapSuffix), this, exportAllObjectsText());

	if (!url.isEmpty())
	{
		ofstream out(url.toLocalFile().toUtf8().constData());

		if (out)
		{
			try
			{
				if (hasCustomObjects())
				{
					this->customObjects().write(out);
				}
				else
				{
					this->customUnits().write(out);
				}
			}
			catch (Exception &e)
			{
				KMessageBox::error(this, tr("Error on exporting"), e.what());
			}
		}
	}
}

void UnitEditor::onImportAllObjects()
{
	const QString customObjectsSuffix = "w3u";
	const QString customObjectsCollectionSuffix = "w3o";
	const QString mapSuffix = "w3m";
	const QString xmapSuffix = "w3x";

	const KUrl url = KFileDialog::getOpenUrl(KUrl(), QString("*\n*.%1\nCustom Objects Collection *.%2\nMap (*.%3 *.%4)").arg(customObjectsSuffix).arg(customObjectsCollectionSuffix).arg(mapSuffix).arg(xmapSuffix), this, importAllObjectsText());

	if (!url.isEmpty())
	{
		const QString suffix = QFileInfo(url.toLocalFile()).suffix();
		qDebug() << "Suffix" << suffix;

		map::CustomUnits customUnits;

		if (suffix == customObjectsSuffix)
		{
			ifstream in(url.toLocalFile().toUtf8().constData());

			try
			{
				customUnits.read(in);
				this->importCustomUnits(customUnits);
			}
			catch (Exception &e)
			{
				KMessageBox::error(this, tr("Error on importing"), e.what());
			}
		}
		else if (suffix == customObjectsCollectionSuffix)
		{
			ifstream in(url.toLocalFile().toUtf8().constData());

			try
			{
				std::unique_ptr<map::CustomObjectsCollection> customObjectsCollection(new map::CustomObjectsCollection());
				customObjectsCollection->read(in);

				if (customObjectsCollection->hasUnits())
				{
					this->importCustomUnits(*customObjectsCollection->units());
				}
				else
				{
					KMessageBox::error(this, tr("Collection has no units."));
				}
			}
			catch (Exception &e)
			{
				KMessageBox::error(this, e.what());
			}
		}
		else if (suffix == mapSuffix)
		{
			try
			{
				std::unique_ptr<map::W3m> map(new map::W3m());
				map->open(url.toLocalFile().toUtf8().constData());
				std::streamsize size = map->readFileFormat(map->customUnits().get());
				qDebug() << "Size of custom units:" << size;
				this->importCustomUnits(*map->customUnits());
			}
			catch (Exception &e)
			{
				KMessageBox::error(this, e.what());
			}
		}
		else if (suffix == xmapSuffix)
		{
			KMessageBox::error(this, tr("W3X is not supported yet."));
		}
		else
		{
			KMessageBox::error(this, tr("Unknown file type."));
		}
	}
}

void UnitEditor::onCopyObject()
{
}

void UnitEditor::onPasteObject()
{
}

void UnitEditor::onShowRawData(bool show)
{
	if (show)
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1 (%2)").arg(iterator.key().first).arg(unitName));
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1:%2 (%3)").arg(iterator.key().first).arg(iterator.key().second).arg(unitName));
		}
	}
	else
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}
	}
}

void UnitEditor::activateObject(QTreeWidgetItem* item, int column, const QString &originalObjectId, const QString &customObjectId)
{
	qDebug() << "Activated" << originalObjectId;
	qDebug() << "with custom ID" << customObjectId;

	/*
	 * Is folder item.
	 * Hide everything.
	 */
	if (item->childCount() > 0 || originalObjectId.isEmpty())
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
			const QString fieldId = iterator.key();

			qDebug() << "Original object id" << originalObjectId;
			qDebug() << "Field id" << fieldId;

			const bool isHero = objectIsHero(originalObjectId);
			const bool isUnit = objectIsUnit(originalObjectId);
			const bool isBuilding = objectIsBuilding(originalObjectId);

			try
			{
				/*
				 * In Warcraft III: Reign of Chaos item fields are also stored in "Units/UnitMetaData.slk".
				 * Those fields should not be displayed for units.
				 */

				const QString useHero = MetaData::fromSlkString(this->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("useHero")));
				const QString useUnit = MetaData::fromSlkString(this->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("useUnit")));
				const QString useBuilding = MetaData::fromSlkString(this->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("useBuilding")));

				qDebug() << "useHero" << useHero;
				qDebug() << "useUnit" << useUnit;
				qDebug() << "useBuilding" << useBuilding;

				/*
				* Hide fields which should not be shown.
				*/
				if ((useHero == "1" && isHero)
					|| (useUnit == "1" && isUnit)
					|| (useBuilding == "1" && isBuilding
					)
				)
				{
					qDebug() << "Field id" << fieldId;
					iterator.value()->activateObject(originalObjectId, customObjectId);
					const QString displayName = MetaData::fromSlkString(this->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("displayName")));
					const QString displayText = this->source()->sharedData()->tr(displayName);

					if (showRawData())
					{
						iterator.value()->descriptionItem()->setText(tr("%1 (%2)").arg(fieldId).arg(displayText));
					}
					else
					{
						iterator.value()->descriptionItem()->setText(displayText);
					}

					const QString value = this->fieldReadableValue(originalObjectId, customObjectId, fieldId);
					qDebug() << "Value:" << value << "for field id" << fieldId;
					iterator.value()->valueItem()->setText(value);
				}
				else
				{
					qDebug() << "Hide row with field id" << fieldId << "and description" << this->source()->sharedData()->tr(MetaData::fromSlkString(this->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("displayName")))) << "the row value is" << iterator.value()->descriptionItem()->row();

					tableWidget()->hideRow(iterator.value()->descriptionItem()->row());
				}
			}
			catch (Exception &e)
			{
				qDebug() << "Exception with object" << originalObjectId << ":" << customObjectId << " and field" << fieldId << ":" << e.what();
			}
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

