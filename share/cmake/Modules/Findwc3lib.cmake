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

find_package(PkgConfig)
pkg_check_modules(PC_LIBWC3LIB QUIET wc3lib)
set(wc3lib_DEFINITIONS ${PC_wc3lib_CFLAGS_OTHER})

find_path(wc3lib_INCLUDE_DIR wc3lib/wc3lib.hpp
	HINTS ${PC_wc3lib_INCLUDEDIR} ${PC_wc3lib_INCLUDE_DIRS}
	PATH_SUFFIXES wc3lib )

find_library(wc3lib_LIBRARY NAMES wc3libcore
	HINTS ${PC_wc3lib_LIBDIR} ${PC_wc3libLIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(wc3lib  DEFAULT_MSG
                                  wc3lib_LIBRARY wc3lib_INCLUDE_DIR)

mark_as_advanced(wc3lib_INCLUDE_DIR wc3lib_LIBRARY)

set(wc3lib_LIBRARIES ${wc3lib_LIBRARY})
set(wc3lib_INCLUDE_DIRS ${wc3lib_INCLUDE_DIR})

# components
if (wc3lib_FIND_COMPONENTS)
	foreach(comp ${wc3lib_FIND_COMPONENTS})
		message(STATUS "Searching for component ${comp}.")
		# Xxx_FIND_REQUIRED_Yyy
		# Xxx_Yyy_FOUND
		find_path(wc3lib_${comp}_INCLUDE_DIR
			NAMES ${comp}.hpp
			HINTS ${PC_wc3lib_INCLUDEDIR} ${PC_wc3lib_INCLUDE_DIRS}
			PATH_SUFFIXES wc3lib
		)

		find_library(wc3lib_${comp}_LIBRARY
			NAMES wc3lib${comp}
			HINTS ${PC_wc3lib_LIBDIR} ${PC_wc3libLIBRARY_DIRS}
		)

		# Add include directory for extra libraries.
		if ("${comp}" STREQUAL "mpq")
			set(wc3lib_INCLUDE_DIRS ${wc3lib_INCLUDE_DIRS} ${wc3lib_INCLUDE_DIR}/wc3lib/lib)
		endif ()

		set(wc3lib_INCLUDE_DIRS ${wc3lib_INCLUDE_DIRS} ${wc3lib_${comp}_INCLUDE_DIR})
		set(wc3lib_LIBRARIES ${wc3lib_LIBRARIES} ${wc3lib_${comp}_LIBRARY})
	endforeach()
endif ()