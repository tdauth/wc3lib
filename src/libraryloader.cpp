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

/**
* Preprocessors has been taken from OGRE.
*/
#ifdef UNIX
#include <dlfcn.h>
#endif

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
	#if !defined(NOMINMAX) && defined(_MSC_VER)
	#define NOMINMAX // required to stop windows.h messing up std::min
	#endif
#include <windows.h>
#endif

#ifdef MAC
#include "macUtils.h"
#endif

#include <boost/format.hpp>

#include "libraryloader.hpp"
#include "internationalisation.hpp"

namespace wc3lib
{

class LibraryLoader::Handle* LibraryLoader::loadLibrary(const boost::filesystem::path &path) throw (class Exception)
{
	std::string libraryName = path.filename().string();

#ifdef MAC
	libraryName.append(".dylib");
#elif defined (UNIX)
	libraryName.insert(0, "lib").append(".so");
#elif defined WINDOWS
	libraryName.append(".dll");
#endif
	boost::filesystem::path newPath(path.root_path() / libraryName); /// \todo If exists the absolute directory path and the library name!
	HandleType handle = 0;

#ifdef MAC
	handle = mac_loadDylib(newPath.string().c_str());
#elif defined (UNIX)
	handle = dlopen(newPath.string().c_str(), RTLD_LAZY | RTLD_GLOBAL);
#elif defined (WINDOWS)
	handle = LoadLibraryEx(newPath.string().c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
#endif

	if (handle == NULL)
	{
		std::string errorMessage;

#ifdef UNIX
		errorMessage = dlerror();
/// @todo Support other system errors
#endif

		throw Exception(boost::format(_("Error while loading shared object \"%1%\": %2%")) % newPath.string() % errorMessage);
	}

	class Handle *result = new Handle;
	result->handle = handle;
	result->path = newPath;

	return result;
}

void LibraryLoader::unloadLibrary(class Handle *handle) throw (class Exception)
{
	if (handle == 0)
		throw Exception(_("Error while unloading shared object. Handle is 0."));

	// Library hasn't already been loaded.
#ifdef UNIX
	if (dlclose(handle->handle) != 0)
#elif defined WINDOWS
	if (FreeLibrary(handle->handle))
#endif
		throw Exception(boost::format(_("Error while unloading shared object \"%1%\". It has never been loaded.")) % handle->path.string());

	delete handle;
}

void* LibraryLoader::librarySymbol(const class Handle &handle, const std::string symbolName) throw (class Exception)
{
#ifdef UNIX
	dlerror(); // clean up errors
#endif

	// get symbol
	HandleType symbolHandle = 0;

#ifdef UNIX
	symbolHandle = dlsym(handle.handle, symbolName.c_str());
#elif defined WINDOWS
	symbolHandle = GetProcAddress(handle.handle, symbolName.c_str());
#endif

	// got error
#ifdef UNIX
	if (dlerror() != NULL) {
		std::string message(dlerror());
#elif defined WINDOWS
	if (symbolHandle == 0) { /// @todo Right check?
		std::string message(""); /// @todo Error message?
#endif
		throw Exception(boost::format(_("Error while loading symbol \"%1%\" from shared object \"%2%\": %3%")) % symbolName % handle.path.string() % message);
	}

	return symbolHandle;
}

LibraryLoader::LibraryLoader()
{
}

}
