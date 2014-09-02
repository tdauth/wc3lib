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

#ifndef WC3LIB_EDITOR_OBJECTTABLEWIDGETPAIR_HPP
#define WC3LIB_EDITOR_OBJECTTABLEWIDGETPAIR_HPP

#include <QObject>
#include <QTableWidgetItem>

#include "../../core.hpp"
#include "../../map.hpp"
#include "../platform.hpp"
#include "objectmetadata.hpp"
#include "objecteditortab.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Provides access to two corresponding table widget items.
 * First is description of value, second is the value itself.
 * Values which are not default (custom values) are usually marked by another color.
 *
 * The description is stored in the meta data file (for example UnitMetaData.slk) which can be accessed via \ref metaData().
 * The meta data stores also the category and additinal displa information
 *
 * The default value is stored in a data file (for example UnitData.txt) which can be accessed via \ref data().
 */
class ObjectTableWidgetPair : public QObject
{
	public:
		ObjectTableWidgetPair(QTableWidget *tableWidget, ObjectEditorTab *tab, int row, const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
		~ObjectTableWidgetPair();

		QTableWidget* tableWidget() const;
		ObjectEditorTab* tab() const;

		void reset();


		/**
		 * Generates a value object with the correct type and data of the field.
		 */
		map::Value defaultValue() const;

		map::CustomUnits::Modification modification(const map::Value &value) const;

		void activateObject(const QString &originalObjectId, const QString &customObjectId);
		QString originalObjectId() const;
		QString customObjectId() const;
		QString fieldId() const;

		QTableWidgetItem* descriptionItem() const;
		QTableWidgetItem* valueItem() const;

		void edit();
		bool isModified() const;

	protected:
		map::Value value(const QString &text) const;

		ObjectEditorTab *m_tab;
		QString m_originalObjectId;
		QString m_customObjectId;
		QString m_fieldId;
		QTableWidgetItem *m_descriptionItem;
		QTableWidgetItem *m_valueItem;
};

inline QTableWidget* ObjectTableWidgetPair::tableWidget() const
{
	return boost::polymorphic_cast<QTableWidget*>(parent());
}

inline ObjectEditorTab* ObjectTableWidgetPair::tab() const
{
	return this->m_tab;
}

inline void ObjectTableWidgetPair::activateObject(const QString &originalObjectId, const QString &customObjectId)
{
	this->m_originalObjectId = originalObjectId;
	this->m_customObjectId = customObjectId;

	if (isModified())
	{
		this->descriptionItem()->setForeground(Qt::magenta);
		this->valueItem()->setForeground(Qt::magenta);
	}
	else
	{
		this->descriptionItem()->setForeground(Qt::black);
		this->valueItem()->setForeground(Qt::black);
	}
}

inline QString ObjectTableWidgetPair::originalObjectId() const
{
	return this->m_originalObjectId;
}

inline QString ObjectTableWidgetPair::customObjectId() const
{
	return this->m_customObjectId;
}

inline QString ObjectTableWidgetPair::fieldId() const
{
	return this->m_fieldId;
}


inline QTableWidgetItem* ObjectTableWidgetPair::descriptionItem() const
{
	return m_descriptionItem;
}

inline QTableWidgetItem* ObjectTableWidgetPair::valueItem() const
{
	return m_valueItem;
}

inline bool ObjectTableWidgetPair::isModified() const
{
	return this->tab()->isFieldModified(this->originalObjectId(), this->customObjectId(), this->fieldId());
}

}

}

#endif
