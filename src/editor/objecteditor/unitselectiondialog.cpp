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

#include <KIconButton>

#include "unitselectiondialog.hpp"
#include "unitdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

UnitSelectionDialog::UnitSelectionDialog(MpqPriorityList *source, UnitData *unitData, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_source(source), m_unitData(unitData)
{
	setupUi(this);

	const ObjectData::ObjectTabEntries raceEntries = unitData->objectTabEntries("unitRace");

	foreach (ObjectData::ObjectTabEntry entry, raceEntries)
	{
		this->m_raceComboBox->addItem(source->sharedData()->tr(entry.second), entry.first);
	}
}

void UnitSelectionDialog::clear()
{
	for (Buttons::iterator iterator = this->m_buttons.begin(); iterator != this->m_buttons.end(); ++iterator)
	{
		delete iterator.key();
	}

	m_buttons.clear();
}

void UnitSelectionDialog::select(const QString &objectId)
{
	this->clear();

	const QString race = MetaData::fromSlkString(this->unitData()->unitData()->value(MetaData::toSlkString(objectId), MetaData::toSlkString("race")));

	// list humans
	if (race == "human")
	{
	}

	const int maxColumns = 12;
	int unitRow = 0;
	int unitColumn = 0;
	int heroRow = 0;
	int heroColumn = 0;

	for (map::Slk::Table::size_type row = 1; row < this->unitData()->unitData()->slk().rows(); ++row)
	{
		const QString id = MetaData::fromSlkString(this->unitData()->unitData()->value(row, MetaData::toSlkString("unitID")));
		qDebug() << "ID" << id;
		const QString objectRace =  MetaData::fromSlkString(this->unitData()->unitData()->value(MetaData::toSlkString(id), MetaData::toSlkString("race")));
		qDebug() << "Race" << objectRace;

		if (objectRace == race)
		{
			KPushButton *button = createButton(id);
			m_buttons.insert(button, id);
			connect(button, SIGNAL(pressed()), this, SLOT(selectButton()));

			if (this->unitData()->objectIsUnit(id, ""))
			{
				this->m_unitsGridLayout->addWidget(button, unitRow, unitColumn);
				unitColumn++;

				if (unitColumn == maxColumns)
				{
					unitColumn = 0;
					unitRow++;
				}
			}
			/*
			else if (this->unitData()->objectIsHero(id))
			{
				this->m_heroesGridLayout->addWidget(button, heroRow, heroColumn);
				heroColumn++;

				if (heroColumn == maxColumns)
				{
					heroColumn = 0;
					heroRow++;
				}
			}
			*/
		}
	}
}

KPushButton* UnitSelectionDialog::createButton(const QString &objectId)
{
	const QString art = MetaData::fromFilePath(this->unitData()->defaultFieldValue(objectId, "uico"));
	const QString name = this->unitData()->defaultFieldValue(objectId, "unam");
	const QString ubertip = this->unitData()->defaultFieldValue(objectId, "utub");
	KPushButton *button = new KPushButton(this);
	button->setToolTip(tr("%1\n--\n%2").arg(name).arg(ubertip));
	button->setCheckable(true);

	if (!art.isEmpty())
	{
		QString iconFile;

		if (source()->download(art, iconFile, this))
		{
			button->setIconSize(QSize(64, 64));
			button->setIcon(KIcon(iconFile));
		}
	}

	return button;
}

void UnitSelectionDialog::selectButton()
{
	KPushButton *button = dynamic_cast<KPushButton*>(QObject::sender());
	Buttons::iterator iterator = this->m_buttons.find(button);

	if (iterator != this->m_buttons.end())
	{
		this->m_originalObjectId = iterator.value();
	}
}

}

}
