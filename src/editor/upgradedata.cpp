/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#include "upgradedata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

UpgradeData::UpgradeData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds UpgradeData::standardObjectIds() const
{
	StandardObjecIds result;

	// add all entries from "Weather.slk" to standard weather effects in Unit Editor
	if (this->upgradeData() != 0 && !this->upgradeData()->isEmpty())
	{
		for (int row = 1; row < this->upgradeData()->rows(); ++row)
		{
			result << this->upgradeData()->value(row, "upgradeid");
		}
	}

	return result;
}

ObjectData::MetaDataList UpgradeData::resolveDefaultField(const QString &objectId, const QString &fieldId, int level) const
{
	MetaDataList result;

	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "UpgradeData")
		{
			result.push_back(this->upgradeData());
		}
		else if (slk == "Profile")
		{
			// TODO in Frozen Throne we can check if unit is in campaign but in Reign of Chaos there is no such field.
			result.push_back(this->campaignUpgradeStrings());
			result.push_back(this->campaignUpgradeFunc());

			if (this->upgradeData()->hasValue(objectId, "race"))
			{
				const QString race = this->upgradeData()->value(objectId, "race");

				if (race == "human")
				{
					result.push_back(this->humanUpgradeStrings());
					result.push_back(this->humanUpgradeFunc());
				}
				else if (race == "orc")
				{
					result.push_back(this->orcUpgradeStrings());
					result.push_back(this->orcUpgradeFunc());
				}
				else if (race == "nightelf")
				{
					result.push_back(this->nightElfUpgradeStrings());
					result.push_back(this->nightElfUpgradeFunc());
				}
				else if (race == "undead")
				{
					result.push_back(this->undeadUpgradeStrings());
					result.push_back(this->undeadUpgradeFunc());
				}
				else
				{
					result.push_back(this->neutralUpgradeStrings());
					result.push_back(this->neutralUpgradeFunc());
				}
			}
		}
	}

	return result;
}

bool UpgradeData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	return false;
}

bool UpgradeData::hasCustomUnits() const
{
	return false;
}

bool UpgradeData::hasCustomObjects() const
{
	return false;
}

bool UpgradeData::hasMetaDataList() const
{
	return true;
}

ObjectData::MetaDataList UpgradeData::metaDataList() const
{
	/*
	map::Slk slk;
	slk.table().resize(boost::extents[this->metaData()->rows()][this->m_objects.size() + this->weather()->rows()]);
	map::Slk::Table::size_type column = 0;

	for (map::Slk::Table::size_type row = 1; row < this->metaData()->rows() && column < slk.columns(); ++row, ++column)
	{
		const QString fieldId = this->metaData()->value(row, "ID");
		slk.table()[column][0] = this->metaData()->value(fieldId, "field").toUtf8().constData();
	}

	Slks result;
	result.append(slk);

	return result;
	TODO create files
	*/

	return MetaDataList();
}

void UpgradeData::load(QWidget *widget)
{
	this->m_upgradeMetaData.reset(new MetaData(KUrl("Units/UpgradeMetaData.slk")));
	this->m_upgradeMetaData->setSource(this->source());
	this->m_upgradeMetaData->load();
	this->m_upgradeData.reset(new MetaData(KUrl("Units/UpgradeData.slk")));
	this->m_upgradeData->setSource(this->source());
	this->m_upgradeData->load();


	this->m_humanUpgradeStrings.reset(new MetaData(KUrl("Units/HumanUpgradeStrings.txt")));
	this->m_humanUpgradeStrings->setSource(this->source());
	this->m_humanUpgradeStrings->load();

	this->m_humanUpgradeFunc.reset(new MetaData(KUrl("Units/HumanUpgradeFunc.txt")));
	this->m_humanUpgradeFunc->setSource(this->source());
	this->m_humanUpgradeFunc->load();

	this->m_orcUpgradeStrings.reset(new MetaData(KUrl("Units/OrcUpgradeStrings.txt")));
	this->m_orcUpgradeStrings->setSource(this->source());
	this->m_orcUpgradeStrings->load();

	this->m_orcUpgradeFunc.reset(new MetaData(KUrl("Units/OrcUpgradeFunc.txt")));
	this->m_orcUpgradeFunc->setSource(this->source());
	this->m_orcUpgradeFunc->load();

	this->m_undeadUpgradeStrings.reset(new MetaData(KUrl("Units/UndeadUpgradeStrings.txt")));
	this->m_undeadUpgradeStrings->setSource(this->source());
	this->m_undeadUpgradeStrings->load();


	this->m_undeadUpgradeFunc.reset(new MetaData(KUrl("Units/UndeadUpgradeFunc.txt")));
	this->m_undeadUpgradeFunc->setSource(this->source());
	this->m_undeadUpgradeFunc->load();

	this->m_nightElfUpgradeStrings.reset(new MetaData(KUrl("Units/NightElfUpgradeStrings.txt")));
	this->m_nightElfUpgradeStrings->setSource(this->source());
	this->m_nightElfUpgradeStrings->load();

	this->m_nightElfUpgradeFunc.reset(new MetaData(KUrl("Units/NightElfUpgradeFunc.txt")));
	this->m_nightElfUpgradeFunc->setSource(this->source());
	this->m_nightElfUpgradeFunc->load();

	this->m_neutralUpgradeStrings.reset(new MetaData(KUrl("Units/NeutralUpgradeStrings.txt")));
	this->m_neutralUpgradeStrings->setSource(this->source());
	this->m_neutralUpgradeStrings->load();

	this->m_neutralUpgradeFunc.reset(new MetaData(KUrl("Units/NeutralUpgradeFunc.txt")));
	this->m_neutralUpgradeFunc->setSource(this->source());
	this->m_neutralUpgradeFunc->load();

	this->m_campaignUpgradeStrings.reset(new MetaData(KUrl("Units/CampaignUpgradeStrings.txt")));
	this->m_campaignUpgradeStrings->setSource(this->source());
	this->m_campaignUpgradeStrings->load();

	this->m_campaignUpgradeFunc.reset(new MetaData(KUrl("Units/CampaignUpgradeFunc.txt")));
	this->m_campaignUpgradeFunc->setSource(this->source());
	this->m_campaignUpgradeFunc->load();
}

MetaData* UpgradeData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

QString UpgradeData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	const QString result = fieldReadableValue(originalObjectId, customObjectId, "gnam");

	return this->source()->sharedData()->tr(result, "WorldEditStrings", result);
}

QIcon UpgradeData::objectIcon(const QString &originalObjectId, const QString &customObjectId, QWidget *window) const
{
	QString art = this->fieldValue(originalObjectId, customObjectId, "gar1");

	if (!art.endsWith(".blp", Qt::CaseInsensitive))
	{
		art += ".blp";
	}

	return this->source()->sharedData()->icon(art, window);
}

int UpgradeData::objectLevels(const QString &originalObjectId, const QString &customObjectId) const
{
	bool ok = false;
	const int result = this->fieldValue(originalObjectId, customObjectId, "glvl").toInt(&ok);

	if (ok)
	{
		return result;
	}

	return 1;
}

QString UpgradeData::nextCustomObjectId() const
{
	QString result = ObjectData::nextCustomObjectId();
	result[0] = 'G';

	return result;
}

}

}
