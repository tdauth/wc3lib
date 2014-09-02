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

#include <KMessageBox>
#include <KFileDialog>

#include <QtGui>

#include "objecteditortab.hpp"
#include "objecttreewidget.hpp"
#include "objecttablewidget.hpp"
#include "objectmetadata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

ObjectEditorTab::ObjectEditorTab(class MpqPriorityList *source, ObjectMetaData *metaData, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
, m_source(source)
, m_metaData(metaData)
, m_tabIndex(0)
, m_filterLineEdit(0)
, m_treeWidget(0)
, m_standardObjectsItem(0)
, m_customObjectsItem(0)
, m_tableWidget(0)
, m_showRawData(false)
{
	this->metaData()->load(this);
}

ObjectEditorTab::~ObjectEditorTab()
{
	delete this->m_metaData;
}


void ObjectEditorTab::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId, const map::CustomUnits::Modification &modification)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		iterator = this->m_objects.insert(objectId, Modifications());
	}

	iterator.value().insert(fieldId, modification);
	QTreeWidgetItem *item = this->treeWidget()->item(originalObjectId, customObjectId);

	if (item != 0)
	{
		item->setForeground(0, Qt::magenta);
	}
	else
	{
		qDebug() << "Missing tree widget item of object " << objectId;
	}
}

void ObjectEditorTab::resetField(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return;
	}

	iterator.value().remove(fieldId);

	if (iterator.value().empty())
	{
		this->m_objects.erase(iterator);

		QTreeWidgetItem *item = this->treeWidget()->item(originalObjectId, customObjectId);

		if (item != 0)
		{
			item->setForeground(0, Qt::black);
		}
		else
		{
			qDebug() << "Missing tree widget item of object " << objectId;
		}
	}
}

bool ObjectEditorTab::isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	return iterator.value().contains(fieldId);

}

void ObjectEditorTab::clearModifications()
{
	this->m_objects.clear();
}

bool ObjectEditorTab::fieldModificiation(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, map::CustomUnits::Modification &modification) const
{

	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	Modifications::const_iterator iterator2 = iterator.value().find(fieldId);

	if (iterator2 == iterator.value().end())
	{
		return false;
	}

	modification = iterator2.value();

	return true;

}

QString ObjectEditorTab::fieldValue(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId) const
{
	map::CustomUnits::Modification modification;

	if (fieldModificiation(originalObjectId, customObjectId, fieldId, modification))
	{
		return valueToString(modification.value());
	}
	// Otherwise return the default value from Warcraft III
	else
	{
		return this->metaData()->getDataValue(originalObjectId, fieldId);
	}

	return "";
}

QString ObjectEditorTab::fieldReadableValue(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId) const
{
	QString fieldValue = this->fieldValue(originalObjectId, customObjectId, fieldId);
	QString fieldType = MetaData::fromSlkString(this->metaData()->metaData()->value(MetaData::toSlkString(fieldId), MetaData::toSlkString("type")));

	if (fieldType == "int" || fieldType == "uint" || fieldType == "real" || fieldType == "unreal" || fieldType == "string")
	{
		return fieldValue;
	}

	const map::Txt::Section *section = this->metaData()->objectTabData()->section(fieldType);

	if (section == 0)
	{
		return fieldValue;
	}

	for (std::size_t i = 0; i < section->entries.size(); ++i)
	{
		const QString sectionValue = QString::fromUtf8(section->entries[i].second.c_str());
		QStringList values = sectionValue.split(',');

		if (values.size() == 2)
		{
			/*
			 * It seems that values are existing like "Summoned" where it should be called "summoned"
			 */
			if (fieldValue.toLower() == values[0].toLower())
			{
				return this->source()->sharedData()->tr(values[1]);
			}
		}
	}

	return fieldValue;
}

void ObjectEditorTab::setupUi()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QWidget *layoutWidget = new QWidget(this);
	layoutWidget->setLayout(layout);

	qDebug() << "Show tab " << this->name();

	m_filterLineEdit = new KLineEdit(this);
	m_filterLineEdit->setClearButtonShown(true);

	m_treeWidget = new ObjectTreeWidget(this);

	QList<QTreeWidgetItem*> topLevelItems;
	this->m_standardObjectsItem = new QTreeWidgetItem(m_treeWidget);
	this->m_customObjectsItem = new QTreeWidgetItem(m_treeWidget);
	topLevelItems << this->m_standardObjectsItem;
	topLevelItems << this->m_standardObjectsItem;
	m_treeWidget->addTopLevelItems(topLevelItems);

	setupTreeWidget(m_treeWidget);

	m_tableWidget = createTableWidget();

	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
	layout->addWidget(m_filterLineEdit);
	layout->addWidget(m_treeWidget);
	splitter->addWidget(layoutWidget);
	splitter->addWidget(m_tableWidget);
	this->setLayout(new QHBoxLayout(this));
	this->layout()->addWidget(splitter);

	connect(m_filterLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTreeWidget(QString)));
	connect(m_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));
}


void ObjectEditorTab::importAllObjects()
{
	onImportAllObjects();
}

void ObjectEditorTab::exportAllObjects()
{
	onExportAllObjects();
}

void ObjectEditorTab::importCustomUnits(const map::CustomUnits &units)
{
	this->clearModifications();
	this->treeWidget()->clearCustomItems();

	qDebug() << "Custom Table size:" << units.customTable().size();

	for (map::CustomUnits::Table::size_type i = 0; i < units.customTable().size(); ++i)
	{
		const map::CustomUnits::Unit &unit = units.customTable()[i];
		const QString originalObjectId = map::idToString(unit.originalId()).c_str();
		const QString customObjectId = map::idToString(unit.customId()).c_str();
		QTreeWidgetItem *item = new QTreeWidgetItem();

		for (map::CustomUnits::Unit::Modifications::size_type j = 0; j < unit.modifications().size(); ++j)
		{
			const map::CustomUnits::Modification &modification = unit.modifications()[j];
			this->modifyField(originalObjectId, customObjectId, map::idToString(modification.valueId()).c_str(), modification);
		}

		this->fillTreeItem(originalObjectId, customObjectId, item);

		this->treeWidget()->insertCustomItem(originalObjectId, customObjectId, item);
	}

	//units.customTable()
	// TODO read all objects
}

void ObjectEditorTab::itemClicked(QTreeWidgetItem *item, int column)
{
	this->activateObject(item, column, treeWidget()->itemOriginalObjectId(*item), treeWidget()->itemCustomObjectId(*item));
}

void ObjectEditorTab::filterTreeWidget(QString text)
{
	// TODO filter

}

void ObjectEditorTab::setShowRawData(bool show)
{
	this->m_showRawData = show;

	QList<QTreeWidgetItem*> selection = treeWidget()->selectedItems();

	if (!selection.isEmpty())
	{
		activateObject(selection.first(), 0, treeWidget()->itemOriginalObjectId(*selection.first()), treeWidget()->itemCustomObjectId(*selection.first()));
	}

	onShowRawData(show);
}

map::CustomUnits::Unit ObjectEditorTab::currentUnit() const
{
	map::CustomUnits::Unit unit;
	// TODO which one is the custom id
	unit.setOriginalId(map::stringToId(currentOriginalObjectId().toStdString()));
	unit.setCustomId(map::stringToId(currentCustomObjectId().toStdString()));

	Objects::const_iterator iterator = this->m_objects.find(ObjectId(currentOriginalObjectId(), currentCustomObjectId()));

	if (iterator != this->m_objects.end())
	{
		foreach (map::CustomUnits::Modification modification, iterator.value())
		{
			unit.modifications().push_back(new map::CustomUnits::Modification(modification));
		}
	}

	return unit;
}

map::CustomObjects::Object ObjectEditorTab::currentObject() const
{
	map::CustomObjects::Object object(map::CustomObjects::Type::Units);

	return object;
}

map::CustomUnits ObjectEditorTab::customUnits() const
{
	map::CustomUnits units;

	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		map::CustomUnits::Unit unit;
		// TODO which one is the custom id
		unit.setOriginalId(map::stringToId(iterator.key().first.toStdString()));
		unit.setCustomId(map::stringToId(iterator.key().second.toStdString()));

		foreach (map::CustomUnits::Modification modification, iterator.value())
		{
			unit.modifications().push_back(new map::CustomUnits::Modification(modification));
		}

		/*
		 * No custom ID means it is a standard unit.
		 */
		if (iterator.key().second.isEmpty())
		{
			units.originalTable().push_back(new map::CustomUnits::Unit(unit));
		}
		else
		{
			units.customTable().push_back(new map::CustomUnits::Unit(unit));
		}
	}

	return units;
}

map::CustomObjects ObjectEditorTab::customObjects() const
{
	map::CustomObjects objects = map::CustomObjects(map::CustomObjects::Type::Units);

	return objects;
}

#include "moc_objecteditortab.cpp"

}

}
