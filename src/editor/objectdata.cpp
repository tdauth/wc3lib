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
#include <QtWidgets/QtWidgets>

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
		|| fieldType == "modelList"
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

bool ObjectData::loadOnRequest(QWidget *widget)
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
		catch (const std::exception &e)
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
		|| fieldType == "itemList"
		;
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


bool ObjectData::hasDefaultFieldValue(const QString &objectId, const QString &fieldId, int level) const
{
	bool exists = false;
	this->defaultFieldValue(objectId, fieldId, level, exists);

	return exists;
}

QString ObjectData::defaultFieldValue(const QString &objectId, const QString &fieldId, int level) const
{
	bool exists = false;
	return this->defaultFieldValue(objectId, fieldId, level, exists);
}

bool ObjectData::repeateField(const QString& fieldId) const
{
	if (this->metaData()->hasValue(fieldId, "repeat"))
	{
		bool ok = false;
		int count = this->metaData()->value(fieldId, "repeat").toInt(&ok);

		return (ok && count > 0);
	}

	return false;
}

ObjectData::ObjectTabEntries ObjectData::objectTabEntries(const QString &fieldType) const
{
	/*
	 * Tilesets are stored in "UI/WorldEditData.txt" not in "UI/UnitEditorData.txt".
	 */
	if ((fieldType == "tilesetList" || fieldType == "doodadCategory" || fieldType == "destructableCategory") && this->source()->sharedData()->worldEditData().get() != 0)
	{
		// get the corresponding section key
		const QString sectionKey = fieldType == "doodadCategory" ? "DoodadCategories" : (fieldType == "destructableCategory" ? "DestructibleCategories" : (fieldType == "tilesetList" ? "TileSets" : fieldType));

		ObjectTabEntries entries;

		if (fieldType == "tilesetList")
		{
			entries.push_back(ObjectData::ObjectTabEntry("*", "WESTRING_EVERY"));
		}

		const map::Txt::Section *section = boost::polymorphic_cast<const TxtTextSource*>(this->source()->sharedData()->worldEditData()->textSource())->section(sectionKey);

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

	const TxtTextSource *txtSource = dynamic_cast<const TxtTextSource*>(this->objectTabData()->textSource());
	const bool hasFieldSection = (txtSource != nullptr && txtSource->sectionKeys().contains(fieldType)) || this->objectTabData()->hasValue(fieldType, "NumValues");

	if (hasFieldSection)
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

ObjectData::Objects::iterator ObjectData::createObject(const QString &originalObjectId, const QString &customObjectId)
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

	return iterator;
}

void ObjectData::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const map::CustomObjects::Modification &modification)
{
	assert(!originalObjectId.isEmpty());
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = createObject(originalObjectId, customObjectId); // only creates it when it does not exist

	if (iterator != this->m_objects.end())
	{
		const FieldId fieldIdKey(fieldId, modification.level());

		iterator.value().insert(fieldIdKey, modification);

		emit fieldModification(originalObjectId, customObjectId, fieldId, modification.level());
	}
}

void ObjectData::modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const QString &value, int level)
{
	map::CustomObjects::Modification modification(this->type());
	modification.setValueId(map::stringToId(fieldId.toUtf8().constData()));
	modification.value() = this->value(fieldId, value);
	modification.setLevel(level);

	this->modifyField(originalObjectId, customObjectId, fieldId, modification);
}

void ObjectData::resetField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		qDebug() << "Reset: Could not find object" << originalObjectId << customObjectId;

		return;
	}

	FieldId fieldIdKey(fieldId, level);
	const int removalCount = iterator.value().remove(fieldIdKey);

	if (removalCount == 0)
	{
		qDebug() << "Reset field not found for object " << originalObjectId << customObjectId << " - " << fieldId;
	}

	emit modificationReset(originalObjectId, customObjectId, fieldId, level);

	if (iterator.value().empty())
	{
		this->m_objects.erase(iterator);

		emit objectReset(originalObjectId, customObjectId);
	}
}

bool ObjectData::isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString& fieldId, int level) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	FieldId fieldIdKey(fieldId, level);
	return iterator.value().contains(fieldIdKey);
}

void ObjectData::resetObject(const QString &originalObjectId, const QString &customObjectId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::iterator iterator = this->m_objects.find(objectId);

	if (iterator != this->m_objects.end())
	{
		iterator.value().clear();

		emit objectReset(originalObjectId, customObjectId);
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

bool ObjectData::fieldModificiation(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, map::CustomObjects::Modification &modification, int level) const
{

	const ObjectId objectId(originalObjectId, customObjectId);
	Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator == this->m_objects.end())
	{
		return false;
	}

	const FieldId fieldIdKey(fieldId, level);
	Modifications::const_iterator iterator2 = iterator.value().find(fieldIdKey);

	if (iterator2 == iterator.value().end())
	{
		return false;
	}

	modification = iterator2.value();

	return true;

}

bool ObjectData::hasFieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	const ObjectId objectId(originalObjectId, customObjectId);
	const Objects::const_iterator iterator = this->m_objects.find(objectId);

	if (iterator != this->m_objects.end())
	{
		const FieldId fieldIdKey(fieldId, level);

		return iterator->find(fieldIdKey) != iterator->end();
	}
	else
	{
		return hasDefaultFieldValue(originalObjectId, fieldId, level);
	}

	return false;
}

QString ObjectData::fieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level) const
{
	map::CustomObjects::Modification modification(this->type());

	if (fieldModificiation(originalObjectId, customObjectId, fieldId, modification, level))
	{
		return valueToString(modification.value());
	}
	// Otherwise return the default value from Warcraft III
	else
	{
		return this->defaultFieldValue(originalObjectId, fieldId, level);
	}

	return "";
}

QString ObjectData::fieldReadableValue(const QString& originalObjectId, const QString& customObjectId, const QString& fieldId, int level) const
{
	// cut the quotes
	const QString fieldValue = MetaData::fromSlkString(this->fieldValue(originalObjectId, customObjectId, fieldId, level));
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

	if (fieldType == "int" || fieldType == "real" || fieldType == "unreal" || fieldType == "char" || fieldType == "string")
	{
		// For these entries the readable values are empty in Warcraft's object editor.
		if (fieldValue == "-" || fieldValue == "_")
		{
			return QString();
		}

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
		// For these entries the readable values are empty in Warcraft's object editor.
		if (fieldValue == "-" || fieldValue == "_")
		{
			return QString();
		}

		return fieldValue;
	}
	else if (fieldType == "abilCode")
	{
		// check if it is a custom or default object
		if (this->source()->sharedData()->sharedObjectData()->abilityData()->standardObjectIds().contains(fieldValue))
		{
			return this->source()->sharedData()->sharedObjectData()->abilityData()->objectName(fieldValue, "");
		}

		return this->source()->sharedData()->sharedObjectData()->abilityData()->objectName("", fieldValue);
	}
	else if (fieldType == "unitCode")
	{
		// check if it is a custom or default object
		if (this->source()->sharedData()->sharedObjectData()->unitData()->standardObjectIds().contains(fieldValue))
		{
			return this->source()->sharedData()->sharedObjectData()->unitData()->objectName(fieldValue, "");
		}

		return this->source()->sharedData()->sharedObjectData()->unitData()->objectName("", fieldValue);
	}
	/*
	 * For object lists we need all the object names.
	 * NOTE the object data must be loaded before. Make sure that the table view or whatever loads it on request before displaying it.
	 */
	else if (this->fieldTypeIsObjectList(fieldType))
	{
		const SharedObjectData::ObjectDataList objectDataList = this->source()->sharedData()->sharedObjectData()->resolveByFieldType(fieldType);

		if (!objectDataList.isEmpty())
		{
			QStringList fieldValues = fieldValue.split(',');
			QStringList result;

			foreach (QString value, fieldValues)
			{
				const QStringList ids = value.split(':');
				const QString originalId = ids.size() >= 1 ? ids[0] : "";
				const QString customId = ids.size() >= 2 ? ids[1] : "";

				// now check from which object data it is
				for (int i = 0; i < objectDataList.size(); ++i)
				{
					if (objectDataList[i]->hasOriginalObject(originalId))
					{
						result.push_back(objectDataList[i]->objectName(originalId, customId));

						break;
					}
				}
			}

			return result.join(", ");
		}
		else
		{
			qDebug() << "Could not resolve field type" << fieldType;
		}
	}
	// Otherwise we expect an object tab entry with predefined values. These values usually have corresponding WESTRING entries which must be shown.
	else
	{
		const ObjectTabEntries entries = this->objectTabEntries(fieldType);

		qDebug() << "object tab entries: " << entries.size() << " for field type " << fieldType;

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
					  *
					  * If the string key is not found simply return the string as default value since it is the entry value.
					  */
					result.push_back(this->source()->sharedData()->tr(entry.second, "WorldEditStrings", entry.second).remove('&'));

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

bool ObjectData::hasOriginalObject(const QString &originalObjectId) const
{
	return this->standardObjectIds().contains(originalObjectId);
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
			const QString fieldId = map::idToString(modification.valueId()).c_str();
			this->modifyField(originalObjectId, customObjectId, fieldId, modification);
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
			const QString fieldId = map::idToString(modification.valueId()).c_str();
			this->modifyField(originalObjectId, customObjectId, fieldId, modification);
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
	if (!hasCustomUnits())
	{
		throw Exception(_("Does not support custom units."));
	}

	map::CustomUnits units;

	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		std::auto_ptr<map::CustomUnits::Unit> unit(new map::CustomUnits::Unit());
		unit->setOriginalId(map::stringToId(iterator.key().originalObjectId().toUtf8().constData()));
		unit->setCustomId(map::stringToId(iterator.key().customObjectId().toUtf8().constData()));

		foreach (map::CustomObjects::Modification modification, iterator.value())
		{
			unit->modifications().push_back(new map::CustomUnits::Modification(modification));
		}

		/*
		 * No custom ID means it is a standard unit.
		 */
		if (iterator.key().customObjectId().isEmpty())
		{
			units.originalTable().push_back(unit);
		}
		else
		{
			units.customTable().push_back(unit);
		}
	}

	return units;
}

map::CustomObjects ObjectData::customObjects() const
{
	if (!hasCustomObjects())
	{
		throw Exception(_("Does not support custom objects."));
	}

	map::CustomObjects objects = map::CustomObjects(this->type());

	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		std::auto_ptr<map::CustomObjects::Object> object(new map::CustomObjects::Object(this->type()));
		const QString originalObjectId = iterator.key().originalObjectId();
		const QString customObjectId = iterator.key().customObjectId();
		object->setOriginalId(map::stringToId(originalObjectId.toUtf8().constData()));
		object->setCustomId(map::stringToId(customObjectId.toUtf8().constData()));

		foreach (const map::CustomObjects::Modification &modification, iterator.value())
		{
			object->modifications().push_back(new map::CustomObjects::Modification(modification));
		}

		/*
		 * No custom ID means it is a standard unit.
		 */
		if (customObjectId.isEmpty())
		{
			objects.originalTable().push_back(object);
		}
		else
		{
			objects.customTable().push_back(object);
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

QString ObjectData::nextCustomObjectId(const QString &originalObjectId) const
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

		case map::CustomObjects::Type::Destructibles:
		{
			result[0] = 'B';

			break;
		}


		case map::CustomObjects::Type::Buffs:
		{
			// buff
			if (originalObjectId.startsWith('B'))
			{
				result[0] = 'B';
			}
			// effect
			else
			{
				result[0] = 'X';
			}

			break;
		}

		case map::CustomObjects::Type::Upgrades:
		{
			result[0] = 'G';

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
			stringsFile.decompress(out);
			out.close();
			file.close();

			qDebug() << "Entries" << w3m.strings()->entries().size();

			this->applyMapStrings(file.fileName());
		}
	}
}

void ObjectData::applyMapStrings(const QUrl &url)
{
	MetaData mapStringsMetaData(url);
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
							const QString translatedString = mapStringsMetaData.value(row, "");
							qDebug() << "Translated string:" <<  translatedString;
							this->modifyField(iterator.key().originalObjectId(), iterator.key().customObjectId(), modIterator.key().fieldId(), translatedString);
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

long long ObjectData::compress()
{
	qDebug() << "Compressing object data:";
	long long counter = 0;

	/*
	 * Find all modified fields which are not necessarily used for the objects.
	 * For example if you modify a hero only field and then make the object from a hero to a normal object the modification remains but is not visible anymore in the object editor.
	 * Besides there might be modifications for higher levels than the object actually has.
	 * NOTE This discards maybe still required modifications.
	 */
	for (Objects::iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		//qDebug() << "Before getting object key";
		const ObjectId &id = iterator.key();
		const int objectLevels = this->objectLevels(id.originalObjectId(), id.customObjectId());
		// Always use the maximum of possible levels. There might be modifications which are not required anymore of higher levels.
		const int levels = 100;

		//qDebug() << "Object " << id;

		// iterate through all fields which are for item fields only and delete their modifications
		for (int i = 0; i < this->metaData()->rows(); ++i)
		{
			//qDebug() << "Before field";

			const QString fieldId = this->metaData()->value(i, "ID");
			//qDebug() << "Field: " << fieldId;
			const bool repeatField = this->repeateField(fieldId);

			for (int j = 0; j < levels && (j == 0 || repeatField); ++j)
			{
				const int level = repeatField ? j + 1 : j;

				// TODO check if the modification has the same value as the default value. This check probably requires more than just a == string comparison since different values of strings might still have the same meaning.
				// This would remove some field modifications at the moment for which the default values are not calculated properly. Therefore check at least first if a default value is found.
				if (this->isFieldModified(id.originalObjectId(), id.customObjectId(), fieldId, level) && (this->hideField(id.originalObjectId(), id.customObjectId(), fieldId, level) || (repeatField && level > objectLevels) || (this->hasDefaultFieldValue(id.originalObjectId(), fieldId, level) && this->fieldValue(id.originalObjectId(), id.customObjectId(), fieldId, level) == this->defaultFieldValue(id.originalObjectId(), fieldId, level))))
				{
					//qDebug() << "Field is modified";

					this->resetField(id.originalObjectId(), id.customObjectId(), fieldId, level);
					++counter;
					//qDebug() << "Compressing " << id.first << ":" << id.second << " field " << fieldId;
				}
			}

			//qDebug() << "Field done";
		}

		//qDebug() << "Object done";
	}

	qDebug() << "Compression done with" << counter << "items";

	return counter;
}

QString ObjectData::baseOfCustomObjectId(const QString &customObjectId) const
{
	for (Objects::const_iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		const ObjectId &id = iterator.key();

		if (id.customObjectId() == customObjectId)
		{
			qDebug() << "Found base for" << customObjectId << ":" << id.originalObjectId();

			return id.originalObjectId();
		}
	}

	qDebug() << "Found no base for" << customObjectId;

	return "";
}

QStringList ObjectData::validateTooltipReferences()
{
	if (this->metaData() == nullptr || this->metaData()->textSource() == nullptr)
	{
		qDebug() << "Load meta data before validating.";

		return QStringList();
	}

	qDebug() << "Validating object data:";

	QStringList allFields;

	for (int i = 1; i < metaData()->rows(); ++i)
	{
		allFields.push_back(this->metaData()->value(i, "field").toLower()); // TODO is it case sensitive?
	}

	qDebug() << "Got" << allFields.size() << "fields";

	QStringList errors;

	for (Objects::iterator iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator)
	{
		const ObjectId &id = iterator.key();
		const int objectLevels = this->objectLevels(id.originalObjectId(), id.customObjectId());

		// iterate through all fields which are for item fields only and delete their modifications
		for (int i = 0; i < this->metaData()->rows(); ++i)
		{
			//qDebug() << "Before field";

			const QString fieldId = this->metaData()->value(i, "ID");

			const QString fieldType = this->metaData()->value(i, "type");

			//qDebug() << "Field: " << fieldId;
			const bool repeatField = this->repeateField(fieldId);

			if (fieldType != "string")
			{
				//qDebug() << "Field type is not string" << fieldType;

				continue;
			}

			for (int j = 0; j < objectLevels && (j == 0 || repeatField); ++j)
			{
				const int level = repeatField ? j + 1 : j;

				// TODO check if the modification has the same value as the default value. This check probably requires more than just a == string comparison since different values of strings might still have the same meaning.
				// This would remove some field modifications at the moment for which the default values are not calculated properly. Therefore check at least first if a default value is found.
				if (this->isFieldModified(id.originalObjectId(), id.customObjectId(), fieldId, level))
				{
					//qDebug() << "Field is modified";

					const QString value = this->fieldValue(id.originalObjectId(), id.customObjectId(), fieldId, level);
					errors << this->validateTooltipReference(value, allFields, true); // first call is recursive
					//qDebug() << "Compressing " << id.first << ":" << id.second << " field " << fieldId;
				}
			}

			//qDebug() << "Field done";
		}

		//qDebug() << "Object done";
	}

	qDebug() << "Validation done with" << errors.size() << "items";

	return errors;
}

namespace
{

QString objectDataName(map::CustomObjects::Type type)
{
	switch (type)
	{
		case map::CustomObjects::Type::Units:
		{
			return QObject::tr("Units");
		}

		case map::CustomObjects::Type::Items:
		{
			return QObject::tr("Items");
		}

		case map::CustomObjects::Type::Abilities:
		{
			return QObject::tr("Abilities");
		}

		case map::CustomObjects::Type::Doodads:
		{
			return QObject::tr("Doodads");
		}

		case map::CustomObjects::Type::Destructibles:
		{
			return QObject::tr("Destructibles");
		}

		case map::CustomObjects::Type::Buffs:
		{
			return QObject::tr("Buffs");
		}

		case map::CustomObjects::Type::Upgrades:
		{
			return QObject::tr("Upgrades");
		}
	}

	return QObject::tr("Unknown");
}

}

QStringList ObjectData::validateTooltipReference(const QString &tooltip, const QStringList &allFields, bool checkRecursive)
{
	QStringList errors;

	for (int i = tooltip.indexOf("<"); i != -1 && i + 1 < tooltip.size(); i = tooltip.indexOf("<", i + 1))
	{
		const int start = i + 1;

		if (start >= tooltip.size())
		{
			errors.push_back(QString("Staring unfinished reference at the end"));

			break;
		}

		const int end = tooltip.indexOf(">", start);
		const QString reference = tooltip.mid(start, end - start);
		qDebug() << "Reference:" << reference;
		const QStringList tokens = reference.split(","); // could also be a triple with the ,% suffix

		if (tokens.isEmpty() || tokens.size() == 1)
		{
			errors.push_back(QString("Reference has only one token: " + reference));

			continue;
		}

		if (tokens.size() == 3 && tokens[2] != QChar('%'))
		{
			errors.push_back(QString("Expecting % at third token: " + reference));

			continue;
		}

		if (tokens.size() > 3)
		{
			errors.push_back(QString("Too many tokens: " + reference));

			continue;
		}

		const QString objectId = tokens[0];
		const QString fieldName = tokens[1].toLower(); // TODO is it case sensitive?
		const bool isStandardObject = this->standardObjectIds().contains(objectId);
		const QString originalObjectId = isStandardObject ? objectId : baseOfCustomObjectId(objectId);
		const QString customObjectId = isStandardObject ? "" : objectId;
		qDebug() << "Reference as object:" << originalObjectId << ":" << customObjectId;
		qDebug() << "Field ID:" << fieldName;

		// TODO Does not belong to this object data.
		if (originalObjectId.isEmpty())
		{
			bool foundInOtherObjectData = false;

			// Check all OTHER shared object data for the object ID in custom and standard objects:
			if (checkRecursive)
			{
				QLinkedList<ObjectData*> objectData;

				objectData << this->source()->sharedData()->sharedObjectData()->unitData().get();
				objectData << this->source()->sharedData()->sharedObjectData()->itemData().get();
				objectData << this->source()->sharedData()->sharedObjectData()->abilityData().get();
				objectData << this->source()->sharedData()->sharedObjectData()->buffData().get();
				objectData << this->source()->sharedData()->sharedObjectData()->upgradeData().get();
				objectData << this->source()->sharedData()->sharedObjectData()->doodadData().get();
				objectData << this->source()->sharedData()->sharedObjectData()->destructableData().get();

				// Don't use null values or the current object data.
				objectData.removeAll(nullptr);
				objectData.removeAll(this);

				for (ObjectData *data : objectData)
				{
					/*
					 * Add all object IDs of the sub object data.
					 */
					QStringList objectIds;

					for (const QString &standardObjectId : data->standardObjectIds())
					{
						objectIds.push_back(standardObjectId);
					}

					for (Objects::key_type key : data->objects().keys())
					{
						objectIds.push_back(key.customObjectId());
					}

					for (const QString &checkingObjectId : objectIds)
					{
						if (checkingObjectId == objectId)
						{
							const QStringList subErrors = data->validateTooltipReference(reference, allFields, false);

							if (!subErrors.isEmpty())
							{
								errors.push_back(tr("Checking sub object data %1").arg(objectDataName(data->type())));
								errors << subErrors;
							}

							foundInOtherObjectData = true;

							break;
						}
					}

					if (foundInOtherObjectData)
					{
						break;
					}
				}
			}

			if (!foundInOtherObjectData)
			{
				errors.push_back(QString("Missing original object for " + customObjectId));
			}

			continue;
		}

		/*
		 * Try to find the full field name first.
		 */
		int fieldIndex = allFields.indexOf(fieldName);
		/*
		 * The level is important to check if the actual field of the corresponding level does even exist.
		 */
		int fieldLevel = 0;

		// The level index is also required for fields like Area1 which are field names themselves. Therefore always extract the level bit if it does exist. Otherwise this value stays -1.
		int levelIndex = -1;

		for (int j = 0; j < fieldName.size(); ++j)
		{
			if (fieldName.at(j).isDigit())
			{
				levelIndex = j;

				break;
			}
		}

		/*
		 * If there is a level value, extract it by converting it. Otherwise 0 is used.
		 */
		if (levelIndex != -1)
		{
			bool ok = false;
			fieldLevel = fieldName.mid(levelIndex).toInt(&ok);

			if (!ok)
			{
				errors.push_back(QString("Field level is no integer: \"" + reference + "\""));

				break;
			}
		}

		/*
		 * If the field does not name a field from the meta data it might be one but with a higher level. Therefore it must have specified a level.
		 * Check if the field name without the level suffix does exist in the meta data. For example 'Area42' could be refered but only 'Area1' appears in the meta data.
		 * In this case 'Area' (without the level suffix) has to be compared to the beginning of all field names in the meta data.
		 */
		if (fieldIndex == -1)
		{
			if (levelIndex == -1)
			{
				errors.push_back(QString("Invalid level index for \"" + reference + "\" with the field name \"" + fieldName + "\" which was not found to be a valid field itself."));

				continue;
			}

			/*
			 * TODO handle DataA1 and DataB1 etc. differently. Check the column useSpecific of all possible data values.
			 */
			const QString fieldNameCut = fieldName.mid(0, levelIndex);
			/*
			 * Always use lower case.
			 */
			const QRegExp rx("data[a-z]");
			const bool isDataValue = rx.exactMatch(fieldNameCut);

			/*
			 * Store the possibilities for better error detection.
			 */
			QStringList possibilities;
			const QString hint = isDataValue ? "Detected data value." : "Normal value";

			for (int j = 0; j < allFields.size(); ++j)
			{
				const QString checkingFieldname = allFields[j];
				possibilities.push_back(checkingFieldname);

				if ((!isDataValue && checkingFieldname.startsWith(fieldNameCut)) || (isDataValue && checkingFieldname == "data"))
				{
					/*
					 * Make sure that the rest of the field name is only another level value.
					 */
					if (!isDataValue && fieldNameCut.size() < checkingFieldname.size())
					{
						const QString checkingSuffix = checkingFieldname.mid(fieldNameCut.size());
						// Since the size is samller, it cannot be empty.
						const QRegExp regExLevel("[0-9]+");

						/*
						 * If it is not an integer, the field name does not match this one.
						 */
						if (!regExLevel.exactMatch(checkingSuffix))
						{
							continue;
						}
					}
					/*
					 * Make sure that the data value is for the specific object ID.
					 */
					else if (isDataValue)
					{
						if (this->metaData() == nullptr)
						{
							continue;
						}

						const int row = j + 1;

						if (!this->metaData()->hasValue(row, "useSpecific"))
						{
							continue;
						}

						const QStringList useSpecific = this->metaData()->value(row, "useSpecific").split(',');

						/*
						 * If the standard object ID is not part of the use specific column for the field, the data value is not for this object.
						 */
						if (!useSpecific.contains(originalObjectId))
						{
							continue;
						}
					}

					fieldIndex = j;

					break;
				}
			}

			if (fieldIndex == -1)
			{
				errors.push_back(QString("Missing field by name \"" + fieldNameCut + "\" with full field name \"" + fieldName + "\". Hint: \"" + hint + "\". Checked possibilities with insufficient conditions \"" + possibilities.join(", ") + "\""));

				continue;
			}
		}

		const QString fieldId = this->metaData()->value(fieldIndex + 1, "ID").toLower();

		// TODO check other object data as well
		if (!this->hasFieldValue(originalObjectId, customObjectId, fieldId, fieldLevel))
		{
			/*
			 * List ALL modifications of the object to make suggestions.
			 */
			QStringList modificationsStringList;

			const ObjectId objectId(originalObjectId, customObjectId);
			Objects::const_iterator iterator = this->m_objects.find(objectId);

			if (iterator != this->m_objects.end())
			{
				const Modifications &modifications = iterator.value();

				for (Modifications::const_iterator modificationIterator = modifications.begin(); modificationIterator != modifications.end(); ++modificationIterator)
				{
					const map::CustomObjects::Modification &modification = modificationIterator.value();

					modificationsStringList.push_back(tr("%1 - Level %2 - Value %3").arg(map::idToString(modification.valueId()).c_str()).arg(modification.level()).arg(valueToString(modification.value())));
				}
			}

			errors.push_back(QString("Missing field \"" + reference + "\" from object " + originalObjectId + ":" + customObjectId + " using field ID \"" + fieldId + "\" and field level " + QString::number(fieldLevel) + " Existing modifications: " + modificationsStringList.join(", ")));
		}
	}

	QStringList modifiedErrors;

	for (const QString &error : errors)
	{
		modifiedErrors << (error + " - \"" + tooltip + "\"");
	}

	return modifiedErrors;
}

void ObjectData::widgetize(const QUrl &url)
{
	//
}

QString ObjectData::defaultFieldValue(const QString &objectId, const QString &fieldId, int level, bool &exists) const
{
	const MetaDataList metaDataList = this->resolveDefaultField(objectId, fieldId, level);

	if (!metaDataList.isEmpty())
	{
		const QString field = this->metaData()->value(fieldId, "field");
		// If a level is specified the field name is used + the level
		const QString levelField = field + QString::number(level);
		/*
		 * The section is used if no object ID is given (for example for Misc Data) and is the .txt file's section which contains the field value.
		 * It does not exist in Warcraft III: Reign of Chaos but in Frozen Throne.
		 */
		const QString section = this->metaData()->hasValue(fieldId, "section") ? this->metaData()->value(fieldId, "section") : "";
		const QString rowKey = section.isEmpty() ? objectId : section;
		bool hasValue = false;
		QString value;

		foreach (MetaData *metaData, metaDataList)
		{
			// This might happen when the meta data file has not been loaded properly.
			if (metaData == nullptr)
			{
				qDebug() << "Warning: nullptr meta data.";

				continue;
			}

			if (metaData->hasValue(rowKey, field))
			{
				value = metaData->value(rowKey, field);
				hasValue = true;

				break;
			}

			// Test level as well. The level entries do work for the SLK files with fields like "adur1" but for TXT fields like "Ubertip"/"aub1" it is always "Ubertip" but the level values are separated by , characerters. The difference between these two types of entries is that for the "Ubertip" entry the index is set to 0 while for the "adur" entry it is set to -1.
			else if (metaData->hasValue(rowKey, levelField))
			{
				value = metaData->value(rowKey, levelField);
				hasValue = true;

				break;
			}
		}

		if (hasValue)
		{
			/*
			 * The index is used for button positions for example when multiple fields share the same field name such as "Buttonpos".
			 *
			 * It is also used for multiple level values. Each index is for one level value.
			 */
			const QString index = this->metaData()->value(fieldId, "index");
			bool ok = true;
			int indexValue = index.toInt(&ok);

			if (ok)
			{
				if (indexValue != -1)
				{
					/*
					 * It seems that the values for the single levels are sperated by the indices.
					 * Therefore take the specific level value.
					 */
					indexValue += level;
					exists = true;
					value = MetaData::valueByIndex(value.trimmed(), indexValue);
				}

				exists = true;

				/*
				 * WESTRING_ entries reference a string in the file "UI/WorldEditStrings.txt.
				 * The default section is "[WorldEditStrings]".
				 *
				 * These entries are used for strings which has to be translated since the file "UI/WorldEditStrings.txt" is part of the MPQ archive War3xLocal.mpq.
				 */
				if (value.startsWith("WESTRING"))
				{
					return this->source()->sharedData()->tr(value);
				}

				return value;
			}
			else
			{
				qDebug() << "Invalid index value" << index << "for object" << objectId << "with field" << fieldId;
			}
		}
	}

	qDebug() << "Default field value not found:" << objectId << ":" << fieldId;

	return "";
}


}

}
