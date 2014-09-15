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

UnitSelectionDialog::UnitSelectionDialog(MpqPriorityList *source, UnitData *unitData, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_source(source), m_unitData(unitData), m_buttonGroup(new QButtonGroup(this))
{
	setupUi(this);
	m_buttonGroup.setExclusive(true);

	const ObjectData::ObjectTabEntries raceEntries = unitData->objectTabEntries("unitRace");

	foreach (ObjectData::ObjectTabEntry entry, raceEntries)
	{
		this->m_raceComboBox->addItem(source->sharedData()->tr(entry.second), entry.first);
	}

	connect(this->m_raceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRace(int)));

	m_meleeComboBox->addItem(source->sharedData()->tr("WESTRING_MELEE"));
	m_meleeComboBox->addItem(source->sharedData()->tr("WESTRING_CAMPAIGN"));

	connect(this->m_meleeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMelee(int)));

	map::Txt::Section *tilesetsSection =  this->source()->sharedData()->worldEditData()->section("TileSets");

	for (std::size_t i = 0; i < tilesetsSection->entries.size(); ++i)
	{
		const map::Txt::Entry &entry = tilesetsSection->entries[i];
		QStringList list = QString::fromUtf8(entry.second.c_str()).split(',');
		m_tilesetComboBox->addItem(source->sharedData()->tr(list[0]), QChar(entry.first[0]));
	}

	connect(this->m_tilesetComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTileset(int)));

	this->m_levelComboBox->addItem(source->sharedData()->tr("WESTRING_ANYLEVEL"), "*");

	for (int i = 1; i < 11; ++i)
	{
		this->m_levelComboBox->addItem(tr("Stufe %1").arg(i), QString::number(i));
	}

	connect(this->m_levelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLevel(int)));

	connect(this->m_nameLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
}

void UnitSelectionDialog::clear()
{
	for (ButtonsByButton::iterator iterator = this->m_buttonsByButtons.begin(); iterator != this->m_buttonsByButtons.end(); ++iterator)
	{
		delete iterator.key();
	}

	m_buttonsByButtons.clear();
	m_buttonsByObjectId.clear();
}

void UnitSelectionDialog::select(const QString &objectId)
{
	this->clear();
	this->m_dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	const QString race = this->unitData()->unitData()->value(objectId, "race");
	QString campaign;

	if (this->unitData()->hasDefaultFieldValue(objectId, "ucam"))
	{
		campaign = this->unitData()->fieldValue(objectId, "", "ucam");
	}

	QString objectTilesets = unitData()->objectTilesets(objectId, "");

	QChar objectTileset;

	if (unitData()->showTilesetForRace(race) &&!objectTilesets.isEmpty())
	{
		objectTileset = objectTilesets[0];
	}

	QString objectLevel;

	if (unitData()->showLevelForRace(race) && this->unitData()->hasDefaultFieldValue(objectId, "ulev"))
	{
		objectLevel = this->unitData()->fieldValue(objectId, "", "ulev");
	}

	this->fill(race, campaign.toInt(), objectTileset, objectLevel);
	ButtonsByObjectId::iterator iterator = m_buttonsByObjectId.find(objectId);

	if (iterator != this->m_buttonsByObjectId.end())
	{
		iterator.value()->setChecked(true);
	}
	else
	{
		qDebug() << "Error: No button for" << objectId;
	}
}

void UnitSelectionDialog::fill(const QString& race, int campaign, const QChar& tileset, const QString& level)
{
	this->clear();
	this->m_dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!this->m_nameLineEdit->text().isEmpty());
	this->m_tilesetComboBox->setVisible(!tileset.isNull());
	this->m_levelComboBox->setVisible(!level.isEmpty());

	const int maxColumns = 12;
	int unitRow = 0;
	int unitColumn = 0;
	int heroRow = 0;
	int heroColumn = 0;

	for (map::Slk::Table::size_type row = 1; row < this->unitData()->unitData()->slk().rows(); ++row)
	{
		const QString unitId = this->unitData()->unitData()->value(row, "unitID");
		qDebug() << "ID" << unitId;
		const QString objectRace = this->unitData()->unitData()->value(unitId, "race");
		qDebug() << "Race" << objectRace;
		QString unitCampaign;

		if (this->unitData()->hasDefaultFieldValue(unitId, "ucam"))
		{
			unitCampaign = this->unitData()->fieldValue(unitId, "", "ucam");
		}

		QString objectTilesets = unitData()->objectTilesets(unitId, "");

		QString objectLevel;

		if (this->unitData()->hasDefaultFieldValue(unitId, "ulev"))
		{
			objectLevel = this->unitData()->fieldValue(unitId, "", "ulev");
		}

		if (objectRace == race && campaign == unitCampaign.toInt() && (tileset.isNull() || tileset == '*' || objectTilesets == "*" || objectTilesets.contains(tileset)) && (level.isEmpty() || level == "*" || objectLevel == level))
		{
			KPushButton *button = createButton(unitId);
			m_buttonsByButtons.insert(button, unitId);
			m_buttonsByObjectId.insert(unitId, button);
			m_buttonGroup.addButton(button);
			connect(button, SIGNAL(clicked(bool)), this, SLOT(selectButton(bool)));

			if (this->unitData()->objectIsHero(unitId, ""))
			{
				this->m_heroesGridLayout->addWidget(button, heroRow, heroColumn);
				heroColumn++;

				if (heroColumn == maxColumns)
				{
					heroColumn = 0;
					heroRow++;
				}
			}
			else
			{
				this->m_unitsGridLayout->addWidget(button, unitRow, unitColumn);
				unitColumn++;

				if (unitColumn == maxColumns)
				{
					unitColumn = 0;
					unitRow++;
				}
			}
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
	button->setMaximumSize(QSize(32, 32));
	button->setMinimumSize(QSize(32, 32));
	button->setIconSize(QSize(32, 32));

	if (!art.isEmpty())
	{
		button->setIcon(this->unitData()->source()->sharedData()->icon(KUrl(art), this));
	}

	return button;
}

void UnitSelectionDialog::update()
{
	const QString race = this->m_raceComboBox->itemData(this->m_raceComboBox->currentIndex()).toString();
	QChar currentTileset;

	if (this->m_tilesetComboBox->isVisible())
	{
		currentTileset = this->m_tilesetComboBox->itemData(this->m_tilesetComboBox->currentIndex()).toChar();
	}

	QString level;

	if (this->m_levelComboBox->isVisible())
	{
		level = this->m_levelComboBox->itemData(this->m_levelComboBox->currentIndex()).toString();
	}

	this->fill(race, this->m_meleeComboBox->currentIndex(), currentTileset, level);
	this->m_buttonGroup.buttons().first()->setChecked(true);
}

void UnitSelectionDialog::selectButton(bool checked)
{
	if (checked)
	{
		KPushButton *button = dynamic_cast<KPushButton*>(QObject::sender());
		ButtonsByButton::iterator iterator = this->m_buttonsByButtons.find(button);

		if (iterator != this->m_buttonsByButtons.end())
		{
			this->m_originalObjectId = iterator.value();
		}
	}
}

void UnitSelectionDialog::nameChanged(const QString& name)
{
	this->m_dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!name.isEmpty());
}

void UnitSelectionDialog::changeRace(int index)
{
	this->update();
}

void UnitSelectionDialog::changeMelee(int index)
{
	this->update();
}

void UnitSelectionDialog::changeTileset(int index)
{
	this->update();
}

void UnitSelectionDialog::changeLevel(int index)
{
	this->update();
}

}

}
