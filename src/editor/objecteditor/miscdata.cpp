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

#include "miscdata.hpp"
#include "../metadata.hpp"
#include "../mpqprioritylist.hpp"


namespace wc3lib
{

namespace editor
{

MiscData::MiscData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds MiscData::standardObjectIds() const
{
	StandardObjecIds result;
	result << "mgam";
	result << "mcv0";
	result << "mcv1";
	result << "mmv0";

	return result;
}

bool MiscData::hasDefaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->miscMetaData()->hasValue(fieldId, "field"))
	{
		const QString field = this->miscMetaData()->value(fieldId, "field");
		const QString slk = this->miscMetaData()->value(fieldId, "slk");

		if (objectId == "mgam")
		{
			return this->miscGame()->hasValue("Misc", field);
		}
		else if (objectId == "mcv0")
		{
			return this->customV0()->hasValue("Misc", field);
		}
		else if (objectId == "mcv1")
		{
			return this->customV1()->hasValue("Misc", field);
		}
		else if (objectId == "mmv0")
		{
			return this->meleeV0()->hasValue("Misc", field);
		}
	}

	return false;
}

QString MiscData::defaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	try
	{
		/*
		 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
		 */
		if (this->miscMetaData()->hasValue(fieldId, "field"))
		{
			const QString field = this->miscMetaData()->value(fieldId, "field");
			const QString slk = this->miscMetaData()->value(fieldId, "slk");

			if (objectId == "mgam")
			{
				return this->miscGame()->value("Misc", field);
			}
			else if (objectId == "mcv0")
			{
				return this->customV0()->value("Misc", field);
			}
			else if (objectId == "mcv1")
			{
				return this->customV1()->value("Misc", field);
			}
			else if (objectId == "mmv0")
			{
				return this->meleeV0()->value("Misc", field);
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

bool MiscData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const
{
	return false;
}

bool MiscData::hasCustomUnits() const
{
	return false;
}

bool MiscData::hasCustomObjects() const
{
	return false;
}

bool MiscData::hasMetaDataList() const
{
	// TODO support
	return true;
}

ObjectData::MetaDataList MiscData::metaDataList() const
{
	return ObjectData::MetaDataList();
}

QString MiscData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	if (customObjectId.isEmpty())
	{
		return QObject::tr("Misc Data: %1").arg(originalObjectId);
	}

	return QObject::tr("Misc Data: %1:%2").arg(originalObjectId).arg(customObjectId);
}

void MiscData::load(QWidget *widget)
{
	this->m_miscMetaData.reset(new MetaData(KUrl("Units/MiscMetaData.slk")));
	this->m_miscMetaData->setSource(this->source());

	try
	{
		this->m_miscMetaData->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_miscMetaData->url().toEncoded().constData(), e.what()));
	}

	this->m_miscGame.reset(new MetaData(KUrl("Units/miscgame.txt")));
	this->m_miscGame->setSource(this->source());

	try
	{
		this->m_miscGame->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_miscGame->url().toEncoded().constData(), e.what()));
	}

	this->m_customV0.reset(new MetaData(KUrl("Custom_V0/Units/miscgame.txt")));
	this->m_customV0->setSource(this->source());

	try
	{
		this->m_customV0->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_customV0->url().toEncoded().constData(), e.what()));
	}

	this->m_customV1.reset(new MetaData(KUrl("Custom_V1/Units/miscgame.txt")));
	this->m_customV1->setSource(this->source());

	try
	{
		this->m_customV1->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_customV1->url().toEncoded().constData(), e.what()));
	}

	this->m_meleeV0.reset(new MetaData(KUrl("melee_v0/Units/miscgame.txt")));
	this->m_meleeV0->setSource(this->source());

	try
	{
		this->m_meleeV0->load();
	}
	catch (Exception &e)
	{
		KMessageBox::error(widget, i18n("Error on loading file \"%1\": %2", this->m_meleeV0->url().toEncoded().constData(), e.what()));
	}
}

}

}
