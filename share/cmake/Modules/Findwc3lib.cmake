# - Warcraft III library for development of modding tools
# Finds the Warcraft III library with specified components.
#
# Components can be:
# blp
#
# The following variables will be set:
#  wc3lib_FOUND - System has wc3lib
#  wc3lib_INCLUDE_DIRS - The wc3lib include directories
#  wc3lib_LIBRARIES - The libraries needed to use wc3lib
#  wc3lib_DEFINITIONS - Compiler switches required for using wc3lib

#=============================================================================
# Copyright 2014 Tamino Dauth
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# /usr/share/cmake/Modules/readme.txt
# http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries#Writing_find_modules
# - Try to find wc3lib
# Once done this will define
#  wc3lib_FOUND - System has wc3lib
#  wc3lib_INCLUDE_DIRS - The wc3lib include directories
#  wc3lib_LIBRARIES - The libraries needed to use wc3lib
#  wc3lib_DEFINITIONS - Compiler switches required for using wc3lib

include(LibFindMacros)

# Dependencies
libfind_package(wc3lib wc3lib)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(wc3lib_PKGCONF wc3lib)

# Include dir
find_path(wc3lib_INCLUDE_DIR
  NAMES wc3lib.hpp
  PATHS ${wc3lib_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(wc3lib_LIBRARY
  NAMES wc3lib
  PATHS ${wc3lib_PKGCONF_LIBRARY_DIRS}
)

# components
if(wc3lib_FIND_COMPONENTS )
	foreach(comp ${wc3lib_FIND_COMPONENTS})
		# Xxx_FIND_REQUIRED_Yyy
		# Xxx_Yyy_FOUND
		find_path(wc3lib_${comp}_INCLUDE_DIR
			NAMES ${comp}.hpp
			PATHS ${wc3lib_PKGCONF_INCLUDE_DIRS}
		)
		
		find_library(wc3lib_${comp}_LIBRARY
			NAMES wc3lib${comp}
			PATHS ${wc3lib_PKGCONF_LIBRARY_DIRS}
		)
		
		set(wc3lib_PROCESS_INCLUDES wc3lib_${comp}_INCLUDE_DIR wc3lib_INCLUDE_DIRS)
		set(wc3lib_PROCESS_LIBS wc3lib_${comp}_LIBRARY wc3lib_LIBRARIES)
	endforeach()
endif()


# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(wc3lib_PROCESS_INCLUDES wc3lib_INCLUDE_DIR wc3lib_INCLUDE_DIRS)
set(wc3lib_PROCESS_LIBS wc3lib_LIBRARY wc3lib_LIBRARIES)
libfind_process(wc3lib)