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

#ifndef WC3LIB_EDITOR_UNITMETADATA_HPP
#define WC3LIB_EDITOR_UNITMETADATA_HPP

#include <QWidget>

#include "objectmetadata.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class MetaData;

/**
 * \brief Stores all data for standard units and unit meta data.
 */
class UnitMetaData : public ObjectMetaData
{
	public:
		UnitMetaData(MpqPriorityList *source);

		MpqPriorityList* source() const;

		virtual void load(QWidget *widget) override;

		/**
		 * \brief Element function which is used for resolving any object data value stored in one of the many SLK files.
		 *
		 * \note This does not resolve meta data but actual object data (default data for existing objects).
		 */
		virtual QString getDataValue(const QString &objectId, const QString &fieldId) const override;

		virtual MetaData* metaData() const override;

		MetaData* unitMetaData() const;
		MetaData* unitData() const;
		MetaData* unitUi() const;
		MetaData* unitBalance() const;
		MetaData* unitWeapons() const;
		MetaData* unitAbilities() const;
		MetaData* humanUnitStrings() const;
		MetaData* humanUnitFunc() const;
		MetaData* orcUnitStrings() const;
		MetaData* orcUnitFunc() const;
		MetaData* undeadUnitStrings() const;
		MetaData* undeadUnitFunc() const;
		MetaData* nightElfUnitStrings() const;
		MetaData* nightElfUnitFunc() const;
	private:
		MpqPriorityList *m_source;
		MetaData *m_unitMetaData;
		MetaData *m_unitData;
		MetaData *m_unitUi;
		MetaData *m_unitBalance;
		MetaData *m_unitWeapons;
		MetaData *m_unitAbilities;
		MetaData *m_humanUnitStrings;
		MetaData *m_humanUnitFunc;
		MetaData *m_orcUnitStrings;
		MetaData *m_orcUnitFunc;
		MetaData *m_undeadUnitStrings;
		MetaData *m_undeadUnitFunc;
		MetaData *m_nightElfUnitStrings;
		MetaData *m_nightElfUnitFunc;
};

inline MpqPriorityList* UnitMetaData::source() const
{
	return this->m_source;
}

inline MetaData* UnitMetaData::metaData() const
{
	return this->unitMetaData();
}

inline MetaData* UnitMetaData::unitMetaData() const
{
	return this->m_unitMetaData;
}

inline MetaData* UnitMetaData::unitData() const
{
	return this->m_unitData;
}

inline MetaData* UnitMetaData::unitUi() const
{
	return this->m_unitUi;
}

inline MetaData* UnitMetaData::unitBalance() const
{
	return this->m_unitBalance;
}

inline MetaData* UnitMetaData::unitWeapons() const
{
	return this->m_unitWeapons;
}

inline MetaData* UnitMetaData::unitAbilities() const
{
	return this->m_unitAbilities;
}

inline MetaData* UnitMetaData::humanUnitStrings() const
{
	return this->m_humanUnitStrings;
}

inline MetaData* UnitMetaData::humanUnitFunc() const
{
	return this->m_humanUnitFunc;
}

inline MetaData* UnitMetaData::orcUnitStrings() const
{
	return this->m_orcUnitStrings;
}

inline MetaData* UnitMetaData::orcUnitFunc() const
{
	return this->m_orcUnitFunc;
}

inline MetaData* UnitMetaData::undeadUnitStrings() const
{
	return this->m_undeadUnitStrings;
}

inline MetaData* UnitMetaData::undeadUnitFunc() const
{
	return this->m_undeadUnitFunc;
}

inline MetaData* UnitMetaData::nightElfUnitStrings() const
{
	return this->m_nightElfUnitStrings;
}

inline MetaData* UnitMetaData::nightElfUnitFunc() const
{
	return this->m_nightElfUnitFunc;
}

}

}

#endif // WC3LIB_EDITOR_UNITMETADATA_HPP
