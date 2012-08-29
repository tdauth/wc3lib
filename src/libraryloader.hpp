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

#ifndef WC3LIB_LIBRARYLOADER_HPP
#define WC3LIB_LIBRARYLOADER_HPP

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

#include "config.h"
#include "exception.hpp"

namespace wc3lib
{

/**
* Static class which allows you to load shared objects (or dynamic link libraries) dynamically which means at the program's runtime.
* It provides a consistent interface which is usable on UNIX and Windows platforms.
* @todo Add Windows support.
*/
class LibraryLoader : public boost::noncopyable
{
	public:
		typedef LibraryLoader self;

#ifdef UNIX
		typedef void* HandleType;
#elif defined WINDOWS
		typedef hInstance HandleType;
#endif

		class Handle
		{
			public:
				Handle(HandleType h, const boost::filesystem::path &p) : handle(h), path(p)
				{

				}

				HandleType handle;
				boost::filesystem::path path;
		};

		/**
		 * Loads library called from \p path and returns a newly allocated handle which refers to it.
		 * \param path File extension is checked and completed automatically if missing. On Unix system "lib" prefix is added as well.
		 * \return Returns a newly allocated \ref Handle object.
		 */
		static class Handle* loadLibrary(const boost::filesystem::path &path) throw (class Exception);
		/**
		 * Unloads library from \p handle and deletes it.
		 */
		static void unloadLibrary(class Handle *handle) throw (class Exception);
		/**
		 * Loads symbol refered as \p symbolName from library handle \param handle and returns it as void* object.
		 */
		static void* librarySymbol(const class Handle &handle, const char *symbolName) throw (class Exception);

	private:
		LibraryLoader();
		~LibraryLoader();
};

}

#endif
