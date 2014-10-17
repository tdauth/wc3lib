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

#ifndef WC3LIB_EDITOR_OBJECTLISTDIALOG_HPP
#define WC3LIB_EDITOR_OBJECTLISTDIALOG_HPP

#include <QDialog>

#include "ui_objectlistdialog.h"
#include "objectdata.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class SharedObjectData;

/**
 * \brief Dialog to select a list of objects.
 */
class ObjectListDialog : public QDialog, protected Ui::ObjectListDialog
{
	Q_OBJECT

	public slots:
		void addObject();
		void removeObject();
		void moveObjectUp();
		void moveObjectDown();

	public:
		explicit ObjectListDialog(MpqPriorityList *source, ObjectData *objectData, QWidget* parent = 0, Qt::WindowFlags f = 0);

		void load(const QStringList &objects);
		QStringList objects() const;

		void setSource(MpqPriorityList *source);
		MpqPriorityList* source() const;
		void setObjectData(ObjectData *objectData);
		ObjectData* objectData() const;

		/**
		 * Lists object IDs depending on the value of field \p fieldId of object with IDs \p originalObjectId and \p customObjectId.
		 */
		static int getObjectIds(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, ObjectData *objectData, SharedObjectData *sharedObjectData, const QString &label, QWidget* parent = 0, Qt::WindowFlags f = 0);
		static int getObjectIds(QStringList &result, MpqPriorityList *source, ObjectData *objectData, const QString &label, QWidget* parent = 0, Qt::WindowFlags f = 0);
	private:
		MpqPriorityList *m_source;
		ObjectData *m_objectData;

		static ObjectListDialog *m_dialog;
};

inline void ObjectListDialog::setSource(MpqPriorityList* source)
{
	this->m_source = source;
}

inline MpqPriorityList* ObjectListDialog::source() const
{
	return this->m_source;
}

inline void ObjectListDialog::setObjectData(ObjectData* objectData)
{
	this->m_objectData = objectData;
}

inline ObjectData* ObjectListDialog::objectData() const
{
	return this->m_objectData;
}

}

}

#endif // WC3LIB_EDITOR_OBJECTLISTDIALOG_HPP
