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

#include "skindata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"


namespace wc3lib
{

namespace editor
{

SkinData::SkinData(MpqPriorityList *source, QObject *parent) : ObjectData(source, parent)
{
}

ObjectData::StandardObjecIds SkinData::standardObjectIds() const
{
	StandardObjecIds result;

	// the main section lists all races
	const QStringList skins = war3Skins()->value("Main", "Skins").split(',');

	foreach (QString skin, skins)
	{
		result << skin;
	}

	// the default section contains default entries and is always present but not listed in the main section
	result << "Default";

	qDebug() << "Standard objects for skins: " << result << "and value" << war3Skins()->value("Main", "Skins");

	return result;
}

ObjectData::MetaDataList SkinData::resolveDefaultField(const QString &objectId, const QString &fieldId, int level) const
{
	MetaDataList result;

	/*
	 * If the field does not exist it might be the case that Reign of Chaos files are loaded and not Frozen Throne.
	 */
	if (this->metaData()->hasValue(fieldId, "slk"))
	{
		const QString slk = this->metaData()->value(fieldId, "slk");

		if (slk == "Profile")
		{
			result.push_back(war3Skins());
		}
	}

	return result;
}

bool SkinData::hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	// for skin data not all fields appear in every section
	return !this->hasDefaultFieldValue(originalObjectId, fieldId, level);
}

bool SkinData::hasCustomUnits() const
{
	return false;
}

bool SkinData::hasCustomObjects() const
{
	return false;
}

bool SkinData::hasMetaDataList() const
{
	// TODO support
	return true;
}

ObjectData::MetaDataList SkinData::metaDataList() const
{
	return ObjectData::MetaDataList();
}

QString SkinData::nextCustomObjectId(const QString &originalObjectId) const
{
	QString result = ObjectData::nextCustomObjectId(originalObjectId);
	result[0] = 'C';

	return result;
}

QString SkinData::objectName(const QString &originalObjectId, const QString &customObjectId) const
{
	if (customObjectId.isEmpty())
	{
		return QObject::tr("Skin Data: %1").arg(originalObjectId);
	}

	return QObject::tr("Skin Data: %1:%2").arg(originalObjectId).arg(customObjectId);
}

QIcon SkinData::objectIcon(const QString& originalObjectId, const QString& customObjectId, QWidget* window) const
{
	return QIcon();
}

int SkinData::objectLevels(const QString& originalObjectId, const QString& customObjectId) const
{
	return 1;
}

void SkinData::load(QWidget *widget)
{
	this->m_metaData.reset(new MetaData(KUrl("UI/SkinMetaData.slk")));
	this->m_metaData->setSource(this->source());
	this->m_metaData->load();
	this->m_war3skins.reset(new MetaData(KUrl("UI/war3skins.txt")));
	this->m_war3skins->setSource(this->source());
	this->m_war3skins->load();

	m_standardFiles.clear();
	m_standardFiles.insert("war3skins", m_war3skins.data());

	// TODO get item functions and unit data
}

MetaData* SkinData::objectTabData() const
{
	return this->source()->sharedData()->sharedObjectData()->unitEditorData().get();
}

}

}
