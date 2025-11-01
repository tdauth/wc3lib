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

#ifndef WC3LIB_EDITOR_OBJECTDATA_HPP
#define WC3LIB_EDITOR_OBJECTDATA_HPP

#include <QString>
#include <QWidget>
#include <QHash>
#include <QPair>
#include <QUrl>

#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

class MetaData;
class MpqPriorityList;

/**
 * \brief Unifies all meta, default and custom data of objects such as units, items, abilities etc. in one class and allows fast access by hashing.
 *
 * It provides a unified access for standard and custom object values.
 * You can use \ref fieldValue() to get the value of a field of an object.
 * Custom objects are stored as modifications.
 * Modifications can be added using \ref modifyField() and be reset using \ref resetField().
 *
 * Call \ref load() to load the standard objects and the meta data files.
 *
 * This class allows you registering slots at the modifications of objects.
 * For instance, signal \ref fieldModification() is emitted whenever a modification is done.
 * If you're creating any GUI you can use the signal to update your data.
 *
 * Importing and exporting is supported as well. Use \ref importCustomUnits() or \ref importCustomObjects() to import any custom data and
 * \ref customUnits(), \ref customObjects() and \ref metaDataList() for exporting it.
 *
 * \ingroup objectdata
 */
class ObjectData : public QObject
{
	Q_OBJECT

	signals:
		/**
		 * These signals can be used to register slots which react to modifications of the object data.
		 * This can be useful if you provide a GUI such as \ref ObjectEditor which has to show the current modifications.
		 * @{
		 */
		void modificationReset(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level);
		void objectCreation(const QString &originalObjectId, const QString &customObjectId);
		void objectReset(const QString &originalObjectId, const QString &customObjectId);
		void objectRemoval(const QString &originalObjectId, const QString &customObjectId);
		void fieldModification(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level);
		/**
		 * @}
		 */

	public:
		/**
		 * All text sources are hold on heap as long as the object data exists.
		 */
		typedef QScopedPointer<MetaData> MetaDataPtr;

		/**
		 * \brief Hash table key for field values which is a pair of the field ID and the level.
		 */
		struct FieldId : public QPair<QString, int>
		{
			public:
				FieldId();
				FieldId(const QString &fieldId, int level);
				/**
				 * \return Returns the ID of the field.
				 */
				QString fieldId() const;
				/**
				 * \return Returns the level of the field.
				 */
				int level() const;
		};

		/**
		 * \brief Hash table which stores modificiations by using the field ID as hash (such as "unam").
		 *
		 * It stores the modifications in the format of Frozen Throne which can be converted up to Reign of Chaos if required for the export.
		 */
		typedef QHash<FieldId, map::CustomObjects::Modification> Modifications;
		/**
		 * \brief Stores the two object IDs. The original as well as the custom.
		 *
		 * The first value of the pair is the original object ID.
		 * The second value is the custom object ID.
		 * If the object is an original object the second value stays empty.
		 */
		struct ObjectId : public QPair<QString, QString>
		{
			public:
				ObjectId(const QString &originalObjectId, const QString &customObjectId);
				/**
				 * \return Returns the original object ID of the object ID.
				 */
				QString originalObjectId() const;
				/**
				 * \return Returns the custom object ID of the object ID.
				 */
				QString customObjectId() const;
		};
		/**
		 * \brief Hash table which stores objects by using the object ID pair as hash.
		 * This allows fast access to all modifications of objects using the object's ID.
		 */
		typedef QHash<ObjectId, Modifications> Objects;

		/**
		 * Creates new object data which is empty.
		 * \param source The source which is used for all files.
		 * \param parent The object's parent.
		 */
		ObjectData(MpqPriorityList *source, QObject *parent = 0);

		/**
		 * \return Returns the corresponding source of the object data which is required to load meta data/object data files.
		 */
		MpqPriorityList* source() const;

		/**
		 * Returns the corresponding value type for a field ID.
		 * This can be useful if you want to create a \ref wc3lib::map::Value.
		 * The type is stored in the meta data file (\ref metaData()) in the column "type".
		 *
		 * \throw Exception Throws an exception if the field type is not supported.
		 */
		map::Value::Type fieldType(const QString &fieldId) const;

		/**
		 * Some field types such as "unitList" allow multiple values separated by a "," character.
		 *
		 * \return Returns true if field type \p fieldType allows a list of values.
		 */
		bool fieldTypeIsList(const QString &fieldType) const;
		bool fieldTypeIsLiteralList(const QString &fieldType) const;
		QString fieldLiteralTypeFromListType(const QString &fieldLiteralListType) const;
		bool fieldTypeIsObjectList(const QString &fieldType) const;

		/**
		 * Some types allow multiple selections of values by checking multiple check boxes.
		 * \return Returns true if the field with type \p fieldType allows multiple selections.
		 */
		bool fieldTypeAllowsMultipleSelections(const QString &fieldType) const;

		/**
		 * \brief A list of meta data pointers.
		 */
		typedef QList<MetaData*> MetaDataList;
		/**
		 * Overwrite this element function to resolve meta data files by the field ID \p fieldId.
		 *
		 * \return Returns a list of meta data which might contain the default value of field \p fieldId.
		 */
		virtual MetaDataList resolveDefaultField(const QString &objectId, const QString &fieldId, int level = 0) const = 0;

		/**
		 * \return Returns true if the field value of the field with ID \p fieldId of the standard object with the ID \p objectId does exist.
		 * \sa defaultValue()
		 */
		virtual bool hasDefaultFieldValue(const QString &objectId, const QString &fieldId, int level = 0) const;

		/**
		 * \brief Element function which is used for resolving any object data value stored in one of the many SLK files.
		 *
		 * \note This does not resolve meta data but actual object data (default data for existing objects).
		 *
		 * \sa hasDefaultFieldValue()
		 */
		virtual QString defaultFieldValue(const QString &objectId, const QString &fieldId, int level = 0) const;

		/**
		 * Some fields are repeated when the levels of the object is changed, so you can specify a field value for each level.
		 * \return Returns true if the given field is repeated and therefore specified for each level. Otherwise it returns false.
		 */
		virtual bool repeateField(const QString &fieldId) const;

		/**
		 * The meta data specifies which fields do exist for objects and specify the IDs and types of those fields.
		 * It is required to resolve any information about a field.
		 *
		 * \return Returns the actual meta data which contains all raw data names of the object fields.
		 */
		virtual MetaData* metaData() const = 0;

		/**
		 * The object tab data contains information about all possible values for specific fields.
		 * For example the field type "moveType" allows only one movement type from a list of possible movement types.
		 * These possible values and their names are stored in the object tab data file.
		 *
		 * \return Returns the TXT file which contains data for the Object Editor GUI.
		 *
		 * \sa objectTabEntries()
		 */
		virtual MetaData* objectTabData() const = 0;

		/**
		 * Each object data entry has the name of the type and its readable form.
		 * For example, "foot" and "WESTRING_UE_MOVETYPE_FOOT" for the section "moveType".
		 */
		typedef QPair<QString, QString> ObjectTabEntry;
		/**
		 * \brief A list of \ref ObjectTabEntry instances which occur in sections of file "UI/UnitEditorData.txt".
		 */
		typedef std::list<ObjectTabEntry> ObjectTabEntries;

		/**
		 * Returns all possible entries for \p fieldType.
		 * Field types are specified in the meta data file (\ref metaData()) such as "moveType" or "string".
		 *
		 * \sa objectTabData()
		 */
		ObjectTabEntries objectTabEntries(const QString &fieldType) const;

		/**
		 * Loads all meta data and data.
		 * The data cannot be used before it has been loaded successfully.
		 *
		 * \throws Exception Throws an exception if loading fails.
		 */
		virtual void load(QWidget *widget) = 0;
		/**
		 * Loads object data changing the cursor of \p widget to "busy" while loading the data.
		 *
		 * \return Returns true if the data has been loaded successfully. Otherwise it returns false and pops up a message box.
		 */
		bool loadOnRequest(QWidget *widget);

		virtual map::CustomObjects::Modification unitToObjectModification(const map::CustomUnits::Modification &modification) const;

		/**
		 * Custom Units are used in Warcraft III: Reign of Chaos.
		 * So this should throw an exception in other tabs than unit and items since they need additional data
		 * which can be gotten from \ref customObjects().
		 *
		 * \return Returns the custom units of the current tab.
		 *
		 * \sa customObjects() hasCustomUnits() hasCustomObjects()
		 *
		 * \throws Exception Throws an exception if custom units cannot store the object data.
		 */
		virtual map::CustomUnits customUnits() const;
		/**
		 * \return Returns the custom objects of the tab.
		 *
		 * \sa customUnits(), hasCustomObjects(), hasCustomUnits()
		 */
		virtual map::CustomObjects customObjects() const;

		/**
		 * \return Returns true if a list of \ref MetaData files is available.
		 */
		virtual bool hasMetaDataList() const = 0;

		/**
		 * \ref MetaData files are used in Warcraft III to define standard objects.
		 * Some object types such as weather effects do not support any other type of object.
		 *
		 * \return Returns the list of meta data files for the custom objects.
		 *
		 * \sa hasMetaDataList() customObjects() hasCustomUnits() hasCustomObjects()
		 *
		 * \throws Exception Throws an exception if Meta Data files are not supported.
		 */
		virtual MetaDataList metaDataList() const = 0;

		/**
		 * Creates an object for serialization of Warcraft III: The Frozen Throne.
		 * \param originalObjectId The original object ID of the created object.
		 */
		virtual map::CustomObjects::Object customObject(const QString &originalObjectId, const QString &customObjectId) const;

		/**
		 * Converts raw data ID \p value to a string value.
		 * \return Returns the raw data ID value as string.
		 */
		QString objectId(int value) const;
		/**
		 * Each custom object needs a unique ID that is automatically calculated.
		 * \param originalObjectId The base ID of the original ID the custom object is based on.
		 * \throws Exception Throws an exception if the next custom object ID is out of range or invalid.
		 */
		virtual QString nextCustomObjectId(const QString &originalObjectId) const;

		/**
		 * \return Returns the type of custom objects which is stored in this object data.
		 * If \ref hasCustomObjects() returns false anyway because this type of object data is not supported by custom objects, the result can be ignored.
		 */
		virtual map::CustomObjects::Type type() const = 0;

		/**
		 * Should return if custom units are available.
		 * Usually only either custom units or custom objects are available depending on which meta data is loaded or if it is Reign of Chaos or Frozen Throne.
		 *
		 * \sa hasCustomObjects().
		 */
		virtual bool hasCustomUnits() const = 0;
		/**
		 * Should return if custom objects are available.
		 *
		 * \sa hasCustomUnits()
		 */
		virtual bool hasCustomObjects() const = 0;
		/**
		 * The file extension without any dot which is used when the custom objects of only one type are exported (no collection).
		 */
		virtual QString customObjectsExtension() const = 0;

		map::Value value(const QString &fieldId, const QString &value) const;

		/**
		 * Creates object with IDs \p originalObjectId and \p customObjectId without any modifications.
		 */
		Objects::iterator createObject(const QString &originalObjectId, const QString &customObjectId);

		/**
		 * Modifies a single field with ID \p fieldId (taken from meta data file) of object with IDS \p originalObjectId and \p customObjectId and sets the field's modification to \p modification.
		 * For example this happens whenever a user enters another or even the same value which overwrites the default one of the object.
		 * All modified fields are exported when the user exports object data.
		 */
		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const map::CustomObjects::Modification &modification);
		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const QString &value, int level = 0);
		/**
		 * Resets the field's \p fieldId modification for object with IDs \p originalObjectId and \p customObjectId.
		 */
		void resetField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0);
		bool isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const;

		void resetObject(const QString &originalObjectId, const QString &customObjectId);
		/**
		 * Deletes an object if it is a custom object.
		 * Otherwise it only resets the object.
		 */
		void deleteObject(const QString &originalObjectId, const QString &customObjectId);
		/**
		 * \return Returns true if the object has at least one modification.
		 */
		bool isObjectModified(const QString &originalObjectId, const QString &customObjectId) const;
		/**
		 * Resets all modifications for standard and custom objects.
		 *
		 * \sa resetField()
		 */
		void clearModifications();
		/**
		 * Stores the modification of object with IDs \p originalObjectId and \p customObjectId for field with ID \p fieldId into \p modification.
		 * Returns whether there is a valid modification or not.
		 *
		 * \return Returns true if there is a modification for the object's field. Otherwise it returns false and the filled modification is invalid.
		 *
		 * \sa isFieldModified()
		 */
		bool fieldModificiation(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, map::CustomObjects::Modification &modification, int level = 0) const;
		/**
		 * \return Returns only true if the field value has been modified and does not use the default field value of the original object.
		 */
		bool hasFieldCustomValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const;
		bool hasFieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const;
		QString fieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const;
		QString fieldReadableValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const;

		/**
		 * This method is required to determine if an object belongs to this object data.
		 * \return Returns true if the original object with the ID \p originalObjectId exists. Otherwise it returns false.
		 */
		virtual bool hasOriginalObject(const QString &originalObjectId) const;

		/**
		 * \return Returns true if the field \p fieldId is not shown in the table view for the corresponding object.
		 */
		virtual bool hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, int level = 0) const = 0;

		/**
		 * Imports custom units \p units and replaces all existing custom units by immediately.
		 * It updates the modifications to the loaded custom units.
		 *
		 * This function supports only importing custom units from Warcraft III: Reign of Chaos.
		 *
		 * \sa importCustomObjects()
		 */
		void importCustomUnits(const map::CustomUnits &units);
		/**
		 * Imports custom objects \p objects.
		 *
		 * \sa importCustomUnits()
		 */
		void importCustomObjects(const map::CustomObjects &objects);

		/**
		 * The name of an object is required in multiple cases.
		 * For example when listing objects of a type (for field type "unitList") or when displaying them in any view.
		 *
		 * \return Returns the readable name of the object.
		 */
		virtual QString objectName(const QString &originalObjectId, const QString &customObjectId) const = 0;
		virtual QIcon objectIcon(const QString &originalObjectId, const QString &customObjectId, QWidget *window) const = 0;
		/**
		 * Objects such as abilities or technologies can have multiple levels.
		 * For each level some field values can be specified once. This method returns the number of specified levels of an object.
		 *
		 * \return Returns the number of levels of the specified object.
		 *
		 * \note Field modifications for level 1 or 2 start with the integer 1 and 2 and not with 0 but only when the field is repeated.
		 */
		virtual int objectLevels(const QString &originalObjectId, const QString &customObjectId) const = 0;

		const Objects& objects() const;

		/**
		 * Replaces all "TRIGSTRING_number" entries by the actual string entries of map \p w3m.
		 */
		void applyMapStrings(map::W3m &w3m);
		/**
		 * Replaces all "TRIGSTRING_number" entries by the actual entries of the strings file \p url.
		 * \param url A war3map.wts file from which all STRING entries are taken.
		 */
		void applyMapStrings(const QUrl &url);

		/**
		 * A list of object IDs (raw codes).
		 */
		typedef QList<QString> StandardObjecIds;

		/**
		 * \return Returns all standard object IDs.
		 */
		virtual StandardObjecIds standardObjectIds() const = 0;

		/**
		 * Removes all unnecessary field modifications of fields which are not shown anyway.
		 * \note This might remove modifications which you might want to use later or which are hidden but recognized by the game in some way.
		 *
		 * \return Returns the number of compressed modifications.
		 */
		virtual long long compress();

		/**
		 * Checks all "<A000,dur1>" like references in string values and checks if they do exist.
		 * If they do not exist it adds an error message to the result.
		 *
		 * If they do exist it checks if the ID is related to the object. Otherwise it adds an error to the result.
		 *
		 * \return Returns a list with all errors in human-readable form.
		 *
		 * \{
		 */
		virtual QStringList validateTooltipReferences();
		/**
		 * \param tooltip The tooltip text value which is checked.
		 * \param allFields A list of all valid fields taken from the corresponding meta data SLK file (column "field"). These fields can be refered.
		 * \param checkRecursive If this value is true and an object ID like 'A000' is not found in the current object data, all other object data from \ref SharedObjectData of \ref MpqPriorityList::sharedData of the object data's source \ref source() is searched for the ID and the tooltip is checked under the other object data if it belongs to one. This is important since you can use references to ability data in item tooltips etc.
		 *
		 * \return Returns a list with all errors in human-readable form.
		 */
		virtual QStringList validateTooltipReference(const QString &tooltip, const QStringList &allFields, bool checkRecursive = true);
		/**
		 * \}
		 */

		/**
		 * Widgetizing means that all possible object data is exported into SLK files which can only store abilities up to leve 4.
		 * The rest of the data will be exported as usual. Therefore a folder must be selected.
		 */
		virtual void widgetize(const QUrl &url);

		/**
		 * Searches for the custom object ID and returns the corresponding base object ID (the ID of the standard object which has been modified).
		 * Complexity: O(n)
		 * \todo Hash the standard object IDs by using only the custom object IDs to make this method faster.
		 * \return Returns the corresponding standard object ID. If the custom object ID is not found in the object data, an empty string is returned.
		 */
		QString baseOfCustomObjectId(const QString &customObjectId) const;

	protected:
		virtual QString defaultFieldValue(const QString &objectId, const QString &fieldId, int level, bool &exists) const;

		MpqPriorityList *m_source;
		/**
		 * Stores all modifications for all objects, standard objects as well as user defined ones.
		 */
		Objects m_objects;
};

inline MpqPriorityList* ObjectData::source() const
{
	return this->m_source;
}

inline const ObjectData::Objects& ObjectData::objects() const
{
	return this->m_objects;
}

inline ObjectData::FieldId::FieldId() : QPair<QString, int>("", 0)
{
}

inline ObjectData::FieldId::FieldId(const QString &fieldId, int level) : QPair<QString, int>(fieldId, level)
{
}

inline QString ObjectData::FieldId::fieldId() const
{
	return this->first;
}

inline int ObjectData::FieldId::level() const
{
	return this->second;
}

inline ObjectData::ObjectId::ObjectId(const QString &originalObjectId, const QString &customObjectId) : QPair<QString, QString>(originalObjectId, customObjectId)
{
}

inline QString ObjectData::ObjectId::originalObjectId() const
{
	return this->first;
}

inline QString ObjectData::ObjectId::customObjectId() const
{
	return this->second;
}

}

}

#endif // WC3LIB_EDITOR_OBJECTDATA_HPP
