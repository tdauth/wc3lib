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

#include "miscdata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"


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

ObjectData::MetaDataList MiscData::resolveDefaultField(const QString &objectId, const QString &fieldId) const
{
	MetaDataList result;

	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->miscMetaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->miscMetaData()->value(fieldId, "slk");

		if (objectId == "mgam")
		{
			result.push_back(this->miscGame());
		}
		else if (objectId == "mcv0")
		{
			result.push_back(this->customV0());
		}
		else if (objectId == "mcv1")
		{
			result.push_back(this->customV1());
		}
		else if (objectId == "mmv0")
		{
			result.push_back(this->meleeV0());
		}
	}

	return result;
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

QString MiscData::nextCustomObjectId() const
{
	QString result = ObjectData::nextCustomObjectId();
	result[0] = 'M';

	return result;
}

QString MiscData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	if (customObjectId.isEmpty())
	{
		return QObject::tr("Misc Data: %1").arg(originalObjectId);
	}

	return QObject::tr("Misc Data: %1:%2").arg(originalObjectId).arg(customObjectId);
}

QIcon MiscData::objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const
{
	return QIcon();
}

void MiscData::load(QWidget *widget)
{
	this->m_miscMetaData.reset(new MetaData(KUrl("Units/MiscMetaData.slk")));
	this->m_miscMetaData->setSource(this->source());
	this->m_miscMetaData->load();
	this->m_miscGame.reset(new MetaData(KUrl("Units/miscgame.txt")));
	this->m_miscGame->setSource(this->source());
	this->m_miscGame->load();
	this->m_customV0.reset(new MetaData(KUrl("Custom_V0/Units/miscgame.txt")));
	this->m_customV0->setSource(this->source());
	this->m_customV0->load();
	this->m_customV1.reset(new MetaData(KUrl("Custom_V1/Units/miscgame.txt")));
	this->m_customV1->setSource(this->source());
	this->m_customV1->load();
	this->m_meleeV0.reset(new MetaData(KUrl("melee_v0/Units/miscgame.txt")));
	this->m_meleeV0->setSource(this->source());
	this->m_meleeV0->load();
	// TODO get item functions and unit data
}

MetaData* MiscData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

}

}
