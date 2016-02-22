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

#include "buffdata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

BuffData::BuffData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds BuffData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "UnitData.slk" to standard units in Unit Editor
	if (this->buffData() != 0 && !this->buffData()->isEmpty())
	{
		for (int row = 1; row < this->buffData()->rows(); ++row)
		{
			const QString alias = this->buffData()->value(row, "alias");

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

ObjectData::MetaDataList BuffData::resolveDefaultField(const QString &objectId, const QString &fieldId, int level) const
{
	MetaDataList result;

	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "AbilityBuffData")
		{
			result.push_back(this->buffData());
		}
		else if (slk == "Profile")
		{
			const QString race = this->buffData()->value(objectId, "race");
			const QString sort = this->buffData()->value(objectId, "sort");

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

map::CustomObjects::Type BuffData::type() const
{
	return map::CustomObjects::Type::Buffs;
}

bool BuffData::hasCustomUnits() const
{
	return false;
}

bool BuffData::hasCustomObjects() const
{
	return true;
}

bool BuffData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList BuffData::metaDataList() const
{
	ObjectData::MetaDataList result;
	result.append(buffData());
	result.append(itemAbilityFunc());
	result.append(humanAbilityFunc());

	// TODO support slks
	return result;
}

bool BuffData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	return false;
}

QString BuffData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	// buffs do not require editor names, in this case the tooltip is shown as name
	QString name = hasFieldValue(originalObjectId, customObjectId, "fnam") ? fieldReadableValue(originalObjectId, customObjectId, "fnam") : fieldReadableValue(originalObjectId, customObjectId, "ftip");

	if (this->hasFieldValue(originalObjectId, customObjectId, "fnsf"))
	{
		const QString suffix = this->fieldReadableValue(originalObjectId, customObjectId, "fnsf");

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

QIcon BuffData::objectIcon(const QString &originalObjectId, const QString &customObjectId, QWidget *window) const
{
	const QString art = this->fieldValue(originalObjectId, customObjectId, "fart");

	if (!art.isEmpty())
	{
		return this->source()->sharedData()->icon(art, window);
	}

	return this->source()->sharedData()->worldEditDataIcon("InvalidIcon", "WorldEditArt", window);
}

int BuffData::objectLevels(const QString &originalObjectId, const QString &customObjectId) const
{
	return 1;
}

void BuffData::load(QWidget *widget)
{
	this->m_metaData.reset(new MetaData(KUrl("Units/AbilityBuffMetaData.slk")));
	this->m_metaData->setSource(this->source());
	this->m_metaData->load();
	this->m_buffData.reset(new MetaData(KUrl("Units/AbilityBuffData.slk")));
	this->m_buffData->setSource(this->source());
	this->m_buffData->load();

	// TODO same files as for abilities, share it!
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

MetaData* BuffData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

}

}
