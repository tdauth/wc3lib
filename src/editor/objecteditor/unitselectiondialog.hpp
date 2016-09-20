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
#include <QButtonGroup>
#include <QPushButton>

#include "ui_unitselectiondialog.h"

#include "../../map.hpp"

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
 *
 * \ingroup objectdata
 */
class UnitSelectionDialog : public QDialog, protected Ui::UnitSelectionDialog
{
	Q_OBJECT

	public:
		typedef QHash<QAbstractButton*, QString> ButtonsByButton;
		typedef QHash<QString, QAbstractButton*> ButtonsByObjectId;

		explicit UnitSelectionDialog(MpqPriorityList *source, UnitData *unitData, QWidget* parent = 0, Qt::WindowFlags f = 0);

		void select(const QString &objectId);
		void fill(const QString &race, int campaign, const QChar &tileset, const QString &level);

		MpqPriorityList* source() const;
		UnitData* unitData() const;

		QString unitName() const;
		/**
		 * \return Returns the original object ID of the currently selected object (unit).
		 */
		const QString& originalObjectId() const;

		void clear();

	private:
		QPushButton* createButton(const QString &objectId);
		/**
		 * Checks button \p button and sets the basic unit name to the one of the checked button.
		 * Besides it updates the OK button and enables it if the name is not empty.
		 */
		void checkButton(QAbstractButton *button);

		int raceIndex(const QString &race) const;
		int campaignIndex(int campaign) const;

		/**
		 * \return Returns the corresponding tileset combo box index of tileset \p tileset.
		 */
		int tilesetIndex(const QChar &tileset) const;
		/**
		 * \return Returns the corresponding level combo box index of level \p level.
		 */
		int levelIndex(const QString &level) const;

		/**
		 * Updates the currently displayed units depending on the selected combo box items.
		 */
		void update();

		/**
		 * \return Returns the section "TileSets" from the "UI/WorldEditorData.txt" file.
		 */
		const map::Txt::Section* tilesetsSection() const;

		MpqPriorityList *m_source;
		UnitData *m_unitData;
		QString m_originalObjectId;
		ButtonsByButton m_buttonsByButtons;
		ButtonsByObjectId m_buttonsByObjectId;
		QButtonGroup m_buttonGroup;

		QAbstractButton *m_checkedButton;

	private slots:
		/**
		 * It changes the original object ID returned by \ref originalObjectId().
		 */
		void selectButton(bool checked);
		void nameChanged(const QString &name);
		void changeRace(int index);
		void changeMelee(int index);
		void changeTileset(int index);
		void changeLevel(int index);
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
