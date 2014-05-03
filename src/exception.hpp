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

#include <boost/format.hpp>

#include "config.h"

namespace wc3lib
{

/**
* \brief Default exception class base which is used for all exception classes in wc3lib.
*/
class Exception : public std::exception
{
	public:
		Exception(const std::string &what = "") throw ();
		/**
		 * Simplified constructor using a boost::format instance that you do not need to call .str() before passing.
		 */
		Exception(const boost::format &format) throw ();
		virtual ~Exception() throw ();

		const char* what() const throw ();

	protected:
		std::string m_what;
};

inline const char* Exception::what() const throw ()
{
	return this->m_what.c_str();
}

}

#endif
