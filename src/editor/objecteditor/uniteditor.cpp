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
#include "objecttreeview.hpp"
#include "objecttreeitem.hpp"
#include "objecttablewidget.hpp"
#include "objecttablewidgetpair.hpp"
#include "unittreemodel.hpp"
#include "unitselectiondialog.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

UnitEditor::UnitEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, new UnitData(source), parent, f)
, m_humanItem(0)
, m_humanUnitsItem(0)
, m_humanBuildingsItem(0)
, m_humanHeroesItem(0)
, m_humanSpecialItem(0)
, m_humanCampaignItem(0)
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
, m_unitSelectionDialog(new UnitSelectionDialog(source, unitData(), this))
{
	setupUi();
}

UnitEditor::~UnitEditor()
{
}

ObjectTreeModel* UnitEditor::createTreeModel()
{
	UnitTreeModel *model = new UnitTreeModel(this);

	const QString iconFilePath = MetaData::fromFilePath(this->source()->sharedData()->worldEditData()->value("WorldEditArt", "UEIcon_UnitCategory"));
	QString iconFile;
	bool iconSuccess = this->source()->download(KUrl(iconFilePath), iconFile, this);

	return model;

	//treeView->setHeaderLabel(source()->sharedData()->tr("WESTRING_OBJTAB_UNITS"));
	//m_standardObjectsItem->setText(0, source()->sharedData()->tr("WESTRING_UE_STANDARDUNITS"));

// 	if (iconSuccess)
// 	{
// 		m_standardObjectsItem->setIcon(0, QIcon(iconFile));
// 	}
//
// 	//m_customObjectsItem->setText(0, source()->sharedData()->tr("WESTRING_UE_CUSTOMUNITS"));
//
// 	if (iconSuccess)
// 	{
// 		m_customObjectsItem->setIcon(0, QIcon(iconFile));
// 	}
//
// 	// Races
// 	m_humanItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_HUMAN")), 0);
// 	m_humanUnitsItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_UNITS")), 0);
// 	m_humanBuildingsItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_UTYPE_BUILDING")), 0);
// 	m_humanHeroesItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_UTYPE_HEROES")), 0);
// 	m_humanSpecialItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_UTYPE_SPECIAL")), 0);
// 	m_humanCampaignItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_CAMPAIGN")), 0);
// 	m_orcItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_ORC")), 0);
// 	m_undeadItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_UNDEAD")), 0);
// 	m_nightElfItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_NIGHTELF")), 0);
// 	m_neutralNagaItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_RACE_NEUTRAL_NAGA")), 0);
// 	m_neutralHostileItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_HOSTILE")), 0);
// 	m_neutralPassiveItem = new QTreeWidgetItem(QStringList(source()->sharedData()->tr("WESTRING_NEUTRAL_PASSIVE")), 0);
//
// 	m_standardObjectsItem->addChild(m_humanItem);
// 	m_humanItem->addChild(m_humanUnitsItem);
// 	m_humanItem->addChild(m_humanBuildingsItem);
// 	m_humanItem->addChild(m_humanHeroesItem);
// 	m_humanItem->addChild(m_humanSpecialItem);
// 	m_humanItem->addChild(m_humanCampaignItem);
// 	m_standardObjectsItem->addChild(m_orcItem);
// 	m_standardObjectsItem->addChild(m_undeadItem);
// 	m_standardObjectsItem->addChild(m_nightElfItem);
// 	m_standardObjectsItem->addChild(m_neutralNagaItem);
// 	m_standardObjectsItem->addChild(m_neutralHostileItem);
// 	m_standardObjectsItem->addChild(m_neutralPassiveItem);

	// add all entries from "UnitData.slk" to standard units in Unit Editor
// 	if (!this->unitData()->unitData()->isEmpty())
// 	{
// 		// skip the first row which defines the column names, start with 1
// 		for (map::Slk::Table::size_type row = 1; row < this->unitData()->unitData()->slk().rows(); ++row)
// 		{
// 			QTreeWidgetItem *unitItem = new QTreeWidgetItem();
// 			const QString objectId = MetaData::fromSlkString(this->unitData()->unitData()->slk().cell(row, 0).c_str());
// 			this->fillTreeItem(objectId, "", unitItem);
//
// 			this->treeWidget()->insertStandardItem(objectId, "", unitItem);
// 		}
// 	}
}

void UnitEditor::fillTreeItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item)
{
	/*
	bool ok = false;
	int inEditor = this->unitData()->unitUi()->value(MetaData::toSlkString(originalObjectId), MetaData::toSlkString("inEditor")).toInt(&ok);
	int hiddenInEditor = this->unitData()->unitUi()->value(MetaData::toSlkString(originalObjectId), MetaData::toSlkString("hiddenInEditor")).toInt(&ok);

	if (!inEditor || hiddenInEditor)
	{
		qDebug() << "Hiding unit " << originalObjectId << ':' << customObjectId;

		return;
	}

	const QString unitName = this->objectData()->fieldValue(originalObjectId, customObjectId, "unam");

	if (!unitName.isEmpty())
	{
		item->setText(0, unitName);
	}
	else
	{
		item->setText(0, customObjectId);
	}

	const QString art = MetaData::fromFilePath(this->objectData()->fieldValue(originalObjectId, customObjectId, "uico"));

	if (!art.isEmpty())
	{
		QString iconFile;

		if (source()->download(art, iconFile, this))
		{
			item->setIcon(0, QIcon(iconFile));
		}
	}

	addItemAsChild(originalObjectId, customObjectId, item);
	*/
}

void UnitEditor::fillTableRow(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, ObjectTableWidgetPair *pair)
{
	const bool isHero = unitData()->objectIsHero(originalObjectId, customObjectId);
	const bool isUnit = unitData()->objectIsUnit(originalObjectId, customObjectId);
	const bool isBuilding = unitData()->objectIsBuilding(originalObjectId, customObjectId);

	/*
	 * In Warcraft III: Reign of Chaos item fields are also stored in "Units/UnitMetaData.slk".
	 * Those fields should not be displayed for units.
	 */
	const QString useHero = MetaData::fromSlkString(this->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("useHero")));
	const QString useUnit = MetaData::fromSlkString(this->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("useUnit")));
	const QString useBuilding = MetaData::fromSlkString(this->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("useBuilding")));

	/*
	* Hide fields which should not be shown.
	*/
	if ((useHero == "1" && isHero)
		|| (useUnit == "1" && isUnit)
		|| (useBuilding == "1" && isBuilding
		)
	)
	{
		pair->activateObject(originalObjectId, customObjectId);
		/*
			* In Frozen Throne there is a category for each value.
			*/
		QString category;

		if (this->objectData()->metaData()->hasValue(MetaData::toSlkString(fieldId), MetaData::toSlkString("category")))
		{
			category = MetaData::fromSlkString(this->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("category")));
			category = this->source()->sharedData()->worldEditData()->value("ObjectEditorCategories", category);
			category = this->source()->sharedData()->tr(category).remove('&');
		}

		const QString displayName = MetaData::fromSlkString(this->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("displayName")));
		const QString displayText = this->source()->sharedData()->tr(displayName);

		if (showRawData())
		{
			if (category.isEmpty())
			{
				pair->descriptionItem()->setText(tr("%1 (%2)").arg(fieldId).arg(displayText));
			}
			else
			{
				pair->descriptionItem()->setText(tr("%1 - %2 (%3)").arg(category).arg(fieldId).arg(displayText));
			}
		}
		else
		{
			if (category.isEmpty())
			{
				pair->descriptionItem()->setText(displayText);
			}
			else
			{
				pair->descriptionItem()->setText(tr("%1 - %2").arg(category).arg(displayText));
			}
		}

		const QString value = this->objectData()->fieldReadableValue(originalObjectId, customObjectId, fieldId);
		pair->valueItem()->setText(value);
	}
	else
	{
		qDebug() << "Hide row with field id" << fieldId << "and description" << this->source()->sharedData()->tr(MetaData::fromSlkString(this->objectData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("displayName")))) << "the row value is" << pair->descriptionItem()->row();

		tableWidget()->hideRow(pair->descriptionItem()->row());
	}
}

ObjectTableWidget* UnitEditor::createTableWidget()
{
	return new ObjectTableWidget(this);
}

void UnitEditor::onSwitchToMap(Map *map)
{
}

void UnitEditor::onNewObject()
{
	QItemSelectionModel *selection = this->treeView()->selectionModel();
	QString originalObjectId;

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		const ObjectTreeItem *item = (ObjectTreeItem*)index.internalPointer();
		originalObjectId = item->originalObjectId();
	}

	this->unitSelectionDialog()->select(originalObjectId);

	if (this->unitSelectionDialog()->exec() == QDialog::Accepted)
	{
		// TODO calculate custom id

		this->objectData()->modifyField(this->unitSelectionDialog()->originalObjectId(), "AAAA", "unam", this->unitSelectionDialog()->unitName());
	}
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
				if (this->objectData()->hasCustomObjects())
				{
					this->objectData()->customObjects().write(out);
				}
				else
				{
					this->objectData()->customUnits().write(out);
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
				this->objectData()->importCustomUnits(customUnits);
				// TODO refresh tree widget by using object data not custom units
				// TODO performance?
				boost::polymorphic_cast<UnitTreeModel*>(this->treeView()->model())->load(this->source(), this->objectData(), this);
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
					this->objectData()->importCustomUnits(*customObjectsCollection->units());
					// TODO refresh tree widget by using object data not custom units
					// TODO performance?
					boost::polymorphic_cast<UnitTreeModel*>(this->treeView()->model())->load(this->source(), this->objectData(), this);
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
				this->objectData()->importCustomUnits(*map->customUnits());
				// TODO refresh tree widget by using object data not custom units
				// TODO performance?
				boost::polymorphic_cast<UnitTreeModel*>(this->treeView()->model())->load(this->source(), this->objectData(), this);
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
	/*
	if (show)
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1 (%2)").arg(iterator.key().first).arg(unitName));
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1:%2 (%3)").arg(iterator.key().first).arg(iterator.key().second).arg(unitName));
		}
	}
	else
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}
	}
	*/
}

void UnitEditor::activateObject(ObjectTreeItem *item)
{
	qDebug() << "Activated" << item->originalObjectId();
	qDebug() << "with custom ID" << item->customObjectId();
}

void UnitEditor::activateFolder(ObjectTreeItem *item)
{
}

}

}

