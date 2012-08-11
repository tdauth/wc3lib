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
 */
class TriggerFunction : public Format
{
	public:
		typedef boost::ptr_vector<TriggerFunctionParameter> Parameters;

		BOOST_SCOPED_ENUM_START(Type) /// \todo C++11 : int32
		{
			Event,
			Condition,
			Action
		};
		BOOST_SCOPED_ENUM_END

		TriggerFunction();

		BOOST_SCOPED_ENUM(Type) type() const;
		const string& name() const;
		bool isEnabled() const;
		Parameters& parameters();
		const Parameters& parameters() const;

		virtual std::streamsize read(InputStream &istream) throw (class Exception)
		{
			throw Exception(_("Not usable."));
		}

		virtual std::streamsize read(InputStream &istream, const TriggerData &triggerData) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);


	protected:
		BOOST_SCOPED_ENUM(Type) m_type;
		string m_name;
		bool m_isEnabled;
		Parameters m_parameters;
};

inline BOOST_SCOPED_ENUM(TriggerFunction::Type) TriggerFunction::type() const
{
	return m_type;
}

inline const string& TriggerFunction::name() const
{
	return m_name;
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
