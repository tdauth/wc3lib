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

#ifndef WC3LIB_MAP_TRIGGERFUNCTION_HPP
#define WC3LIB_MAP_TRIGGERFUNCTION_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "triggerfunctionparameter.hpp"
#include "triggerdata.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \todo Add derived class TriggerFunctionX.
 * \sa TriggerFunctionEx
 *
 * \ingroup triggers
 */
class TriggerFunction : public Format
{
	public:
		typedef boost::ptr_vector<TriggerFunctionParameter> Parameters;

		enum class Type : int32
		{
			Event,
			Condition,
			Action,
			Call
		};

		TriggerFunction();

		void setType(Type type);
		Type type() const;
		void setName(const string &name);
		const string& name() const;
		void setIsEnabled(bool isEnabled);
		bool isEnabled() const;
		Parameters& parameters();
		const Parameters& parameters() const;

		virtual std::streamsize read(InputStream &istream)
		{
			throw Exception(_("Not usable."));
		}

		virtual std::streamsize read(InputStream &istream, const TriggerData &triggerData);
		virtual std::streamsize write(OutputStream &ostream) const;


	protected:
		Type m_type;
		string m_name;
		bool m_isEnabled;
		Parameters m_parameters;
};

inline void TriggerFunction::setType(TriggerFunction::Type type)
{
	this->m_type = type;
}

inline TriggerFunction::Type TriggerFunction::type() const
{
	return m_type;
}

inline void TriggerFunction::setName(const string& name)
{
	this->m_name = name;
}

inline const string& TriggerFunction::name() const
{
	return m_name;
}

inline void TriggerFunction::setIsEnabled(bool isEnabled)
{
	this->m_isEnabled = isEnabled;
}

inline bool TriggerFunction::isEnabled() const
{
	return m_isEnabled;
}

inline TriggerFunction::Parameters& TriggerFunction::parameters()
{
	return m_parameters;
}

inline const TriggerFunction::Parameters& TriggerFunction::parameters() const
{
	return m_parameters;
}

}

}

#endif
