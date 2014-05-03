/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_TRIGGERS_HPP
#define WC3LIB_MAP_TRIGGERS_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "triggercategory.hpp"
#include "variable.hpp"
#include "trigger.hpp"
#include "triggerdata.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \note You have to use a corresponding \ref TriggerData instance to fill all contained triggers. This is necessary because trigger parameter data is associated with data of \ref TriggerData.
 * \todo Add derived class TriggersX.
 * \sa TriggersX
 */
class Triggers : public FileFormat
{
	public:
		typedef boost::ptr_vector<TriggerCategory> Categories;
		typedef boost::ptr_vector<Variable> Variables;
		typedef boost::ptr_vector<Trigger> TriggerEntries;

		Triggers();

		virtual std::streamsize read(InputStream &istream) throw (Exception)
		{
			throw Exception(_("Not usable."));
		}

		virtual std::streamsize read(InputStream &istream, const TriggerData &triggerData) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual const byte* fileTextId() const;
		virtual const byte* fileName() const;
		virtual uint32 latestFileVersion() const;

		int32 unknown0() const;
		Categories& categories();
		const Categories& categories() const;
		Variables& variables();
		const Variables& variables() const;
		TriggerEntries& triggers();
		const TriggerEntries& triggers() const;

	protected:
		int32 m_unknown0;
		Categories m_categories;
		Variables m_variables;
		TriggerEntries m_triggers;
};

inline const byte* Triggers::fileTextId() const
{
	return "WTG!";
}

inline const byte* Triggers::fileName() const
{
	return "war3map.wtg";
}

inline uint32 Triggers::latestFileVersion() const
{
	return 4;
}

inline int32 Triggers::unknown0() const
{
	return m_unknown0;
}

inline Triggers::Categories& Triggers::categories()
{
	return m_categories;
}

inline const Triggers::Categories& Triggers::categories() const
{
	return m_categories;
}

inline Triggers::Variables& Triggers::variables()
{
	return m_variables;
}

inline const Triggers::Variables& Triggers::variables() const
{
	return m_variables;
}

inline Triggers::TriggerEntries& Triggers::triggers()
{
	return m_triggers;
}

inline const Triggers::TriggerEntries& Triggers::triggers() const
{
	return m_triggers;
}

}

}

#endif
