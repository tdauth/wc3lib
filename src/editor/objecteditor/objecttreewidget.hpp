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

#ifndef WC3LIB_EDITOR_OBJECTREEWIDGET_HPP
#define WC3LIB_EDITOR_OBJECTREEWIDGET_HPP

#include <QTreeWidget>
#include <QMenu>

namespace wc3lib
{

namespace editor
{

class ObjectEditorTab;
/**
 * Object tree widgets can be used to display game/campaign/map objects in a tree style divided into groups (e. g. their races, players etc.)
 *
 */
class ObjectTreeWidget : public QTreeWidget
{
	Q_OBJECT

	public:
		/**
		 * \brief Stores the two object IDs. The original as well as the custom.
		 */
		typedef QPair<QString, QString> ObjectId;
		/**
		 * All items are hashed by the raw data name of the object.
		 */
		typedef QHash<ObjectId, QTreeWidgetItem*> Items;

		ObjectTreeWidget(ObjectEditorTab *parent = 0, Qt::WindowFlags f = 0);

		ObjectEditorTab* tab() const;
		QMenu* contextMenu() const;

		void insertStandardItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item);
		void insertCustomItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item);
		Items& standardItems();
		Items& customItems();

		/**
		 * Clears all items which are under the "Custom ..." entry.
		 * \note This member function removes their entries from the hash and deletes the items.
		 */
		void clearCustomItems();

		QString itemOriginalObjectId(const QTreeWidgetItem &item) const;
		QString itemCustomObjectId(const QTreeWidgetItem &item) const;
		QTreeWidgetItem* item(const QString &originalObjectId, const QString &customObjectId);

	private:
		ObjectEditorTab *m_tab;

		Items m_standardItems;
		Items m_customItems;

		QMenu *m_contextMenu;

	private slots:
		void customContextMenuRequested(QPoint pos);
};

inline ObjectEditorTab* ObjectTreeWidget::tab() const
{
	return this->m_tab;
}

inline QMenu* ObjectTreeWidget::contextMenu() const
{
	return this->m_contextMenu;
}

inline void ObjectTreeWidget::insertStandardItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem* item)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	this->m_standardItems.insert(objectId, item);
	item->setData(0, Qt::UserRole, QString("%1:%2").arg(originalObjectId).arg(customObjectId));
}

inline void ObjectTreeWidget::insertCustomItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem* item)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	this->m_customItems.insert(objectId, item);
	item->setData(0, Qt::UserRole, QString("%1:%2").arg(originalObjectId).arg(customObjectId));
}

inline ObjectTreeWidget::Items& ObjectTreeWidget::standardItems()
{
	return this->m_standardItems;
}

inline ObjectTreeWidget::Items& ObjectTreeWidget::customItems()
{
	return this->m_customItems;
}

inline QString ObjectTreeWidget::itemOriginalObjectId(const QTreeWidgetItem &item) const
{
	const QString data = item.data(0, Qt::UserRole).toString();
	QStringList list = data.split(':');

	if (list.isEmpty())
	{
		return "";
	}

	return list[0];
}

inline QString ObjectTreeWidget::itemCustomObjectId(const QTreeWidgetItem &item) const
{
	const QString data = item.data(0, Qt::UserRole).toString();
	QStringList list = data.split(':');

	if (list.size() < 2)
	{
		return "";
	}

	return list[1];
}


inline QTreeWidgetItem* ObjectTreeWidget::item(const QString &originalObjectId, const QString &customObjectId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Items::iterator iterator = this->m_standardItems.find(objectId);

	if (iterator != this->m_standardItems.end())
	{
		return iterator.value();
	}

	iterator = this->m_customItems.find(objectId);

	if (iterator != this->m_customItems.end())
	{
		return iterator.value();
	}

	return 0;
}


}

}

#endif
