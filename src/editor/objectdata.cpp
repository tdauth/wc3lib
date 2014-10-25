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

#include "objectdata.hpp"
#include "metadata.hpp"
#include "mpqprioritylist.hpp"
#include "sharedobjectdata.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

ObjectData::ObjectData(MpqPriorityList *source, QObject *parent) : QObject(parent), m_source(source)
{
}

map::Value::Type ObjectData::fieldType(const QString &fieldId) const
{
	const QString type = this->metaData()->value(fieldId, "type");

	if (type == "string")
	{
		return map::Value::Type::String;
	}
	else if (type == "int")
	{
		return map::Value::Type::Integer;
	}
	else if (type == "real")
	{
		return map::Value::Type::Real;
	}
	else if (type == "unreal")
	{
		return map::Value::Type::Unreal;
	}
	else if (type == "bool")
	{
		return map::Value::Type::Boolean;
	}
	else if (type == "stringList")
	{
		return map::Value::Type::StringList;
	}
	else if (type == "char")
	{
		return map::Value::Type::Character;
	}
	else if (type == "unitList")
	{
		return map::Value::Type::UnitList;
	}
	else if (type == "itemList")
	{
		return map::Value::Type::ItemList;
	}
	else if (type == "regenType")
	{
		return map::Value::Type::RegenerationType;
	}
	else if (type == "attackType")
	{
		return map::Value::Type::AttackType;
	}
	else if (type == "weaponType")
	{
		return map::Value::Type::WeaponType;
	}
	else if (type == "targetType")
	{
		return map::Value::Type::TargetType;
	}
	else if (type == "moveType")
	{
		return map::Value::Type::MoveType;
	}
	else if (type == "defenseType")
	{
		return map::Value::Type::DefenseType;
	}
	else if (type == "pathingTexture")
	{
		return map::Value::Type::PathingTexture;
	}
	else if (type == "upgradeList")
	{
		return map::Value::Type::UpgradeList;
	}
	else if (type == "abilityList")
	{
		return map::Value::Type::AbilityList;
	}
	else if (type == "heroAbilityList")
	{
		return map::Value::Type::HeroAbilityList;
	}
	else if (type == "missileArt")
	{
		return map::Value::Type::MissileArt;
	}
	else if (type == "attributeType")
	{
		return map::Value::Type::AttributeType;
	}
	else if (type == "attackBits")
	{
		return map::Value::Type::AttackBits;
	}

	// TODO how to detect Frozen Throne types, which one is int and which one boolean and which one string
	return map::Value::Type::String;
}

bool ObjectData::fieldTypeIsList(const QString &fieldType) const
{
	return fieldTypeIsLiteralList(fieldType)
		|| fieldTypeIsObjectList(fieldType)
		;
}

bool ObjectData::fieldTypeIsLiteralList(const QString &fieldType) const
{
	return fieldType == "intList"
		|| fieldType == "unrealList"
		|| fieldType == "stringList"
		;
}

QString ObjectData::fieldLiteralTypeFromListType(const QString &fieldLiteralListType) const
{
	QString result = fieldLiteralListType;
	const int index = fieldLiteralListType.lastIndexOf("List");

	if (index != -1)
	{
		result.truncate(index);

		return result;
	}

	return "";
}

bool ObjectData::loadOnRequest(QWidget* widget)
{
	if (this->metaData() == 0)
	{
		/*
		* Indicate loading by changing the cursor to busy.
		* The process of loading object data might take quite some time.
		*/
		QCursor cursor = widget->cursor();
		cursor.setShape(Qt::BusyCursor);
		widget->setCursor(cursor);

		try
		{
			this->load(widget);
		}
		catch (const Exception &e)
		{
			QMessageBox::warning(widget, QObject::tr("Error"), e.what());

			return false;
		}

		cursor = widget->cursor();
		cursor.setShape(Qt::ArrowCursor);
		widget->setCursor(cursor);
	}

	return true;
}

bool ObjectData::fieldTypeIsObjectList(const QString& fieldType) const
{
	return fieldType == "unitList"
		|| fieldType == "upgradeList"
		|| fieldType == "abilityList"
		|| fieldType == "heroAbilityList"
		|| fieldType == "techList"
		|| fieldType == "itemList";
}

bool ObjectData::fieldTypeAllowsMultipleSelections(const QString &fieldType) const
{
	return (fieldType == "unitClass"
		|| fieldType == "targetType"
		|| fieldType == "targetList"
		|| fieldType == "tilesetList"
		|| fieldType == "versionFlags"
	);
}


bool ObjectData::hasDefaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	MetaDataList metaDataList = this->resolveDefaultField(objectId, fieldId);

	if (!metaDataList.isEmpty())
	{
		const QString field = this->metaData()->value(fieldId, "field");
		/*
		 * The index is used for button positions for example when multiple fields share the same field name such as "Buttonpos".
		 */
		const QString index = this->metaData()->value(fieldId, "index");
		bool hasValue = false;
		QString value;

		foreach (MetaData *metaData, metaDataList)
		{
			if (metaData->hasValue(objectId, field))
			{
				value = metaData->value(objectId, field);
				hasValue = true;

				break;
			}
		}

		if (hasValue)
		{
			bool ok = true;
			int indexValue = index.toInt(&ok);

			if (ok)
			{
				if (indexValue != -1)
				{
					return indexValue < value.split(',').size();
				}
				else
				{
					return true;
				}
			}
		}
	}

	return false;
}

QString ObjectData::defaultFieldValue(const QString &objectId, const QString &fieldId) const
{
	MetaDataList metaDataList = this->resolveDefaultField(objectId, fieldId);

	if (!metaDataList.isEmpty())
	{
		const QString field = this->metaData()->value(fieldId, "field");
		/*
		 * The index is used for button positions for example when multiple fields share the same field name such as "Buttonpos".
		 */
		const QString index = this->metaData()->value(fieldId, "index");
		bool hasValue = false;
		QString value;

		foreach (MetaData *metaData, metaDataList)
		{
			if (metaData->hasValue(objectId, field))
			{
				value = metaData->value(objectId, field);
				hasValue = true;

				break;
			}
		}

		if (hasValue)
		{
			bool ok = true;
			int indexValue = index.toInt(&ok);

			if (ok)
			{
				if (indexValue != -1)
				{
					const QStringList values = value.split(',');

					if (indexValue < values.size())
					{
						value = values[indexValue];
					}
				}

				return value;
			}
			else
			{
				qDebug() << "Invalid index value" << index << "for object" << objectId << "with field" << fieldId;
			}
		}
	}

	qDebug() << "Data value not found:" << objectId << fieldId;

	return "";
}

ObjectData::ObjectTabEntries ObjectData::objectTabEntries(const QString &fieldType) const
{
	/*
	 * Tilesets are stored in "UI/WorldEditData.txt" not in "UI/UnitEditorData.txt".
	 */
	if (fieldType == "tilesetList" && this->source()->sharedData()->worldEditData().get() != 0)
	{
		ObjectTabEntries entries;
		entries.push_back(ObjectData::ObjectTabEntry("*", "WESTRING_EVERY"));

		const map::Txt::Section *section = boost::polymorphic_cast<const TxtTextSource*>(this->source()->sharedData()->worldEditData()->textSource())->section("TileSets");

		if (section != 0)
		{
			for (std::size_t i = 0; i < section->entries.size(); ++i)
			{
				const QStringList values = QString::fromUtf8(section->entries[i].second.c_str()).split(',');

				if (!values.isEmpty())
				{
					entries.push_back(ObjectData::ObjectTabEntry(QString::fromUtf8(section->entries[i].first.c_str()), values[0]));
				}
			}
		}

		return entries;
	}

	if (this->objectTabData() == 0)
	{
		qDebug() << "No object tab data defined.";

		return ObjectData::ObjectTabEntries();
	}

	if (this->objectTabData()->hasValue(fieldType, "NumValues"))
	{
		const QString numValues = this->objectTabData()->value(fieldType, "NumValues");
		bool ok = false;
		int num = numValues.toInt(&ok);

		if (ok)
		{
			ObjectTabEntries entries;

			for (int i = 0; i < num; ++i)
			{
				QStringList entryValues = this->objectTabData()->value(fieldType, QString("%1").arg(i, 2, 10, QChar('0'))).split(',');

				if (entryValues.size() == 2)
				{
					entries.push_back(ObjectTabEntry(entryValues[0], entryValues[1]));
				}
			}

			bool sortIt = false;

			if (this->objectTabData()->hasValue(fieldType, "Sort"))
			{
				if (this->objectTabData()->value(fieldType, "Sort") == "1")
				{
					sortIt = true;
					// TODO Flag indicating to sort the list by name in editor display
					//qSort(entries.begin(), entries.end());
				}
			}

			return entries;
		}
		else
		{
			throw Exception(boost::format(_("Invalid \"NumValues\" entry for field type %1%.")) % fieldType.toUtf8().constData());
		}
	}

	return ObjectTabEntries();
}

map::Value ObjectData::value(const QString &fieldId, const QString &value) const
{
	const map::Value::Type type = this->fieldType(fieldId);

	switch (type)
	{
		case map::Value::Type::Integer:
		{
			int32 data = boost::numeric_cast<int32>(value.toInt());

			return map::Value(data);
		}

		case map::Value::Type::Real:
		case map::Value::Type::Unreal:
		{
			float32 data = boost::numeric_cast<float32>(value.toDouble());

			return map::Value(data);
		}

		case map::Value::Type::String:
		case map::Value::Type::RegenerationType:
		case map::Value::Type::AttackType:
		case map::Value::Type::WeaponType:
		case map::Value::Type::TargetType:
		case map::Value::Type::MoveType:
		case map::Value::Type::DefenseType:
		case map::Value::Type::PathingTexture:
		case map::Value::Type::MissileArt:
		case map::Value::Type::AttributeType:
		case map::Value::Type::AttackBits:
		{
			return map::Value(value.toUtf8().constData(), type);
		}

		case map::Value::Type::Boolean:
		{
			return map::Value((bool)value.toInt());
		}

		case map::Value::Type::Character:
		{
			return map::Value(value.toUtf8().constData()[0]);
		}

		case map::Value::Type::StringList:
		case map::Value::Type::UpgradeList:
		case map::Value::Type::UnitList:
		case map::Value::Type::ItemList:
		case map::Value::Type::AbilityList:
		case map::Value::Type::HeroAbilityList:
		{
			map::List vector;

			if (!value.isEmpty())
			{
				QStringList list = value.split(',');

				foreach (QString var, list)
				{
					vector.push_back(var.toUtf8().constData());
				}
			}

			return map::Value(vector, type);
		}
	}

	return map::Value(0);
}

void ObjectData::createObject(const QString& originalObjectId, const QString& customObjectId)
{
	assert(!originalObjectId.isEmpty());
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		iterator = this->m_objects.insert(objectId, Modifications());

		/*
		 * Created a new custom object.
		 */
		if (!customObjectId.isEmpty())
		{
			emit objectCreation(originalObjectId, customObjectId);
		}
	}
}

void ObjectData::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId, const map::CustomObjects::Modification &modification)
{
	assert(!originalObjectId.isEmpty());
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		createObject(originalObjectId, customObjectId);
		iterator = this->m_objects.find(objectId);
	}

	if (iterator != this->m_objects.end())
	{
		iterator.value().insert(fieldId, modification);
		emit fieldModification(originalObjectId, customObjectId, fieldId);
	}
}

void ObjectData::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const QString &value)
{
	map::CustomObjects::Modification modification(this->type());
	modification.setValueId(map::stringToId(fieldId.toUtf8().constData()));
	modification.value() = this->value(fieldId, value);

	this->modifyField(originalObjectId, customObjectId, fieldId, modification);
}

void ObjectData::resetField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return;
	}

	iterator.value().remove(fieldId);
	emit modificationReset(originalObjectId, customObjectId, fieldId);

	if (iterator.value().empty())
	{
		this->m_objects.erase(iterator);

		emit objectReset(originalObjectId, customObjectId);
	}
}

bool ObjectData::isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	return iterator.value().contains(fieldId);
}

void ObjectData::resetObject(const QString &originalObjectId, const QString &customObjectId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator != this->m_objects.end())
	{
		iterator.value().clear();
	}
}

void ObjectData::deleteObject(const QString &originalObjectId, const QString &customObjectId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator != this->m_objects.end())
	{
		m_objects.erase(iterator);
	}

	/*
	 * Only custom items are actually removed.
	 */
	if (!customObjectId.isEmpty())
	{
		emit objectRemoval(originalObjectId, customObjectId);
	}
}

bool ObjectData::isObjectModified(const QString &originalObjectId, const QString &customObjectId) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	return true;
}

void ObjectData::clearModifications()
{
	while (!this->m_objects.isEmpty())
	{
		deleteObject(this->m_objects.begin().key().first, this->m_objects.begin().key().second);
	}
}

bool ObjectData::fieldModificiation(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, map::CustomObjects::Modification &modification) const
{

	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	Modifications::const_iterator iterator2 = iterator.value().find(fieldId);

	if (iterator2 == iterator.value().end())
	{
		return false;
	}

	modification = iterator2.value();

	return true;

}

bool ObjectData::hasFieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	const Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator != this->m_objects.end())
	{
		return iterator->find(fieldId) != iterator->end();
	}
	else
	{
		return hasDefaultFieldValue(originalObjectId, fieldId);
	}

	return false;
}

QString ObjectData::fieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const
{
	map::CustomObjects::Modification modification(this->type());

	if (fieldModificiation(originalObjectId, customObjectId, fieldId, modification))
	{
		return valueToString(modification.value());
	}
	// Otherwise return the default value from Warcraft III
	else
	{
		return this->defaultFieldValue(originalObjectId, fieldId);
	}

	return "";
}

QString ObjectData::fieldReadableValue(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId) const
{
	const QString fieldValue = this->fieldValue(originalObjectId, customObjectId, fieldId);
	const QString fieldType = this->metaData()->value(fieldId, "type");

	/*
	 * If value is "-" or "_" for example it should return a default value.
	 */
	bool ok = true;

	if (fieldType == "int")
	{
		fieldValue.toInt(&ok);

		if (!ok)
		{
			return "0";
		}
	}
	else if (fieldType == "real" || fieldType == "unreal")
	{
		fieldValue.toDouble(&ok);

		if (!ok)
		{
			return "0.0";
		}
	}
	else if (fieldType == "bool")
	{
		fieldValue.toInt(&ok);

		if (!ok)
		{
			return "0";
		}
	}


	if (fieldType == "int" || fieldType == "real" || fieldType == "unreal" || fieldType == "string" || fieldType == "char")
	{
		return fieldValue;
	}
	else if (fieldType == "bool")
	{
		if (fieldValue == "1")
		{
			return this->source()->sharedData()->tr("WESTRING_TRUE");
		}

		return this->source()->sharedData()->tr("WESTRING_FALSE");
	}
	else if (fieldType == "model" || fieldType == "icon")
	{
		QString result = fieldValue;
		int index = result.lastIndexOf('\\');

		if (index != -1 && result.size() > index + 1)
		{
			result = result.mid(index + 1);
		}

		index = result.lastIndexOf('.');

		if (index != -1)
		{
			result = result.mid(0, index);
		}

		return result;
	}
	/*
	 * For object lists we need all the object names.
	 * NOTE the object data must be loaded before. Make sure that the table view or whatever loads it on request before displaying it.
	 */
	else if (this->fieldTypeIsObjectList(fieldType))
	{
		ObjectData *objectData = this->source()->sharedData()->sharedObjectData()->resolveByFieldType(fieldType);

		if (objectData != 0)
		{
			QStringList fieldValues = fieldValue.split(',');
			QStringList result;

			foreach (QString value, fieldValues)
			{
				// TODO which one is the custom ID
				result.push_back(objectData->objectName(value, ""));
			}

			return result.join(", ");
		}
		else
		{
			qDebug() << "Could not resolve field type" << fieldType;
		}
	}
	else
	{
		const ObjectTabEntries entries = this->objectTabEntries(fieldType);

		if (entries.isEmpty())
		{
			return fieldValue;
		}

		const QStringList fieldValues = fieldValue.split(',');
		QStringList result;

		foreach (QString value, fieldValues)
		{
			bool found = false;

			/*
			* If there is a section for this field type search for the entry corresponding to value and get its readable description.
			*/
			foreach (ObjectTabEntry entry, entries)
			{
				/*
				 * It seems that values are existing like "Summoned" where it should be called "summoned"
				 */
				if (entry.first.toLower() == value.toLower())
				{
					 /*
					  * Many translatable strings contain & characters to indicate the Alt shortcut
					  * but we do not want to see this when listing the values.
					  */
					result.push_back(this->source()->sharedData()->tr(entry.second).remove('&'));

					found = true;

					break;
				}
			}

			if (!found)
			{
				result.push_back(value);

				qDebug() << "Missing entry for value" << value;
			}
		}

		return result.join(", ");
	}

	return fieldValue;
}

void ObjectData::importCustomUnits(const map::CustomUnits &units)
{
	this->clearModifications();

	for (map::CustomUnits::Table::size_type i = 0; i < units.originalTable().size(); ++i)
	{
		const map::CustomUnits::Unit &unit = units.originalTable()[i];
		const QString originalObjectId = map::idToString(unit.originalId()).c_str();

		/*
		 * In Reign of Chaos items are stored in custom units as well.
		 * If there is no original object it does not belong to the current object data.
		 */
		if (this->standardObjectIds().contains(originalObjectId))
		{
			const QString customObjectId = map::idToString(unit.customId()).c_str();

			for (map::CustomUnits::Unit::Modifications::size_type j = 0; j < unit.modifications().size(); ++j)
			{
				const map::CustomUnits::Modification &modification = unit.modifications()[j];
				this->modifyField(originalObjectId, customObjectId, map::idToString(modification.valueId()).c_str(), unitToObjectModification(modification));
			}
		}
	}

	for (map::CustomUnits::Table::size_type i = 0; i < units.customTable().size(); ++i)
	{
		const map::CustomUnits::Unit &unit = units.customTable()[i];
		const QString originalObjectId = map::idToString(unit.originalId()).c_str();

		/*
		 * In Reign of Chaos items are stored in custom units as well.
		 * If there is no original object it does not belong to the current object data.
		 */
		if (this->standardObjectIds().contains(originalObjectId))
		{
			const QString customObjectId = map::idToString(unit.customId()).c_str();

			for (map::CustomUnits::Unit::Modifications::size_type j = 0; j < unit.modifications().size(); ++j)
			{
				const map::CustomUnits::Modification &modification = unit.modifications()[j];
				this->modifyField(originalObjectId, customObjectId, map::idToString(modification.valueId()).c_str(), unitToObjectModification(modification));
			}
		}
	}

	//units.customTable()
	// TODO read all objects
}

void ObjectData::importCustomObjects(const map::CustomObjects& objects)
{
	this->clearModifications();

	for (map::CustomObjects::Table::size_type i = 0; i < objects.originalTable().size(); ++i)
	{
		const map::CustomObjects::Object &object = *boost::polymorphic_cast<const map::CustomObjects::Object*>(&objects.originalTable()[i]);
		const QString originalObjectId = map::idToString(object.originalId()).c_str();
		const QString customObjectId = map::idToString(object.customId()).c_str();

		for (map::CustomObjects::Object::Modifications::size_type j = 0; j < object.modifications().size(); ++j)
		{
			const map::CustomObjects::Modification &modification = *boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object.modifications()[j]);
			this->modifyField(originalObjectId, customObjectId, map::idToString(modification.valueId()).c_str(), unitToObjectModification(modification));
		}
	}

	for (map::CustomObjects::Table::size_type i = 0; i < objects.customTable().size(); ++i)
	{
		const map::CustomObjects::Object &object = *boost::polymorphic_cast<const map::CustomObjects::Object*>(&objects.customTable()[i]);
		const QString originalObjectId = map::idToString(object.originalId()).c_str();
		const QString customObjectId = map::idToString(object.customId()).c_str();

		for (map::CustomObjects::Object::Modifications::size_type j = 0; j < object.modifications().size(); ++j)
		{
			const map::CustomObjects::Modification &modification = *boost::polymorphic_cast<const map::CustomObjects::Modification*>(&object.modifications()[j]);
			this->modifyField(originalObjectId, customObjectId, map::idToString(modification.valueId()).c_str(), unitToObjectModification(modification));
		}
	}

}

map::CustomObjects::Modification ObjectData::unitToObjectModification(const map::CustomUnits::Modification& modification) const
{
	map::CustomObjects::Modification result(this->type());
	result.setValueId(modification.valueId());
	result.setData(0);
	result.setLevel(0);
	result.value() = modification.value();

	return result;
}

map::CustomUnits ObjectData::customUnits() const
{
	map::CustomUnits units;

	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		map::CustomUnits::Unit unit;
		// TODO which one is the custom id
		unit.setOriginalId(map::stringToId(iterator.key().first.toStdString()));
		unit.setCustomId(map::stringToId(iterator.key().second.toStdString()));

		foreach (map::CustomObjects::Modification modification, iterator.value())
		{
			unit.modifications().push_back(new map::CustomUnits::Modification(modification));
		}

		/*
		 * No custom ID means it is a standard unit.
		 */
		if (iterator.key().second.isEmpty())
		{
			units.originalTable().push_back(new map::CustomUnits::Unit(unit));
		}
		else
		{
			units.customTable().push_back(new map::CustomUnits::Unit(unit));
		}
	}

	return units;
}

map::CustomObjects ObjectData::customObjects() const
{
	map::CustomObjects objects = map::CustomObjects(map::CustomObjects::Type::Units);

	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		map::CustomObjects::Object object(this->type());
		// TODO which one is the custom id
		object.setOriginalId(map::stringToId(iterator.key().first.toStdString()));
		object.setCustomId(map::stringToId(iterator.key().second.toStdString()));

		foreach (map::CustomObjects::Modification modification, iterator.value())
		{
			object.modifications().push_back(new map::CustomObjects::Modification(modification));
		}

		/*
		 * No custom ID means it is a standard unit.
		 */
		if (iterator.key().second.isEmpty())
		{
			objects.originalTable().push_back(new map::CustomObjects::Object(object));
		}
		else
		{
			objects.customTable().push_back(new map::CustomObjects::Object(object));
		}
	}

	return objects;
}

map::CustomObjects::Object ObjectData::customObject(const QString &originalObjectId, const QString &customObjectId) const
{
	map::CustomObjects::Object object(this->type());
	// TODO which one is the custom id
	object.setOriginalId(map::stringToId(originalObjectId.toStdString()));
	object.setCustomId(map::stringToId(customObjectId.toStdString()));

	Objects::const_iterator iterator = this->m_objects.find(ObjectId(originalObjectId, customObjectId));

	if (iterator != this->m_objects.constEnd())
	{
		foreach (map::CustomObjects::Modification modification, iterator.value())
		{
			object.modifications().push_back(new map::CustomObjects::Modification(modification));
		}
	}

	return object;
}

QString ObjectData::objectId(int value) const
{
	QString result = QString::number(value, 16);

	if (result.size() > 4)
	{
		throw Exception();
	}
	else if (result.size() < 4)
	{
		for (int i = result.size(); i < 4; ++i)
		{
			result.prepend('0');
		}
	}

	return result.toUpper();
}

QString ObjectData::nextCustomObjectId() const
{
	/*
	 * Get all custom IDs in the correct order from the minimum ID to the maximum ID.
	 */
	QSet<int> customIds;

	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		bool ok = true;
		int idNumber = iterator.key().second.mid(1).toInt(&ok, 16);

		if (!ok)
		{
			throw Exception();
		}
		else
		{
			customIds.insert(idNumber);
		}
	}

	QString result;
	int last = -1;

	foreach (int id, customIds)
	{
		if (last != -1)
		{
			if (id > last + 1)
			{
				result = objectId(last + 1);

				break;
			}
		}

		last = id;
	}

	if (result.isEmpty() && last != -1)
	{
		result = objectId(last + 1);
	}
	else
	{
		result = objectId(0);
	}

	if (result.size() != 4)
	{
		qDebug() << "Invalid ID" << result;

		throw Exception();
	}
	// The first hexa decimal value is reserved for the type.
	else if (result[0] != '0')
	{
		qDebug() << "ID overflow" << result;

		throw Exception();
	}

	switch (type())
	{
		case map::CustomObjects::Type::Abilities:
		{
			result[0] = 'A';

			break;
		}

		case map::CustomObjects::Type::Units:
		{
			result[0] = 'U';

			break;
		}

		case map::CustomObjects::Type::Items:
		{
			result[0] = 'I';

			break;
		}

		case map::CustomObjects::Type::Doodads:
		{
			result[0] = 'D';

			break;
		}

		default:
		{
			result[0] = 'Z';

			break;
		}
	}

	return result;
}

void ObjectData::applyMapStrings(map::W3m &w3m)
{
	/*
	 * All string entries are usually stored in the map strings.
	 */
	if (w3m.strings().get() != 0)
	{
		mpq::File stringsFile = w3m.findFile(w3m.strings()->fileName());

		if (stringsFile.isValid())
		{
			QTemporaryFile file;
			// keep suffix
			file.setFileTemplate("XXXXXX.wts");
			file.open();
			ofstream out(file.fileName().toUtf8().constData());
			stringsFile.writeData(out);
			out.close();
			file.close();

			qDebug() << "Entries" << w3m.strings()->entries().size();

			MetaData mapStringsMetaData(file.fileName());
			mapStringsMetaData.setSource(this->source());
			mapStringsMetaData.load();

			const QString triggerStringPrefix = "TRIGSTR_";

			for (ObjectData::Objects::const_iterator iterator = this->objects().begin(); iterator != this->objects().end(); ++iterator)
			{
				for (ObjectData::Modifications::const_iterator modIterator = iterator.value().begin(); modIterator != iterator.value().end(); ++modIterator)
				{
					if (modIterator.value().value().type() == map::Value::Type::String)
					{
						const QString key = valueToString(modIterator.value().value());

						qDebug() << "key" << key;

						if (key.startsWith(triggerStringPrefix))
						{
							bool ok = true;
							const QString index = key.mid(triggerStringPrefix.size());
							int row = index.toInt(&ok);

							if (ok)
							{
								qDebug() << "Full" << key;
								qDebug() << "index" << index;
								qDebug() << "Row" << row;

								if (mapStringsMetaData.hasValue(row, ""))
								{
									qDebug() << "Translated string:" <<  mapStringsMetaData.value(row, "");
									this->modifyField(iterator.key().first, iterator.key().second, modIterator.key(), mapStringsMetaData.value(row, ""));
								}
								else
								{
									qDebug() << "Missing translated string" << row;
								}
							}
							else
							{
								qDebug() << "Invalid string index" << index;
							}
						}
					}
				}
			}
		}
	}
}

}

}
