/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EXCEPTION_HPP
#define WC3LIB_EXCEPTION_HPP

#include <exception>
#include <string>

#include <boost/exception/all.hpp>
#include <boost/format.hpp>

namespace wc3lib
{

/**
* Default exception class base.
* Is used by all exception classes in wc3lib.
*/
class Exception : public virtual std::exception, public virtual boost::exception
{
	public:
		Exception(const std::string &what = "") throw ();
		Exception(const boost::format &what) throw ();
		virtual ~Exception() throw ();
		const char* what() const throw ();
		std::string& what();

	protected:
		std::string m_what;
};

inline const char* Exception::what() const throw ()
{
	return this->m_what.c_str();
}

inline std::string& Exception::what()
{
	return this->m_what;
}

}

#endif
