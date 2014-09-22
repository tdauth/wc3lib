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
#include "unittreemodel.hpp"
#include "unitselectiondialog.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

UnitEditor::UnitEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, new UnitData(source), parent, f), m_unitSelectionDialog(new UnitSelectionDialog(source, unitData(), this))
{
	setupUi();
}

UnitEditor::~UnitEditor()
{
}

ObjectTreeModel* UnitEditor::createTreeModel()
{
	return new UnitTreeModel(this->source(), this);
}

void UnitEditor::onSwitchToMap(Map *map)
{
}

void UnitEditor::onNewObject()
{
	QItemSelectionModel *selection = this->treeView()->selectionModel();
	qDebug() << "Selection:" << selection;
	QString originalObjectId;

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		const ObjectTreeItem *item = (ObjectTreeItem*)index.internalPointer();
		originalObjectId = item->originalObjectId();
	}

	if (originalObjectId.isEmpty())
	{
		originalObjectId = "hpea";
	}

	this->unitSelectionDialog()->select(originalObjectId);

	if (this->unitSelectionDialog()->exec() == QDialog::Accepted)
	{
		// TODO calculate custom id
		const QString customObjectId = this->unitData()->nextCustomObjectId();

		qDebug() << "Custom Object ID:" << customObjectId;

		this->objectData()->modifyField(this->unitSelectionDialog()->originalObjectId(), customObjectId, "unam", this->unitSelectionDialog()->unitName());
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

