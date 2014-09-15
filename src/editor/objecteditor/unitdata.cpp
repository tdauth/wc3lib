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

#include <KMessageBox>

#include "unitdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"


namespace wc3lib
{

namespace editor
{

UnitData::UnitData(MpqPriorityList *source)
: ObjectData(source)
, m_unitMetaData(0)
, m_unitEditorData(0)
, m_unitData(0)
, m_unitUi(0)
, m_unitBalance(0)
, m_unitWeapons(0)
, m_unitAbilities(0)
, m_humanUnitStrings(0)
, m_humanUnitFunc(0)
, m_orcUnitStrings(0)
, m_orcUnitFunc(0)
, m_undeadUnitStrings(0)
, m_undeadUnitFunc(0)
, m_nightElfUnitStrings(0)
, m_nightElfUnitFunc(0)
, m_neutralUnitStrings(0)
, m_neutralUnitFunc(0)
, m_campaignUnitStrings(0)
, m_campaignUnitFunc(0)
{

}

bool UnitData::hasDefaultFieldValue(const QString& objectId, const QString& fieldId) const
{
	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->unitMetaData()->hasValue(fieldId, "field"))
	{
		const QString field = this->unitMetaData()->value(fieldId, "field");
		const QString slk = this->unitMetaData()->value(fieldId, "slk");

		// TODO improve performance by not calling the hasValue() methods?
		if (slk == "UnitUI")
		{
			return this->unitUi()->hasValue(objectId, field);
		}
		else if (slk == "UnitData")
		{
			return this->unitData()->hasValue(objectId, field);
		}
		else if (slk == "UnitBalance")
		{
			return this->unitBalance()->hasValue(objectId, field);
		}
		else if (slk == "UnitWeapons")
		{
			return this->unitWeapons()->hasValue(objectId, field);
		}
		else if (slk == "UnitAbilities")
		{
			return this->unitAbilities()->hasValue(objectId, field);
		}
		/*
		* Profile means to use a TXT file from the corresponding race.
		*/
		else if (slk == "Profile")
		{
			// TODO in Frozen Throne we can check if unit is in campaign but in Reign of Chaos there is no such field.
			if (this->campaignUnitStrings()->hasValue(objectId, field)
			|| this->campaignUnitFunc()->hasValue(objectId, field))
			{
				return true;
			}
			else if (this->unitData()->hasValue(objectId, "race"))
			{
				const QString race = this->unitData()->value(objectId, "race");

				if (race == "human")
				{
					return this->humanUnitStrings()->hasValue(objectId, field)
						|| this->humanUnitFunc()->hasValue(objectId, field);
				}
				else if (race == "orc")
				{
					return this->orcUnitStrings()->hasValue(objectId, field)
						|| this->orcUnitFunc()->hasValue(objectId, field);
				}
				else if (race == "nightelf")
				{
					return this->nightElfUnitStrings()->hasValue(objectId, field)
						|| this->nightElfUnitFunc()->hasValue(objectId, field);
				}
				else if (race == "undead")
				{
					return this->undeadUnitStrings()->hasValue(objectId, field)
						|| this->undeadUnitFunc()->hasValue(objectId, field);
				}
				else
				{
					return this->neutralUnitStrings()->hasValue(objectId, field)
						|| this->neutralUnitFunc()->hasValue(objectId, field);
				}
			}
		}
	}

	return false;
}

QString UnitData::defaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	try
	{
		const QString field = this->unitMetaData()->value(fieldId, "field");
		const QString slk = this->unitMetaData()->value(fieldId, "slk");

		qDebug() << "SLK:" << slk;

		// TODO improve performance by not calling the hasValue() methods?
		if (slk == "UnitUI")
		{
			if (this->unitUi()->hasValue(objectId, field))
			{
				return this->unitUi()->value(objectId, field);
			}
		}
		else if (slk == "UnitData")
		{
			if (this->unitData()->hasValue(objectId, field))
			{
				return this->unitData()->value(objectId, field);
			}
		}
		else if (slk == "UnitBalance")
		{
			if (this->unitBalance()->hasValue(objectId, field))
			{
				return this->unitBalance()->value(objectId, field);
			}
		}
		else if (slk == "UnitWeapons")
		{
			if (this->unitWeapons()->hasValue(objectId, field))
			{
				return this->unitWeapons()->value(objectId, field);
			}
		}
		else if (slk == "UnitAbilities")
		{
			if (this->unitAbilities()->hasValue(objectId, field))
			{
				return this->unitAbilities()->value(objectId, field);
			}
		}
		/*
		 * Profile means to use a TXT file from the corresponding race.
		 */
		else if (slk == "Profile")
		{
			// TODO in Frozen Throne we can check if unit is in campaign but in Reign of Chaos there is no such field.
			if (this->campaignUnitStrings()->hasValue(objectId, field))
			{
				return this->campaignUnitStrings()->value(objectId, field);
			}
			else if (this->campaignUnitFunc()->hasValue(objectId, field))
			{
				return this->campaignUnitFunc()->value(objectId, field);
			}
			else if (this->unitData()->hasValue(objectId, "race"))
			{
				const QString race = this->unitData()->value(objectId, "race");

				if (race == "human")
				{
					if (this->humanUnitStrings()->hasValue(objectId, field))
					{
						return this->humanUnitStrings()->value(objectId, field);
					}

					if (this->humanUnitFunc()->hasValue(objectId, field))
					{
						return this->humanUnitFunc()->value(objectId, field);
					}
				}
				else if (race == "orc")
				{
					if (this->orcUnitStrings()->hasValue(objectId, field))
					{
						return this->orcUnitStrings()->value(objectId, field);
					}

					if (this->orcUnitFunc()->hasValue(objectId, field))
					{
						return this->orcUnitFunc()->value(objectId, field);
					}
				}
				else if (race == "nightelf")
				{
					if (this->nightElfUnitStrings()->hasValue(objectId, field))
					{
						return this->nightElfUnitStrings()->value(objectId, field);
					}

					if (this->nightElfUnitFunc()->hasValue(objectId, field))
					{
						return this->nightElfUnitFunc()->value(objectId, field);
					}
				}
				else if (race == "undead")
				{
					if (this->undeadUnitStrings()->hasValue(objectId, field))
					{
						return this->undeadUnitStrings()->value(objectId, field);
					}

					if (this->undeadUnitFunc()->hasValue(objectId, field))
					{
						return this->undeadUnitFunc()->value(objectId, field);
					}
				}
				else
				{
					if (this->neutralUnitStrings()->hasValue(objectId, field))
					{
						return this->neutralUnitStrings()->value(objectId, field);
					}

					if (this->neutralUnitFunc()->hasValue(objectId, field))
					{
						return this->neutralUnitFunc()->value(objectId, field);
					}
				}
			}
		}
	}
	catch (Exception &e)
	{
		qDebug() << "Exception occured";
		qDebug() << e.what();
		//QMessageBox::warning(this, tr("Error"), e.what());
	}

	qDebug() << "Data value not found:" << objectId << fieldId;

	return "";
}

bool UnitData::hasCustomUnits() const
{
	return true;
}

bool UnitData::hasCustomObjects() const
{
	return true;
}

void UnitData::load(QWidget *widget)
{
	this->m_unitMetaData = new MetaData(KUrl("Units/UnitMetaData.slk"));
	this->m_unitMetaData->setSource(this->source());

	try
	{
		this->m_unitMetaData->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitMetaData->url().toEncoded().constData(), e.what()));
	}

	this->m_unitEditorData = new MetaData(KUrl("UI/UnitEditorData.txt"));
	this->m_unitEditorData->setSource(this->source());

	try
	{
		this->m_unitEditorData->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitEditorData->url().toEncoded().constData(), e.what()));
	}

	this->m_unitData = new MetaData(KUrl("Units/UnitData.slk"));
	this->m_unitData->setSource(this->source());

	try
	{
		this->m_unitData->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitData->url().toEncoded().constData(), e.what()));
	}

	this->m_unitUi = new MetaData(KUrl("Units/unitUI.slk"));
	this->m_unitUi->setSource(this->source());

	try
	{
		this->m_unitUi->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitUi->url().toEncoded().constData(), e.what()));
	}

	this->m_unitBalance = new MetaData(KUrl("Units/UnitBalance.slk"));
	this->m_unitBalance->setSource(this->source());

	try
	{
		this->m_unitBalance->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitBalance->url().toEncoded().constData(), e.what()));
	}

	this->m_unitWeapons = new MetaData(KUrl("Units/UnitWeapons.slk"));
	this->m_unitWeapons->setSource(this->source());

	try
	{
		this->m_unitWeapons->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitWeapons->url().toEncoded().constData(), e.what()));
	}

	this->m_unitAbilities = new MetaData(KUrl("Units/UnitAbilities.slk"));
	this->m_unitAbilities->setSource(this->source());

	try
	{
		this->m_unitAbilities->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_unitAbilities->url().toEncoded().constData(), e.what()));
	}

	this->m_humanUnitStrings = new MetaData(KUrl("Units/HumanUnitStrings.txt"));
	this->m_humanUnitStrings->setSource(this->source());

	try
	{
		this->m_humanUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_humanUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_humanUnitFunc = new MetaData(KUrl("Units/HumanUnitFunc.txt"));
	this->m_humanUnitFunc->setSource(this->source());

	try
	{
		this->m_humanUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_humanUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_orcUnitStrings = new MetaData(KUrl("Units/OrcUnitStrings.txt"));
	this->m_orcUnitStrings->setSource(this->source());

	try
	{
		this->m_orcUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_orcUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_orcUnitFunc = new MetaData(KUrl("Units/OrcUnitFunc.txt"));
	this->m_orcUnitFunc->setSource(this->source());

	try
	{
		this->m_orcUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_orcUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_undeadUnitStrings = new MetaData(KUrl("Units/UndeadUnitStrings.txt"));
	this->m_undeadUnitStrings->setSource(this->source());

	try
	{
		this->m_undeadUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_undeadUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_undeadUnitFunc = new MetaData(KUrl("Units/UndeadUnitFunc.txt"));
	this->m_undeadUnitFunc->setSource(this->source());

	try
	{
		this->m_undeadUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_undeadUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_nightElfUnitStrings = new MetaData(KUrl("Units/NightElfUnitStrings.txt"));
	this->m_nightElfUnitStrings->setSource(this->source());

	try
	{
		this->m_nightElfUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_nightElfUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_nightElfUnitFunc = new MetaData(KUrl("Units/NightElfUnitFunc.txt"));
	this->m_nightElfUnitFunc->setSource(this->source());

	try
	{
		this->m_nightElfUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_nightElfUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_neutralUnitStrings = new MetaData(KUrl("Units/NeutralUnitStrings.txt"));
	this->m_neutralUnitStrings->setSource(this->source());

	try
	{
		this->m_neutralUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_neutralUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_neutralUnitFunc = new MetaData(KUrl("Units/NeutralUnitFunc.txt"));
	this->m_neutralUnitFunc->setSource(this->source());

	try
	{
		this->m_neutralUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_neutralUnitFunc->url().toEncoded().constData(), e.what()));
	}

	this->m_campaignUnitStrings = new MetaData(KUrl("Units/CampaignUnitStrings.txt"));
	this->m_campaignUnitStrings->setSource(this->source());

	try
	{
		this->m_campaignUnitStrings->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_campaignUnitStrings->url().toEncoded().constData(), e.what()));
	}

	this->m_campaignUnitFunc = new MetaData(KUrl("Units/CampaignUnitFunc.txt"));
	this->m_campaignUnitFunc->setSource(this->source());

	try
	{
		this->m_campaignUnitFunc->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_campaignUnitFunc->url().toEncoded().constData(), e.what()));
	}
}


bool UnitData::objectIsBuilding(const QString &originalObjectId, const QString &customObjectId) const
{
	bool ok = true;

	/*
	 * Objects without a level are buildings.
	 */
	if (this->hasDefaultFieldValue(originalObjectId, "ulev"))
	{
		const QString levelVar = this->defaultFieldValue(originalObjectId, "ulev");
		levelVar.toInt(&ok);
	}

	bool isBuilding = false;

	/*
	 * In Frozen Throne there is an additional field which indicates if the unit is a building.
	 */
	if (((ObjectData*)this)->hasDefaultFieldValue(originalObjectId, "udbg"))
	{
		if (((ObjectData*)this)->fieldValue(originalObjectId, customObjectId, "udbg") == "1")
		{
			isBuilding = true;
		}
	}

	return !ok || isBuilding; // buildings have no levels
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

}

}
