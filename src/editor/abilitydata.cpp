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

#include "abilitydata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

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
		for (int row = 1; row < this->abilityData()->rows(); ++row)
		{
			const QString alias = this->abilityData()->value(row, "alias");

			// skip empty rows
			if (!alias.isEmpty())
			{
				result << alias;
			}
			else
			{
				qDebug() << "Empty ability row " << row;
			}
		}
	}

	return result;
}

ObjectData::MetaDataList AbilityData::resolveDefaultField(const QString &objectId, const QString &fieldId) const
{
	MetaDataList result;

	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "AbilityData")
		{
			result.push_back(this->abilityData());
		}
		else if (slk == "Profile")
		{
			const QString race = this->abilityData()->value(objectId, "race");
			const QString sort = this->abilityData()->value(objectId, "sort");

			if (sort == "item")
			{
				result.push_back(this->itemAbilityStrings());
				result.push_back(this->itemAbilityFunc());
			}
			else
			{
				// TODO in Frozen Throne we can check if unit is in campaign but in Reign of Chaos there is no such field.
				result.push_back(this->campaignAbilityStrings());
				result.push_back(this->campaignAbilityFunc());

				/*
				 * It seems that some race abilities are specified in the neutral/common files.
				 */
				result.push_back(this->neutralAbilityStrings());
				result.push_back(this->neutralAbilityFunc());
				result.push_back(this->commonAbilityStrings());
				result.push_back(this->commonAbilityFunc());

				if (race == "human")
				{
					result.push_back(this->humanAbilityStrings());
					result.push_back(this->humanAbilityFunc());
				}
				else if (race == "orc")
				{
					result.push_back(this->orcAbilityStrings());
					result.push_back(this->orcAbilityFunc());
				}
				else if (race == "nightelf")
				{
					result.push_back(this->nightElfAbilityStrings());
					result.push_back(this->nightElfAbilityFunc());
				}
				else if (race == "undead")
				{
					result.push_back(this->undeadAbilityStrings());
					result.push_back(this->undeadAbilityFunc());
				}
			}

			if (result.empty())
			{
				qDebug() << "Missing value" << fieldId << "for object" << objectId << "with slk" << slk << "and race" << race << "and sort" << sort;
			}
		}
	}

	if (result.empty())
	{
		qDebug() << "Missing value" << fieldId << "for object" << objectId;
	}

	return result;
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

	return !((useHero && isHero) || (useUnit && isUnit) || (useItem && isItem));
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

QIcon AbilityData::objectIcon(const QString &originalObjectId, const QString &customObjectId, QWidget *window) const
{
	const QString art = this->fieldValue(originalObjectId, customObjectId, "aart");

	if (!art.isEmpty())
	{
		return this->source()->sharedData()->icon(art, window);
	}

	return this->source()->sharedData()->worldEditDataIcon("InvalidIcon", "WorldEditArt", window);
}

void AbilityData::widgetize(const KUrl &url)
{
	const QString dir = url.toLocalFile();

	Objects abilitiesWithMaximumLevel4;
	Objects abilitiesWithHigherLevel4;

	for (Objects::const_iterator iterator = this->objects().begin(); iterator != this->objects().end(); ++iterator)
	{
		if (this->fieldValue(iterator.key().originalObjectId(), iterator.key().customObjectId(), "alev") <= "4")
		{
			abilitiesWithMaximumLevel4.insert(iterator.key(), iterator.value());
		}
		else
		{
			abilitiesWithHigherLevel4.insert(iterator.key(), iterator.value());
		}
	}

	// create AbilityData.slk by merging the original with the custom objects
	map::Slk abilityData = dynamic_cast<SlkTextSource*>(this->abilityData()->textSource())->slk();
	abilityData.resizeTable(std::make_pair(abilityData.rows() + abilitiesWithMaximumLevel4.size(), abilityData.columns()));
	int row = 0;

	for (Objects::const_iterator iterator = abilitiesWithMaximumLevel4.begin(); iterator != abilitiesWithMaximumLevel4.end(); ++iterator)
	{
		ObjectId id = iterator.key();
		Modifications modifications = iterator.value();

		QList<QString> line;

		// alias
		line << id.second;
		// code
		line << id.second;
		// uberAlias
		line << id.second;
		// comments
		line << "";
		// useInEditor
		line << "1";

		// TODO all fields

		for (int column = 0; column < line.size() && column < abilityData.columns(); ++column)
		{
			abilityData.cell(row, column) = string(line[column].toUtf8().constData());
		}
	}

	// TODO create races TXT files!

	// TODO write abilitiesWithHigherLevel4 into w3a file

	ofstream out(string(dir.toUtf8().constData()) + "/" + "AbilityData.slk");
	abilityData.write(out);
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

MetaData* AbilityData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

bool AbilityData::objectIsHero(const QString &originalObjectId, const QString &customObjectId) const
{
	return fieldValue(originalObjectId, customObjectId, "aher") == "1";
}

bool AbilityData::objectIsUnit(const QString &originalObjectId, const QString &customObjectId) const
{
	return !objectIsHero(originalObjectId, customObjectId) && !objectIsItem(originalObjectId, customObjectId);
}

bool AbilityData::objectIsItem(const QString &originalObjectId, const QString &customObjectId) const
{
	return fieldValue(originalObjectId, customObjectId, "aite") == "1";
}


}

}
