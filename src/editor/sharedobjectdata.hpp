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

#ifndef WC3LIB_EDITOR_SHAREDOBJECTDATA_HPP
#define WC3LIB_EDITOR_SHAREDOBJECTDATA_HPP

#include <QWidget>

#include <boost/scoped_ptr.hpp>

#include "objectdata.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

/**
 * \brief Stores and resolves object data of all different types such as units, abilities, items etc.
 *
 * The whole object data is stored all together since different object data accesses other object data.
 * For example a unit can have abilities. Therefore it needs to access the ability data as well.
 */
class SharedObjectData
{
	public:
		typedef boost::scoped_ptr<ObjectData> ObjectDataPtr;
		typedef boost::scoped_ptr<MetaData> MetaDataPtr;

		SharedObjectData(MpqPriorityList *source);
		virtual ~SharedObjectData();

		virtual void load(QWidget *widget);

		MpqPriorityList* source() const;

		const ObjectDataPtr& unitData() const;
		const ObjectDataPtr& itemData() const;
		const ObjectDataPtr& destructableData() const;
		const ObjectDataPtr& doodadData() const;
		const ObjectDataPtr& abilityData() const;
		const ObjectDataPtr& buffData() const;
		const ObjectDataPtr& upgradeData() const;
		const ObjectDataPtr& waterData() const;
		const ObjectDataPtr& weatherData() const;
		const ObjectDataPtr& miscData() const;
		const ObjectDataPtr& skinData() const;
		const MetaDataPtr& unitEditorData() const;

		typedef QList<ObjectData*> ObjectDataList;

		/**
		 * Depending on the type \p fieldType it returns the corresponding object data.
		 */
		ObjectDataList resolveByFieldType(const QString &fieldType) const;

	private:
		MpqPriorityList *m_source;
		ObjectDataPtr m_unitData;
		ObjectDataPtr m_itemData;
		ObjectDataPtr m_destructableData;
		ObjectDataPtr m_doodadData;
		ObjectDataPtr m_abilityData;
		ObjectDataPtr m_buffData;
		ObjectDataPtr m_upgradeData;
		ObjectDataPtr m_waterData;
		ObjectDataPtr m_weatherData;
		ObjectDataPtr m_miscData;
		ObjectDataPtr m_skinData;
		MetaDataPtr m_unitEditorData;
};

inline MpqPriorityList* SharedObjectData::source() const
{
	return this->m_source;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::unitData() const
{
	return this->m_unitData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::itemData() const
{
	return this->m_itemData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::destructableData() const
{
	return this->m_destructableData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::doodadData() const
{
	return this->m_doodadData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::abilityData() const
{
	return this->m_abilityData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::buffData() const
{
	return this->m_buffData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::upgradeData() const
{
	return this->m_upgradeData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::waterData() const
{
	return this->m_waterData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::weatherData() const
{
	return this->m_weatherData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::miscData() const
{
	return this->m_miscData;
}

inline const SharedObjectData::ObjectDataPtr& SharedObjectData::skinData() const
{
	return this->m_skinData;
}

inline const SharedObjectData::MetaDataPtr& SharedObjectData::unitEditorData() const
{
	return this->m_unitEditorData;
}

}

}

#endif // WC3LIB_EDITOR_SHAREDOBJECTDATA_HPP
