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

#include "platform.hpp"
#include "triggerfunction.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Definition of a Warcraft 3 trigger. For Warcraft 3 The Frozen Trigger use TriggerEx.
 * \todo Add derived class TriggerX.
 * \sa TriggerX
 */
class Trigger : public Format
{
	public:
		typedef boost::shared_ptr<TriggerFunction> FunctionPtr;
		typedef std::vector<FunctionPtr> Functions;

		Trigger(class Triggers *triggers);

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		class Triggers* triggers() const;
		const string& name() const;
		void setDescription(const string &description);
		const string& description() const;
		void setEnabled(bool enabled);
		bool isEnabled() const;
		bool isCustomText() const;
		void setInitiallyOn(bool on);
		bool isInitiallyOn() const;
		int32 unknown() const;
		class TriggerCategory* category() const;
		const Functions& functions() const;

	protected:
		class Triggers *m_triggers;
		string m_name;
		string m_description;
		bool m_isEnabled;
		bool m_isCustomText;
		bool m_isInitiallyOn;
		int32 m_unknown;
		class TriggerCategory *m_category;
		Functions m_functions;
};

inline class Triggers* Trigger::triggers() const
{
	return m_triggers;
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
	m_isEnabled = enabled;
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

inline class TriggerCategory* Trigger::category() const
{
	return m_category;
}

inline const Trigger::Functions& Trigger::functions() const
{
	return m_functions;
}

}

}

#endif
