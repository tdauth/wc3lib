/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Tamino Dauth <tamino@cdauth.eu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef WC3LIB_SLK_SLKCOLUMN_HPP
#define WC3LIB_SLK_SLKCOLUMN_HPP

#include "slkcolumns.hpp"

namespace wc3lib
{

namespace slk
{

class SlkColumn
{
	public:
		struct Predicate : std::binary_function<const SlkItemKey&, const SlkColumn*, bool>
		{
			bool operator()(const SlkItemKey &key, const SlkColumn *column)
			{
				return key.column() == column;
			}
		};
		
		class SlkColumns* columns() const
		{
			return m_columns;
		}
		
		class SlkItems items() const
		{
			return this->columns()->items(this);
		}
		
	protected:
		class SlkColumns *m_columns;
		Slk::IndexType m_column;
		
		map::string m_name;
		id m_id; // from "Units\UnitMetaData.slk"
		map::CustomUnits::Modification::Type m_type;
		/*
		union
		{
			int32 Integer;
			float32 Real; // float (single precision)
			float32 Unreal; // Unreal (0 <= val <= 1) float (single Precision)
			char8* String; // string (null terminated)
			bool Boolean;
			char8 Character;
			char8* UnitList;
			char8* ItemList;
			char8* RegenerationType;
			char8* AttackType;
			char8* WeaponType;
			char8* TargetType;
			char8* MoveType;
			char8* DefenseType;
			char8* PathingTexture;
			char8* UpgradeList;
			char8* StringList;
			char8* AbilityList;
			char8* HeroAbilityList;
			char8* MissileArt;
			char8* AttributeType;
			char8* AttackBits;
		} m_value;
		*/
};

}

}

#endif // SLKCOLUMN_HPP
