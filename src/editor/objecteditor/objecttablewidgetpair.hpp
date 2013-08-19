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

namespace wc3lib
{

namespace editor
{

/**
 * Provides access to two corresponding table widget items.
 * First is description of value, second is the value itself.
 * Values which are not default (custom values) are usually marked by another color.
 */
class ObjectTableWidgetPair : public QObject
{
	public:
		ObjectTableWidgetPair(QTableWidget *tableWidget, int row, map::MetaData *metaData);
		~ObjectTableWidgetPair();
		
		QTableWidget* tableWidget() const;
		int row() const;
		
		void reset();
		bool isDefault() const;
		
		map::MetaData* metaData() const;
		void setData(map::Data *data);
		map::Data* data() const;
		/**
		 * \return Returns value of data by using corresponding meta data.
		 */
		const map::Value& defaultValue() const;
		/**
		 * Assigns 
		 */
		void setCustomValue(const map::Value &customValue);
		const map::Value& customValue() const;
		QTableWidgetItem* descriptionItem() const;
		QTableWidgetItem* valueItem() const;
		
	protected:
		void update();
		
		map::MetaData *m_metaData; // meta data describes which kind of value and description this pair have
		map::Data *m_data;
		map::Value m_customValue;
		QTableWidgetItem *m_descriptionItem;
		QTableWidgetItem *m_valueItem;
		int m_row;
};

inline QTableWidget* ObjectTableWidgetPair::tableWidget() const
{
	return boost::polymorphic_cast<QTableWidget*>(parent());
}

inline int ObjectTableWidgetPair::row() const
{
	return m_row;
}

inline void ObjectTableWidgetPair::reset()
{
	setCustomValue(defaultValue());
}

inline bool ObjectTableWidgetPair::isDefault() const
{
	return customValue() == defaultValue();
}

inline map::MetaData* ObjectTableWidgetPair::metaData() const
{
	return m_metaData;
}

inline void ObjectTableWidgetPair::setData(map::Data *data)
{
	m_data = data;
	m_customValue = defaultValue();
	update();
}

inline map::Data* ObjectTableWidgetPair::data() const
{
	return m_data;
}

inline const map::Value& ObjectTableWidgetPair::defaultValue() const
{
	return data()->left.find(metaData())->second;
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
