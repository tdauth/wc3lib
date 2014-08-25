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
		ObjectTableWidgetPair(QTableWidget *tableWidget, ObjectEditorTab *tab, int row, const QString &objectId, const QString &fieldId);
		~ObjectTableWidgetPair();

		QTableWidget* tableWidget() const;
		ObjectEditorTab* tab() const;

		void reset();
		bool isDefault() const;

		/**
		 * Sets the object ID such as "hfoo" for the Footman."
		 */
		void setObjectId(const QString &id);
		const QString& objectId() const;
		/**
		 * Sets the field ID such as "unam" for the name
		 */
		void setFieldId(const QString &id);
		const QString& fieldId() const;
		/**
		 * \return Returns value of data by using corresponding meta data.
		 */
		map::Value defaultValue() const;
		/**
		 * Assigns
		 */
		void setCustomValue(const map::Value &customValue);
		const map::Value& customValue() const;
		QTableWidgetItem* descriptionItem() const;
		QTableWidgetItem* valueItem() const;

		void edit();

	protected:
		void update();

		ObjectEditorTab *m_tab;
		QString m_objectId;
		QString m_fieldId;
		map::Value m_customValue;
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

inline void ObjectTableWidgetPair::reset()
{
	setCustomValue(defaultValue());
}

inline bool ObjectTableWidgetPair::isDefault() const
{
	return customValue() == defaultValue();
}

inline void ObjectTableWidgetPair::setObjectId(const QString &id)
{
	this->m_objectId = id;
}

inline const QString& ObjectTableWidgetPair::objectId() const
{
	return this->m_objectId;
}

inline void ObjectTableWidgetPair::setFieldId(const QString &id)
{
	m_fieldId = id;
}

inline const QString& ObjectTableWidgetPair::fieldId() const
{
	return m_fieldId;
}

inline map::Value ObjectTableWidgetPair::defaultValue() const
{
	return this->tab()->getDataValue(objectId(), this->fieldId()).toStdString();
}

inline void ObjectTableWidgetPair::setCustomValue(const map::Value &customValue)
{
	m_customValue = customValue;
	update();
}

inline const map::Value& ObjectTableWidgetPair::customValue() const
{
	return m_customValue;
}

inline QTableWidgetItem* ObjectTableWidgetPair::descriptionItem() const
{
	return m_descriptionItem;
}

inline QTableWidgetItem* ObjectTableWidgetPair::valueItem() const
{
	return m_valueItem;
}

}

}

#endif
