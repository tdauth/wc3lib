/***************************************************************************
 *   Copyright (C) 2013 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_TRIGGERSTRINGS_HPP
#define WC3LIB_MAP_TRIGGERSTRINGS_HPP

#include <boost/ptr_container/ptr_map.hpp>

#include "platform.hpp"
#include "txt.hpp"
#include "triggerfunction.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \brief Stores all strings corresponding to trigger data from \ref TriggerData.
 *
 * "UI/TriggerStrings.txt" contains the corresponding strings for all trigger function types which are displayed in trigger editor
 * to help users understanding the meaning of function parameters and functions at all.
 *
 * \ingroup triggers
 */
class TriggerStrings : public FileFormat
{
	public:
		TriggerStrings();

		virtual const byte* fileName() const;
		virtual const byte* fileTextId() const;
		virtual uint32 latestFileVersion() const;

		class Entry
		{
			public:
				Entry();

				void setCode(const string &code);
				const string& code() const;
				void setName(const string &name);
				const string& name() const;
				void setLayout(const string &layout);
				const string& layout() const;
				void setHint(const string &hint);
				const string& hint() const;

			private:
				string m_code;
				string m_name;
				string m_layout;
				string m_hint;
		};

		typedef boost::ptr_map<string, Entry> Entries;

		virtual std::streamsize read(InputStream& istream) throw (class Exception);
		virtual std::streamsize write(OutputStream& ostream) const throw (class Exception);

		Entries& events();
		const Entries& events() const;
		Entries& conditions();
		const Entries& conditions() const;
		Entries& actions();
		const Entries& actions() const;
		Entries& calls();
		const Entries& calls() const;

		/**
		 * \return Returns the entries only for \p type.
		 *
		 * \throws Exception Throws an exception if \p type is invalid.
		 *
		 * @{
		 */
		Entries& entries(TriggerFunction::Type type);
		const Entries& entries(TriggerFunction::Type type) const;
		/**
		 * @}
		 */

	private:
		void readFunction(const Txt::Entry &ref, Entries &functions);

		Entries m_events;
		Entries m_conditions;
		Entries m_actions;
		Entries m_calls;
};

inline const byte* TriggerStrings::fileName() const
{
	return "TriggerStrings.txt";
}

inline const byte* TriggerStrings::fileTextId() const
{
	return "";
}

inline uint32 TriggerStrings::latestFileVersion() const
{
	return 0;
}

inline void TriggerStrings::Entry::setCode(const string& code)
{
	this->m_code = code;
}


inline const string& TriggerStrings::Entry::code() const
{
	return this->m_code;
}

inline void TriggerStrings::Entry::setName(const string& name)
{
	this->m_name = name;
}

inline const string& TriggerStrings::Entry::name() const
{
	return this->m_name;
}

inline void TriggerStrings::Entry::setLayout(const string& layout)
{
	this->m_layout = layout;
}

inline const string& TriggerStrings::Entry::layout() const
{
	return this->m_layout;
}

inline void TriggerStrings::Entry::setHint(const string& hint)
{
	this->m_hint = hint;
}

inline const string& TriggerStrings::Entry::hint() const
{
	return this->m_hint;
}

inline TriggerStrings::Entries& TriggerStrings::events()
{
	return this->m_events;
}

inline const TriggerStrings::Entries& TriggerStrings::events() const
{
	return this->m_events;
}

inline TriggerStrings::Entries& TriggerStrings::conditions()
{
	return this->m_conditions;
}

inline const TriggerStrings::Entries& TriggerStrings::conditions() const
{
	return this->m_conditions;
}

inline TriggerStrings::Entries& TriggerStrings::actions()
{
	return this->m_actions;
}

inline const TriggerStrings::Entries& TriggerStrings::actions() const
{
	return this->m_actions;
}

inline TriggerStrings::Entries& TriggerStrings::calls()
{
	return this->m_calls;
}

inline const TriggerStrings::Entries& TriggerStrings::calls() const
{
	return this->m_calls;
}

}

}

#endif
