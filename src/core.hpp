/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_CORE_HPP
#define WC3LIB_CORE_HPP

namespace wc3lib
{
/**
 * \page coresection Core
 * The core of the wc3lib is shared by all of its modules. It provides some basic functions and classes for exception handling, internationalisation, runtime loading of shared objects/DLLs and I/O operations.
 *
 * Class \ref Exception is the base class of all exceptions thrown by functions of the wc3lib.
 *
 * Include \ref i18n.hpp to use \ref boost::format and gettext macros for internationalisation of your program.
 *
 * Class \ref Format is the base class of all format related classes of the wc3lib. It supports some basic serialization member functions.
 *
 * Include \ref utilities.hpp to use many I/O stream functions heavily used by all supported format classes. Besides it includes many default components of the STL and the Boost C++ Libraries and provides some error/info output functions with human-readable messages and class \ref Vertex.
 *
 * \namespace wc3lib
 * \brief \ref coresection
 */
}

#include "color.hpp"
#include "exception.hpp"
#include "format.hpp"
#include "i18n.hpp"
#include "platform.hpp"
#include "utilities.hpp"
#include "vertex.hpp"

#endif
