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

#ifndef WC3LIB_EDITOR_UNITSELECTIONDIALOG_HPP
#define WC3LIB_EDITOR_UNITSELECTIONDIALOG_HPP

#include <QDialog>
#include <QHash>
#include <QString>

#include <KPushButton>

#include "ui_unitselectiondialog.h"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class UnitData;

/**
 * \brief An icon listing dialog which allows the user to select a standard unit.
 *
 * The units are ordered by their race.
 */
class UnitSelectionDialog : public QDialog, protected Ui::UnitSelectionDialog
{
	Q_OBJECT

	public:
		typedef QHash<KPushButton*, QString> Buttons;

		explicit UnitSelectionDialog(MpqPriorityList *source, UnitData *unitData, QWidget* parent = 0, Qt::WindowFlags f = 0);

		void select(const QString &objectId);

		MpqPriorityList* source() const;
		UnitData* unitData() const;

		QString unitName() const;
		const QString& originalObjectId() const;

		void clear();

	private:
		KPushButton* createButton(const QString &objectId);

		MpqPriorityList *m_source;
		UnitData *m_unitData;
		QString m_originalObjectId;
		Buttons m_buttons;

	private slots:
		void selectButton();
};

inline MpqPriorityList* UnitSelectionDialog::source() const
{
	return this->m_source;
}

inline UnitData* UnitSelectionDialog::unitData() const
{
	return this->m_unitData;
}

inline QString UnitSelectionDialog::unitName() const
{
	return this->m_nameLineEdit->text();
}

inline const QString& UnitSelectionDialog::originalObjectId() const
{
	return this->m_originalObjectId;
}

}

}

#endif // WC3LIB_EDITOR_UNITSELECTIONDIALOG_HPP
