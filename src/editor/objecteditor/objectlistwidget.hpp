/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_OBJECTLISTWIDGET_HPP
#define WC3LIB_EDITOR_OBJECTLISTWIDGET_HPP

#include <QWidget>

#include "ui_objectlistwidget.h"
#include "objectdata.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

class ObjectListWidget : public QWidget, protected Ui::ObjectListWidget
{
	Q_OBJECT

	public slots:
		void addObject();
		
	public:
		typedef QList<ObjectData::ObjectId> ObjectIds;
		explicit ObjectListWidget(MpqPriorityList *source, ObjectData *objectData, QWidget* parent = 0, Qt::WindowFlags f = 0);

		void load(ObjectIds &objects);
		ObjectIds objects() const;

		MpqPriorityList* source() const;
		ObjectData* objectData() const;

	private:
		MpqPriorityList *m_source;
		ObjectData *m_objectData;
};

inline MpqPriorityList* ObjectListWidget::source() const
{
	return this->m_source;
}

inline ObjectData* ObjectListWidget::objectData() const
{
	return this->m_objectData;
}

}

}

#endif // WC3LIB_EDITOR_OBJECTLISTWIDGET_HPP
