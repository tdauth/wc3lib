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

#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class MetaData;
class MpqPriorityList;

/**
 * \brief Unifies all meta, default and custom data of objects such as units, items, abilities etc.
 *
 * It provides a unified access for standard and custom object values.
 * You can use \ref fieldValue() to get the value of a field of an object.
 * Custom objects are stored as modifications.
 * Modifications can be added using \ref modifyField() and be reset using \ref resetField().
 *
 * \ingroup objectdata
 */
class ObjectData : public QObject
{
	Q_OBJECT

	signals:
		void modificationReset(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
		void objectCreation(const QString &originalObjectId, const QString &customObjectId);
		void objectReset(const QString &originalObjectId, const QString &customObjectId);
		void objectRemoval(const QString &originalObjectId, const QString &customObjectId);
		void fieldModification(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);

	public:
		/**
		 * \brief Hash table which stores modificiations by using the field ID as hash (such as "unam").
		 *
		 * It stores the modifications in the format of Frozen Throne which can be converted up to Reign of Chaos if required for the export.
		 */
		typedef QHash<QString, map::CustomObjects::Modification> Modifications;
		/**
		 * \brief Stores the two object IDs. The original as well as the custom.
		 */
		typedef QPair<QString, QString> ObjectId;
		/**
		 * \brief Hash table which stores objects by using the object ID pair as hash.
		 */
		typedef QHash<ObjectId, Modifications> Objects;

		ObjectData(MpqPriorityList *source, QObject *parent = 0);

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

		/**
		 * Some types allow multiple selections of values by checking multiple check boxes.
		 * \return Returns true if the field with ID \p fieldId allows multiple selections.
		 */
		bool fieldTypeAllowsMultipleSelections(const QString &fieldId) const;

		/**
		 * \return Returns true if the field value of the field with ID \p fieldId of the standard object with the ID \p objectId does exist.
		 * \sa defaultValue()
		 */
		virtual bool hasDefaultFieldValue(const QString &objectId, const QString &fieldId) const = 0;

		/**
		 * \brief Element function which is used for resolving any object data value stored in one of the many SLK files.
		 *
		 * \note This does not resolve meta data but actual object data (default data for existing objects).
		 *
		 * \sa hasDefaultFieldValue()
		 */
		virtual QString defaultFieldValue(const QString &objectId, const QString &fieldId) const = 0;

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

		typedef QPair<QString, QString> ObjectTabEntry;
		typedef QLinkedList<ObjectTabEntry> ObjectTabEntries;

		/**
		 * Returns all possible entries for \p fieldType.
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

		typedef QList<map::Slk> Slks;

		/**
		 * SLK files are used in Warcraft III to define standard objects.
		 * Some object types such as weather effects do not support any other type of object.
		 *
		 * \return Returns the custom units of the current tab.
		 *
		 * \sa customObjects() hasCustomUnits() hasCustomObjects()
		 *
		 * \throws Exception Throws an exception if SLKs are not supported.
		 */
		virtual Slks slks() const = 0;

		virtual map::CustomObjects::Object customObject(const QString &originalObjectId, const QString &customObjectId) const;

		QString objectId(int value) const;
		/**
		 * Each custom object needs a unique ID that is automatically calculated.
		 * \throws Exception Throws an exception if the next custom object ID is out of range or invalid.
		 */
		virtual QString nextCustomObjectId() const;

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
		 * \return Returns true if a list of SLK files is available.
		 */
		virtual bool hasSlks() const = 0;

		map::Value value(const QString &fieldId, const QString &value) const;

		/**
		 * Modifies a single field with ID \p fieldId (taken from meta data file) of object with IDS \p originalObjectId and \p customObjectId and sets the field's modification to \p modification.
		 * For example this happens whenever a user enters another or even the same value which overwrites the default one of the object.
		 * All modified fields are exported when the user exports object data.
		 */
		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const map::CustomObjects::Modification &modification);
		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const QString &value);
		/**
		 * Resets the field's \p fieldId modification for object with IDs \p originalObjectId and \p customObjectId.
		 */
		void resetField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
		bool isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;

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
		bool fieldModificiation(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, map::CustomObjects::Modification &modification) const;
		bool hasFieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;
		QString fieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;
		QString fieldReadableValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;

		/**
		 * \return Returns true if the field \p fieldId is not shown in the table view for the corresponding object.
		 */
		virtual bool hideField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const = 0;

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
		 * \return Returns the field ID of the readable name for objects.
		 */
		virtual QString objectNameFieldId() const = 0;

		const Objects& objects() const;

	protected:
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

}

}

#endif // WC3LIB_EDITOR_OBJECTDATA_HPP
