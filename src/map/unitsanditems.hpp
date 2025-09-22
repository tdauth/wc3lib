/***************************************************************************
 *   Copyright (C) 2025 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_UNITSANDITEMS_HPP
#define WC3LIB_MAP_UNITSANDITEMS_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "unitanditem.hpp"

namespace wc3lib
{

namespace map
{

class UnitsAndItems : public FileFormat
{
	public:
		typedef boost::ptr_vector<UnitAndItem> UnitAndItemContainer;

		virtual std::streamsize read(InputStream& istream) override;
		virtual std::streamsize write(OutputStream& ostream) const override;

		virtual const byte* fileTextId() const override;
		virtual const byte* fileName() const override;
		virtual uint32 latestFileVersion() const override;
		virtual uint32 version() const override;

		uint32 subVersion() const;
		UnitAndItemContainer& unitsAndItems();
		const UnitAndItemContainer& unitsAndItems() const;

	protected:
		uint32 m_subVersion;
		UnitAndItemContainer m_unitsAndItems;
};

inline const byte* UnitsAndItems::fileTextId() const
{
	return "W3do";
}

inline const byte* UnitsAndItems::fileName() const
{
	return "war3mapUnits.doo";
}

inline uint32 UnitsAndItems::latestFileVersion() const
{
	return 8;
}

inline uint32 UnitsAndItems::version() const
{
	return this->m_version;
}

inline uint32 UnitsAndItems::subVersion() const
{
	return this->m_subVersion;
}

inline UnitsAndItems::UnitAndItemContainer& UnitsAndItems::unitsAndItems()
{
	return this->m_unitsAndItems;
}

inline const UnitsAndItems::UnitAndItemContainer& UnitsAndItems::unitsAndItems() const
{
	return this->m_unitsAndItems;
}

}

}

#endif
