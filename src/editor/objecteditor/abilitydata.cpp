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

#include <QtCore>

#include <KMessageBox>

#include "abilitydata.hpp"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

AbilityData::AbilityData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds AbilityData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (this->abilityData() != 0 && !this->abilityData()->isEmpty())
	{
		for (map::Slk::Table::size_type row = 1; row < this->abilityData()->rows(); ++row)
		{
			result << this->abilityData()->value(row, "alias");
		}
	}

	return result;
}

QString AbilityData::defaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	if (this->metaData()->hasValue(fieldId, "slk") && this->metaData()->hasValue(fieldId, "field"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");
		const QString field = this->metaData()->value(fieldId, "field");

		if (slk == "AbilityData")
		{
			if (this->abilityData()->hasValue(objectId, field))
			{
				return this->abilityData()->value(objectId, field);
			}
		}
		else if (slk == "Profile")
		{
			const QString race = this->abilityData()->value(objectId, "race");
			const QString sort = this->abilityData()->value(objectId, "sort");

			if (sort == "item")
			{
				if (this->itemAbilityStrings()->hasValue(objectId, field))
				{
					return this->itemAbilityStrings()->value(objectId, field);
				}

				if (this->itemAbilityFunc()->hasValue(objectId, field))
				{
					return this->itemAbilityFunc()->value(objectId, field);
				}
			}
			else
			{
				// TODO in Frozen Throne we can check if unit is in campaign but in Reign of Chaos there is no such field.
				if (this->campaignAbilityStrings()->hasValue(objectId, field))
				{
					return this->campaignAbilityStrings()->value(objectId, field);
				}
				else if (this->campaignAbilityFunc()->hasValue(objectId, field))
				{
					return this->campaignAbilityFunc()->value(objectId, field);
				}
				else if (race == "human")
				{
					if (this->humanAbilityStrings()->hasValue(objectId, field))
					{
						return this->humanAbilityStrings()->value(objectId, field);
					}

					if (this->humanAbilityFunc()->hasValue(objectId, field))
					{
						return this->humanAbilityFunc()->value(objectId, field);
					}
				}
				else if (race == "orc")
				{
					if (this->orcAbilityStrings()->hasValue(objectId, field))
					{
						return this->orcAbilityStrings()->value(objectId, field);
					}

					if (this->orcAbilityFunc()->hasValue(objectId, field))
					{
						return this->orcAbilityFunc()->value(objectId, field);
					}
				}
				else if (race == "nightelf")
				{
					if (this->nightElfAbilityStrings()->hasValue(objectId, field))
					{
						return this->nightElfAbilityStrings()->value(objectId, field);
					}

					if (this->nightElfAbilityFunc()->hasValue(objectId, field))
					{
						return this->nightElfAbilityFunc()->value(objectId, field);
					}
				}
				else if (race == "undead")
				{
					if (this->undeadAbilityStrings()->hasValue(objectId, field))
					{
						return this->undeadAbilityStrings()->value(objectId, field);
					}

					if (this->undeadAbilityFunc()->hasValue(objectId, field))
					{
						return this->undeadAbilityFunc()->value(objectId, field);
					}
				}
				else if (race == "neutral")
				{
					if (this->neutralAbilityStrings()->hasValue(objectId, field))
					{
						return this->neutralAbilityStrings()->value(objectId, field);
					}

					if (this->neutralAbilityFunc()->hasValue(objectId, field))
					{
						return this->neutralAbilityFunc()->value(objectId, field);
					}
				}
				else
				{
					if (this->commonAbilityStrings()->hasValue(objectId, field))
					{
						return this->commonAbilityStrings()->value(objectId, field);
					}

					if (this->commonAbilityFunc()->hasValue(objectId, field))
					{
						return this->commonAbilityFunc()->value(objectId, field);
					}
				}
			}

			qDebug() << "Missing value" << fieldId << "for object" << objectId << "with slk" << slk << "and race" << race << "and sort" << sort;
		}
	}

	qDebug() << "Missing value" << fieldId << "for object" << objectId;

	return QString();
}

bool AbilityData::hasDefaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	if (this->metaData()->hasValue(fieldId, "slk") && this->metaData()->hasValue(fieldId, "field"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");
		const QString field = this->metaData()->value(fieldId, "field");

		if (slk == "AbilityData")
		{
			return this->abilityData()->hasValue(objectId, field);
		}
		else if (slk == "Profile")
		{
			const QString race = this->abilityData()->value(objectId, "race");
			const QString sort = this->abilityData()->value(objectId, "sort");

			if (sort == "item")
			{
				return this->itemAbilityStrings()->hasValue(objectId, field) || this->itemAbilityFunc()->hasValue(objectId, field);
			}
			else
			{
				// TODO in Frozen Throne we can check if unit is in campaign but in Reign of Chaos there is no such field.
				if (this->campaignAbilityStrings()->hasValue(objectId, field))
				{
					return true;
				}
				else if (this->campaignAbilityFunc()->hasValue(objectId, field))
				{
					return true;
				}
				else if (race == "human")
				{
					if (this->humanAbilityStrings()->hasValue(objectId, field))
					{
						return true;
					}

					if (this->humanAbilityFunc()->hasValue(objectId, field))
					{
						return true;
					}
				}
				else if (race == "orc")
				{
					if (this->orcAbilityStrings()->hasValue(objectId, field))
					{
						return true;
					}

					if (this->orcAbilityFunc()->hasValue(objectId, field))
					{
						return true;
					}
				}
				else if (race == "nightelf")
				{
					if (this->nightElfAbilityStrings()->hasValue(objectId, field))
					{
						return true;
					}

					if (this->nightElfAbilityFunc()->hasValue(objectId, field))
					{
						return true;
					}
				}
				else if (race == "undead")
				{
					if (this->undeadAbilityStrings()->hasValue(objectId, field))
					{
						return true;
					}

					if (this->undeadAbilityFunc()->hasValue(objectId, field))
					{
						return true;
					}
				}
				else if (race == "neutral")
				{
					if (this->neutralAbilityStrings()->hasValue(objectId, field))
					{
						return true;
					}

					if (this->neutralAbilityFunc()->hasValue(objectId, field))
					{
						return true;
					}
				}
				else
				{
					if (this->commonAbilityStrings()->hasValue(objectId, field))
					{
						return true;
					}

					if (this->commonAbilityFunc()->hasValue(objectId, field))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

map::CustomObjects::Type AbilityData::type() const
{
	return map::CustomObjects::Type::Abilities;
}

bool AbilityData::hasCustomUnits() const
{
	return false;
}

bool AbilityData::hasCustomObjects() const
{
	return true;
}

bool AbilityData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList AbilityData::metaDataList() const
{
	// TODO support slks
	return ObjectData::MetaDataList();
}

bool AbilityData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const
{
	/*
	 * Ability fields might be for specific abilities only.
	 * The field "useSpecific" lists all abilities for which the field is shown.
	 * If the field is empty it is shown for all abilities.
	 */
	if (this->metaData()->hasValue(fieldId, "useSpecific"))
	{
		const QString useSpecific = this->metaData()->value(fieldId, "useSpecific");

		if (!useSpecific.isEmpty())
		{
			const QStringList specificAbilityIds = useSpecific.split(',');
			bool found = false;

			foreach (QString abilityId, specificAbilityIds)
			{
				if (originalObjectId == abilityId)
				{
					found = true;

					break;
				}
			}

			/*
			 * Field is not for this ability.
			 */
			if (!found)
			{
				return true;
			}
		}
	}

	const bool isHero = objectIsHero(originalObjectId, customObjectId);
	const bool isUnit = objectIsUnit(originalObjectId, customObjectId);
	const bool isItem = objectIsItem(originalObjectId, customObjectId);
	bool useHero = false;
	bool useUnit = false;
	bool useItem = false;

	if (this->metaData()->hasValue(fieldId, "useHero"))
	{
		if (this->metaData()->value(fieldId, "useHero") == "1")
		{
			useHero = true;
		}
	}

	if (this->metaData()->hasValue(fieldId, "useUnit"))
	{
		if (this->metaData()->value(fieldId, "useUnit") == "1")
		{
			useUnit = true;
		}
	}

	if (this->metaData()->hasValue(fieldId, "useItem"))
	{
		if (this->metaData()->value(fieldId, "useItem") == "1")
		{
			useItem = true;
		}
	}

	return (!isHero || !useHero) && (!isUnit || !useUnit) && (!isItem || !useItem);
}

QString AbilityData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	QString name = fieldReadableValue(originalObjectId, customObjectId, "anam");

	if (this->hasFieldValue(originalObjectId, customObjectId, "ansf"))
	{
		const QString suffix = this->fieldReadableValue(originalObjectId, customObjectId, "ansf");

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

MetaData* AbilityData::objectTabData() const
{
	// TODO needs "UI/UnitEditorData.txt"? share with unit data.
	return 0;
}

void AbilityData::load(QWidget *widget)
{
	// TODO same meta data as for units -> share it!
	this->m_metaData.reset(new MetaData(KUrl("Units/AbilityMetaData.slk")));
	this->m_metaData->setSource(this->source());
	this->m_metaData->load();
	this->m_abilityData.reset(new MetaData(KUrl("Units/AbilityData.slk")));
	this->m_abilityData->setSource(this->source());
	this->m_abilityData->load();

	this->m_itemAbilityFunc.reset(new MetaData(KUrl("Units/ItemAbilityFunc.txt")));
	this->m_itemAbilityFunc->setSource(this->source());
	this->m_itemAbilityFunc->load();
	this->m_itemAbilityStrings.reset(new MetaData(KUrl("Units/ItemAbilityStrings.txt")));
	this->m_itemAbilityStrings->setSource(this->source());
	this->m_itemAbilityStrings->load();

	this->m_humanAbilityFunc.reset(new MetaData(KUrl("Units/HumanAbilityFunc.txt")));
	this->m_humanAbilityFunc->setSource(this->source());
	this->m_humanAbilityFunc->load();
	this->m_humanAbilityStrings.reset(new MetaData(KUrl("Units/HumanAbilityStrings.txt")));
	this->m_humanAbilityStrings->setSource(this->source());
	this->m_humanAbilityStrings->load();

	this->m_orcAbilityFunc.reset(new MetaData(KUrl("Units/OrcAbilityFunc.txt")));
	this->m_orcAbilityFunc->setSource(this->source());
	this->m_orcAbilityFunc->load();
	this->m_orcAbilityStrings.reset(new MetaData(KUrl("Units/OrcAbilityStrings.txt")));
	this->m_orcAbilityStrings->setSource(this->source());
	this->m_orcAbilityStrings->load();

	this->m_undeadAbilityFunc.reset(new MetaData(KUrl("Units/UndeadAbilityFunc.txt")));
	this->m_undeadAbilityFunc->setSource(this->source());
	this->m_undeadAbilityFunc->load();
	this->m_undeadAbilityStrings.reset(new MetaData(KUrl("Units/UndeadAbilityStrings.txt")));
	this->m_undeadAbilityStrings->setSource(this->source());
	this->m_undeadAbilityStrings->load();

	this->m_nightElfAbilityFunc.reset(new MetaData(KUrl("Units/NightElfAbilityFunc.txt")));
	this->m_nightElfAbilityFunc->setSource(this->source());
	this->m_nightElfAbilityFunc->load();
	this->m_nightElfAbilityStrings.reset(new MetaData(KUrl("Units/NightElfAbilityStrings.txt")));
	this->m_nightElfAbilityStrings->setSource(this->source());
	this->m_nightElfAbilityStrings->load();

	this->m_commonAbilityFunc.reset(new MetaData(KUrl("Units/CommonAbilityFunc.txt")));
	this->m_commonAbilityFunc->setSource(this->source());
	this->m_commonAbilityFunc->load();
	this->m_commonAbilityStrings.reset(new MetaData(KUrl("Units/CommonAbilityStrings.txt")));
	this->m_commonAbilityStrings->setSource(this->source());
	this->m_commonAbilityStrings->load();

	this->m_neutralAbilityFunc.reset(new MetaData(KUrl("Units/NeutralAbilityFunc.txt")));
	this->m_neutralAbilityFunc->setSource(this->source());
	this->m_neutralAbilityFunc->load();
	this->m_neutralAbilityStrings.reset(new MetaData(KUrl("Units/NeutralAbilityStrings.txt")));
	this->m_neutralAbilityStrings->setSource(this->source());
	this->m_neutralAbilityStrings->load();

	this->m_campaignAbilityFunc.reset(new MetaData(KUrl("Units/CampaignAbilityFunc.txt")));
	this->m_campaignAbilityFunc->setSource(this->source());
	this->m_campaignAbilityFunc->load();
	this->m_campaignAbilityStrings.reset(new MetaData(KUrl("Units/CampaignAbilityStrings.txt")));
	this->m_campaignAbilityStrings->setSource(this->source());
	this->m_campaignAbilityStrings->load();
}

bool AbilityData::objectIsHero(const QString& originalObjectId, const QString& customObjectId) const
{
	return fieldValue(originalObjectId, customObjectId, "aher") == "1";
}

bool AbilityData::objectIsUnit(const QString& originalObjectId, const QString& customObjectId) const
{
	return !objectIsHero(originalObjectId, customObjectId) && !objectIsItem(originalObjectId, customObjectId);
}

bool AbilityData::objectIsItem(const QString& originalObjectId, const QString& customObjectId) const
{
	return fieldValue(originalObjectId, customObjectId, "aite") == "1";
}


}

}
