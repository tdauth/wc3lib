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

#include "unitmetadata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"


namespace wc3lib
{

namespace editor
{

UnitMetaData::UnitMetaData(MpqPriorityList *source)
: m_source(source)
, m_unitMetaData(0)
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
{

}

QString UnitMetaData::getDataValue(const QString &objectId, const QString &fieldId) const
{
	try
	{
		const QString slkFieldId = MetaData::toSlkString(fieldId);
		const QString slkObjectId = MetaData::toSlkString(objectId);
		const QString txtObjectId = objectId;
		const QString slkField = this->unitMetaData()->value(slkFieldId, "\"field\"");
		const QString txtField = MetaData::fromSlkString(slkField);
		const QString slk = MetaData::fromSlkString(this->unitMetaData()->value(slkFieldId, "\"slk\""));

		qDebug() << "SLK Object ID" << slkObjectId;
		qDebug() << "TXT Object ID" << txtObjectId;
		qDebug() << "SLK Field" << slkField;
		qDebug() << "TXT Field" << txtField;
		qDebug() << "SLK" << slk;

		// TODO improve performance by not calling the hasValue() methods?
		if (slk == "UnitUI")
		{
			if (this->unitUi()->hasValue(slkObjectId, slkField))
			{
				return this->unitUi()->value(slkObjectId, slkField);
			}
		}
		else if (slk == "UnitData")
		{
			if (this->unitData()->hasValue(slkObjectId, slkField))
			{
				return this->unitData()->value(slkObjectId, slkField);
			}
		}
		else if (slk == "UnitBalance")
		{
			if (this->unitBalance()->hasValue(slkObjectId, slkField))
			{
				return this->unitBalance()->value(slkObjectId, slkField);
			}
		}
		else if (slk == "UnitWeapons")
		{
			if (this->unitWeapons()->hasValue(slkObjectId, slkField))
			{
				return this->unitWeapons()->value(slkObjectId, slkField);
			}
		}
		else if (slk == "UnitAbilities")
		{
			if (this->unitAbilities()->hasValue(slkObjectId, slkField))
			{
				return this->unitAbilities()->value(slkObjectId, slkField);
			}
		}
		/*
		 * Profile means to use a TXT file from the corresponding race.
		 */
		else if (slk == "Profile")
		{
			if (this->unitData()->hasValue(slkObjectId, "\"race\""))
			{
				QString race = this->unitData()->value(slkObjectId, "\"race\"");
				race = race.mid(1, race.size() - 2);

				if (race == "human")
				{
					if (this->humanUnitStrings()->hasValue(txtObjectId, txtField))
					{
						return this->humanUnitStrings()->value(txtObjectId, txtField);
					}

					if (this->humanUnitFunc()->hasValue(txtObjectId, txtField))
					{
						return this->humanUnitFunc()->value(txtObjectId, txtField);
					}
				}
				else if (race == "orc")
				{
					if (this->orcUnitStrings()->hasValue(txtObjectId, txtField))
					{
						return this->orcUnitStrings()->value(txtObjectId, txtField);
					}

					if (this->orcUnitFunc()->hasValue(txtObjectId, txtField))
					{
						return this->orcUnitFunc()->value(txtObjectId, txtField);
					}
				}
				else if (race == "nightelf")
				{
					if (this->nightElfUnitStrings()->hasValue(txtObjectId, txtField))
					{
						return this->nightElfUnitStrings()->value(txtObjectId, txtField);
					}

					if (this->nightElfUnitFunc()->hasValue(txtObjectId, txtField))
					{
						return this->nightElfUnitFunc()->value(txtObjectId, txtField);
					}
				}
				else if (race == "undead")
				{
					if (this->undeadUnitStrings()->hasValue(txtObjectId, txtField))
					{
						return this->undeadUnitStrings()->value(txtObjectId, txtField);
					}

					if (this->undeadUnitFunc()->hasValue(txtObjectId, txtField))
					{
						return this->undeadUnitFunc()->value(txtObjectId, txtField);
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

void UnitMetaData::load(QWidget *widget)
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
}

}

}
