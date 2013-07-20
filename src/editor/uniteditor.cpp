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
#include "metadata.hpp"
#include "map.hpp"

namespace wc3lib
{

namespace editor
{

UnitEditor::UnitEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, KUrl("Units/UnitMetaData.slk"), parent, f)
{
	setupUi();
}

class ObjectTreeWidget* UnitEditor::createTreeWidget()
{
	ObjectTreeWidget *treeWidget = new ObjectTreeWidget(this);
	treeWidget->setHeaderLabel(tr("Units"));

	m_standardUnitsItem = new QTreeWidgetItem(treeWidget);
	m_standardUnitsItem->setText(0, source()->tr(this, "WESTRING_UE_STANDARDUNITS"));


	/// \todo Add all default unit entries
	//m_standardUnitsItem->addChild();
	/*
	 * without TFT
	m_campaignUnitsItem = new QTreeWidgetItem(treeWidget);
	item->setText(0, source()->tr("WESTRING_UE_CAMPAIGNUNITS"));
	*/

	m_customUnitsItem = new QTreeWidgetItem(treeWidget);
	m_customUnitsItem->setText(0, source()->tr(this, "WESTRING_UE_CUSTOMUNITS"));


	// TODO create UnitTreeWidget

	return treeWidget;
}

class ObjectTableWidget* UnitEditor::createTableWidget()
{
	return new ObjectTableWidget(this, m_metaData);
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

}

}

