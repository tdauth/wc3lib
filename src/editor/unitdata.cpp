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

#include <QtGui>

#include "unitdata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"


namespace wc3lib
{

namespace editor
{

UnitData::UnitData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds UnitData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (this->unitData() != 0 && !this->unitData()->isEmpty())
	{
		for (int row = 1; row < this->unitData()->rows(); ++row)
		{
			result << this->unitData()->value(row, "unitID");
		}
	}

	return result;
}

UnitData::MetaDataList UnitData::resolveDefaultField(const QString &objectId, const QString &fieldId, int level) const
{
	MetaDataList result;

	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->unitMetaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->unitMetaData()->value(fieldId, "slk");

		// TODO improve performance by not calling the hasValue() methods?
		if (slk == "UnitUI")
		{
			result.push_back(this->unitUi());
		}
		else if (slk == "UnitData")
		{
			result.push_back(this->unitData());
		}
		else if (slk == "UnitBalance")
		{
			result.push_back(this->unitBalance());
		}
		else if (slk == "UnitWeapons")
		{
			result.push_back(this->unitWeapons());
		}
		else if (slk == "UnitAbilities")
		{
			result.push_back(this->unitAbilities());
		}
		/*
		* Profile means to use a TXT file from the corresponding race.
		*/
		else if (slk == "Profile")
		{
			// TODO in Frozen Throne we can check if unit is in campaign but in Reign of Chaos there is no such field.
			result.push_back(this->campaignUnitStrings());
			result.push_back(this->campaignUnitFunc());

			if (this->unitData()->hasValue(objectId, "race"))
			{
				const QString race = this->unitData()->value(objectId, "race");

				if (race == "human")
				{
					result.push_back(this->humanUnitStrings());
					result.push_back(this->humanUnitFunc());
				}
				else if (race == "orc")
				{
					result.push_back(this->orcUnitStrings());
					result.push_back(this->orcUnitFunc());
				}
				else if (race == "nightelf")
				{
					result.push_back(this->nightElfUnitStrings());
					result.push_back(this->nightElfUnitFunc());
				}
				else if (race == "undead")
				{
					result.push_back(this->undeadUnitStrings());
					result.push_back(this->undeadUnitFunc());
				}
				else
				{
					result.push_back(this->neutralUnitStrings());
					result.push_back(this->neutralUnitFunc());
				}
			}
		}
	}

	return result;
}

bool UnitData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	const QString useUnit = this->metaData()->value(fieldId, "useUnit");
	const QString useHero = this->metaData()->value(fieldId, "useHero");
	const QString useBuilding = this->metaData()->value(fieldId, "useBuilding");

	return (!objectIsHero(originalObjectId, customObjectId) || useHero != "1") && (!objectIsBuilding(originalObjectId, customObjectId) || useBuilding != "1") && (!objectIsUnit(originalObjectId, customObjectId) || useUnit != "1");
}

bool UnitData::hasCustomUnits() const
{
	return true;
}

bool UnitData::hasCustomObjects() const
{
	return true;
}

bool UnitData::hasMetaDataList() const
{
	// TODO support
	return false;
}

ObjectData::MetaDataList UnitData::metaDataList() const
{
	return ObjectData::MetaDataList();
}

QString UnitData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	QString name = fieldReadableValue(originalObjectId, customObjectId, "unam");

	if (this->hasFieldValue(originalObjectId, customObjectId, "unsf"))
	{
		const QString suffix = this->fieldReadableValue(originalObjectId, customObjectId, "unsf");

		if (suffix.startsWith('('))
		{
			name = QObject::tr("%1 %2").arg(name).arg(suffix);
		}
		else
		{
			name = QObject::tr("%1 (%2)").arg(name).arg(suffix);
		}
	}

	return name;
}

QIcon UnitData::objectIcon(const QString &originalObjectId, const QString &customObjectId, QWidget *window) const
{
	const QString art = this->fieldValue(originalObjectId, customObjectId, "uico");

	if (!art.isEmpty())
	{
		return this->source()->sharedData()->icon(art, window);
	}

	return this->source()->sharedData()->worldEditDataIcon("InvalidIcon", "WorldEditArt", window);
}

int UnitData::objectLevels(const QString& originalObjectId, const QString& customObjectId) const
{
	return 1;
}

void UnitData::load(QWidget *widget)
{
	this->m_unitMetaData.reset(new MetaData(QUrl("Units/UnitMetaData.slk")));
	this->m_unitMetaData->setSource(this->source());
	this->m_unitMetaData->load();

	this->m_unitData.reset(new MetaData(QUrl("Units/UnitData.slk")));
	this->m_unitData->setSource(this->source());
	this->m_unitData->load();

	this->m_unitUi.reset(new MetaData(QUrl("Units/unitUI.slk")));
	this->m_unitUi->setSource(this->source());
	this->m_unitUi->load();

	this->m_unitBalance.reset(new MetaData(QUrl("Units/UnitBalance.slk")));
	this->m_unitBalance->setSource(this->source());
	this->m_unitBalance->load();

	this->m_unitWeapons.reset(new MetaData(QUrl("Units/UnitWeapons.slk")));
	this->m_unitWeapons->setSource(this->source());
	this->m_unitWeapons->load();

	this->m_unitAbilities.reset(new MetaData(QUrl("Units/UnitAbilities.slk")));
	this->m_unitAbilities->setSource(this->source());
	this->m_unitAbilities->load();

	this->m_humanUnitStrings.reset(new MetaData(QUrl("Units/HumanUnitStrings.txt")));
	this->m_humanUnitStrings->setSource(this->source());
	this->m_humanUnitStrings->load();

	this->m_humanUnitFunc.reset(new MetaData(QUrl("Units/HumanUnitFunc.txt")));
	this->m_humanUnitFunc->setSource(this->source());
	this->m_humanUnitFunc->load();

	this->m_orcUnitStrings.reset(new MetaData(QUrl("Units/OrcUnitStrings.txt")));
	this->m_orcUnitStrings->setSource(this->source());
	this->m_orcUnitStrings->load();

	this->m_orcUnitFunc.reset(new MetaData(QUrl("Units/OrcUnitFunc.txt")));
	this->m_orcUnitFunc->setSource(this->source());
	this->m_orcUnitFunc->load();

	this->m_undeadUnitStrings.reset(new MetaData(QUrl("Units/UndeadUnitStrings.txt")));
	this->m_undeadUnitStrings->setSource(this->source());
	this->m_undeadUnitStrings->load();


	this->m_undeadUnitFunc.reset(new MetaData(QUrl("Units/UndeadUnitFunc.txt")));
	this->m_undeadUnitFunc->setSource(this->source());
	this->m_undeadUnitFunc->load();

	this->m_nightElfUnitStrings.reset(new MetaData(QUrl("Units/NightElfUnitStrings.txt")));
	this->m_nightElfUnitStrings->setSource(this->source());
	this->m_nightElfUnitStrings->load();

	this->m_nightElfUnitFunc.reset(new MetaData(QUrl("Units/NightElfUnitFunc.txt")));
	this->m_nightElfUnitFunc->setSource(this->source());
	this->m_nightElfUnitFunc->load();

	this->m_neutralUnitStrings.reset(new MetaData(QUrl("Units/NeutralUnitStrings.txt")));
	this->m_neutralUnitStrings->setSource(this->source());
	this->m_neutralUnitStrings->load();

	this->m_neutralUnitFunc.reset(new MetaData(QUrl("Units/NeutralUnitFunc.txt")));
	this->m_neutralUnitFunc->setSource(this->source());
	this->m_neutralUnitFunc->load();

	this->m_campaignUnitStrings.reset(new MetaData(QUrl("Units/CampaignUnitStrings.txt")));
	this->m_campaignUnitStrings->setSource(this->source());
	this->m_campaignUnitStrings->load();

	this->m_campaignUnitFunc.reset(new MetaData(QUrl("Units/CampaignUnitFunc.txt")));
	this->m_campaignUnitFunc->setSource(this->source());
	this->m_campaignUnitFunc->load();
}

MetaData* UnitData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

bool UnitData::objectIsBuilding(const QString &originalObjectId, const QString &customObjectId) const
{
	/*
	 * In Frozen Throne there is an additional field which indicates if the unit is a building.
	 */
	if (this->hasDefaultFieldValue(originalObjectId, "ubdg"))
	{
		if (this->fieldValue(originalObjectId, customObjectId, "ubdg") == "1")
		{
			return true;
		}
	}

	/*
	 * Reign of Chaos
	 */
	bool ok = true;

	/*
	 * Objects without a level are buildings.
	 */
	if (this->hasDefaultFieldValue(originalObjectId, "ulev"))
	{
		const QString levelVar = this->defaultFieldValue(originalObjectId, "ulev");
		levelVar.toInt(&ok);
	}

	return !ok;
}

bool UnitData::objectIsSpecial(const QString &originalObjectId, const QString &customObjectId) const
{
	/*
	 * In Frozen Throne there is an additional field which indicates if the unit is special.
	 */
	if (this->hasDefaultFieldValue(originalObjectId, "uspe"))
	{
		if (this->fieldValue(originalObjectId, customObjectId, "uspe") == "1")
		{
			return true;
		}
	}

	/*
	 * Reign of Chaos
	 */
	bool ok = true;

	if (this->hasDefaultFieldValue(originalObjectId, "special"))
	{
		const QString specialVar = this->defaultFieldValue(originalObjectId, "special");
		specialVar.toInt(&ok);
	}

	return !ok;
}

bool UnitData::objectIsHero(const QString &originalObjectId, const QString &customObjectId) const
{
	bool ok = false;

	/*
	 * Objects with strength value are heroes.
	 */
	if (this->hasDefaultFieldValue(originalObjectId, "ustr"))
	{
		const QString strengthVar = this->defaultFieldValue(originalObjectId, "ustr");
		strengthVar.toInt(&ok);
	}

	return ok;
}

bool UnitData::objectIsUnit(const QString &originalObjectId, const QString &customObjectId) const
{
	return unitData()->hasValue(originalObjectId, "unitID") && !objectIsHero(originalObjectId, customObjectId) && !objectIsBuilding(originalObjectId, customObjectId);
}

QString UnitData::objectTilesets(const QString &originalObjectId, const QString &customObjectId) const
{
	/*
	 * Frozen Throne
	 */
	if (this->metaData()->hasValue("util", "ID"))
	{
		return this->fieldValue(originalObjectId, customObjectId, "util");
	}

	/*
	 * Reign of Chaos
	 */
	if (customObjectId.isEmpty())
	{
		return this->unitUi()->value(originalObjectId, "tilesets");
	}

	return QString();
}

bool UnitData::showTilesetForRace(const QString &race) const
{
	return showLevelForRace(race);
}

bool UnitData::showLevelForRace(const QString &race) const
{
	return race == "creeps" || race == "other" || race == "demon" || race == "critters" || race == "common" || race == "other";
}

}

}
