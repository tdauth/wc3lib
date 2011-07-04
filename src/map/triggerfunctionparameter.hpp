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

#ifndef WC3LIB_MAP_TRIGGERFUNCTIONPARAMETER_HPP
#define WC3LIB_MAP_TRIGGERFUNCTIONPARAMETER_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * \todo Add derived class TriggerFunctionParamterX.
 * \sa TriggerFunctionParameterX
 */
class TriggerFunctionParameter : public Format
{
	public:
		BOOST_SCOPED_ENUM_START(Type)
		{
			Preset,
			Variable,
			Function
		};
		BOOST_SCOPED_ENUM_END

		TriggerFunctionParameter(class TriggerFunction *function);

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);
		
		BOOST_SCOPED_ENUM(Type) type() const;
		const string& value() const;
		int32 unknown0() const;
		int32 unknown1() const;

	protected:
		class TriggerFunction *m_function;
		BOOST_SCOPED_ENUM(Type) m_type;
		string m_value;
		int32 m_unknown0;
		int32 m_unknown1;
};

inline BOOST_SCOPED_ENUM(TriggerFunctionParameter::Type) TriggerFunctionParameter::type() const
{
	return m_type;
}

inline const string& TriggerFunctionParameter::value() const
{
	return m_value;
}

inline int32 TriggerFunctionParameter::unknown0() const
{
	return m_unknown0;
}

inline int32 TriggerFunctionParameter::unknown1() const
{
	return m_unknown1;
}

}

}

#endif
