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

#ifndef WC3LIB_EDITOR_OBJECTMETADATA_HPP
#define WC3LIB_EDITOR_OBJECTMETADATA_HPP

#include <QString>
#include <QWidget>

#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class MetaData;

class ObjectMetaData
{
	public:
		/**
		 * \brief Element function which is used for resolving any object data value stored in one of the many SLK files.
		 *
		 * \note This does not resolve meta data but actual object data (default data for existing objects).
		 */
		virtual QString getDataValue(const QString &objectId, const QString &fieldId) const = 0;

		/**
		 * \return Returns the actual meta data which contains all raw data names of the object fields.
		 */
		virtual MetaData* metaData() const = 0;

		/**
		 * \return Returns the TXT file which contains data for the Object Editor GUI such as "UI/UnitEditorData.txt".
		 */
		virtual MetaData* objectTabData() const = 0;

		virtual void load(QWidget *widget) = 0;

		/**
		 * Checks if the meta data from Reign of Chaos is loaded.
		 * This helps you to decide if the object data can be imported or exported as Reign of Chaos unit data or not.
		 * It checks for fields using Reign of Chaos types like "unitList" or "itemList" etc. which are not used in Frozen Throne
		 * anymore.
		 */
		bool isReignOfChaos() const;

		/**
		 * Returns the corresponding value type for a field ID.
		 * This can be useful if you want to create a \ref wc3lib::map::Value object from a \ref ObjectTableWidgetPair.
		 * The type is stored in the meta data file (\ref metaData()) in the column "type".
		 * \throw Exception Throws an exception if the field type is not supported.
		 */
		map::Value::Type fieldType(const QString &fieldId) const;
};

}

}

#endif // OBJECTMETADATA_H
