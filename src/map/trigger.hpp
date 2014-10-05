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

#ifndef WC3LIB_MAP_TRIGGER_HPP
#define WC3LIB_MAP_TRIGGER_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "triggerfunction.hpp"
#include "triggerdata.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Definition of a Warcraft III trigger. For Warcraft III: The Frozen Trigger use \ref TriggerEx.
 * \todo Add derived class TriggerX.
 * \sa TriggerX
 *
 * \ingroup triggers
 */
class Trigger : public Format
{
	public:
		typedef boost::ptr_vector<TriggerFunction> Functions;

		Trigger();

		virtual std::streamsize read(InputStream & /* istream */)
		{
			throw Exception(_("Not usable."));
		}

		virtual std::streamsize read(InputStream &istream, const TriggerData &triggerData);
		virtual std::streamsize write(OutputStream &ostream) const;

		class Triggers* triggers() const;
		void setName(const string &name);
		const string& name() const;
		void setDescription(const string &description);
		const string& description() const;
		void setEnabled(bool enabled);
		bool isEnabled() const;
		bool isCustomText() const;
		void setInitiallyOn(bool on);
		bool isInitiallyOn() const;
		int32 unknown() const;
		int32 category() const;
		Functions& functions();
		const Functions& functions() const;

		/**
		 * Corresponding custom text trigger entry.
		 * \sa CustomTextTrigger
		 */
		const string& triggerText() const;

	protected:
		string m_name;
		string m_description;
		bool m_isEnabled;
		bool m_isCustomText;
		bool m_isInitiallyOn;
		int32 m_unknown;
		int32 m_category;
		Functions m_functions;
};

inline void Trigger::setName(const string& name)
{
	this->m_name = name;
}

inline const string& Trigger::name() const
{
	return m_name;
}

inline void Trigger::setDescription(const string &description)
{
	m_description = description;
}

inline const string& Trigger::description() const
{
	return m_description;
}

inline void Trigger::setEnabled(bool enabled)
{
	this->m_isEnabled = enabled;
}

inline bool Trigger::isEnabled() const
{
	return m_isEnabled;
}

inline bool Trigger::isCustomText() const
{
	return m_isCustomText;
}

inline void Trigger::setInitiallyOn(bool on)
{
	m_isInitiallyOn = on;
}

inline bool Trigger::isInitiallyOn() const
{
	return m_isInitiallyOn;
}

inline int32 Trigger::unknown() const
{
	return m_unknown;
}

inline int32 Trigger::category() const
{
	return m_category;
}

inline Trigger::Functions& Trigger::functions()
{
	return m_functions;
}

inline const Trigger::Functions& Trigger::functions() const
{
	return m_functions;
}

}

}

#endif
