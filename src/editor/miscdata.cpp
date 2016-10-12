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

	for (int i = 0; i < m_miscFiles.keys().size(); ++i)
	{
		result << m_miscFiles.keys()[i];
	}

	return result;
}

ObjectData::MetaDataList MiscData::resolveDefaultField(const QString &objectId, const QString &fieldId, int level) const
{
	MetaDataList result;

	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->miscMetaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->miscMetaData()->value(fieldId, "slk");

		if (m_miscFiles.contains(slk))
		{
			result.push_back(m_miscFiles.value(slk));
		}
	}

	return result;
}

bool MiscData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
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

QString MiscData::customObjectsExtension() const
{
	return QString();
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

QString MiscData::nextCustomObjectId(const QString &originalObjectId) const
{
	QString result = ObjectData::nextCustomObjectId(originalObjectId);
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

int MiscData::objectLevels(const QString& originalObjectId, const QString& customObjectId) const
{
	return 1;
}

void MiscData::load(QWidget *widget)
{
	this->m_miscMetaData.reset(new MetaData(QUrl("Units/MiscMetaData.slk")));
	this->m_miscMetaData->setSource(this->source());
	this->m_miscMetaData->load();
	this->m_miscGame.reset(new MetaData(QUrl("Units/miscgame.txt")));
	this->m_miscGame->setSource(this->source());
	this->m_miscGame->load();
	this->m_customV0.reset(new MetaData(QUrl("Custom_V0/Units/miscgame.txt")));
	this->m_customV0->setSource(this->source());
	this->m_customV0->load();
	this->m_customV1.reset(new MetaData(QUrl("Custom_V1/Units/miscgame.txt")));
	this->m_customV1->setSource(this->source());
	this->m_customV1->load();
	this->m_meleeV0.reset(new MetaData(QUrl("melee_v0/Units/miscgame.txt")));
	this->m_meleeV0->setSource(this->source());
	this->m_meleeV0->load();

	m_miscFiles.clear();
	m_miscFiles.insert("mgam", m_miscGame.data());
	m_miscFiles.insert("mcv0", m_customV0.data());
	m_miscFiles.insert("mcv1", m_customV1.data());
	m_miscFiles.insert("mmv0", m_meleeV0.data());

	// TODO get item functions and unit data
}

MetaData* MiscData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

}

}
