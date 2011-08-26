cmake_minimum_required(VERSION 2.8.4)

project(wc3lib)

function (WC3LIB_GET_H resultName)
	file(GLOB list *.hpp)
	list(APPEND result ${list})
	file(GLOB list *.hxx)
	list(APPEND result ${list})
	file(GLOB list *.h)
	list(APPEND result ${list})
	set(${resultName} ${result} PARENT_SCOPE)
endfunction ()

# TODO Exclude moc_* files
function (WC3LIB_GET_SRC resultName)
	file(GLOB list *.cpp)
	list(APPEND result ${list})
	file(GLOB list *.cxx)
	list(APPEND result ${list})
	file(GLOB list *.c)
	list(APPEND result ${list})
	set(${resultName} ${result} PARENT_SCOPE)
endfunction ()

function (WC3LIB_GET_UI resultName)
	file(GLOB list *.ui)
	list(APPEND result ${list})
	set(${resultName} ${result} PARENT_SCOPE)
endfunction ()

# Used for compiler flags like -Wall
function (WC3LIB_GET_COMPILE_FLAG variable flagName)
	string(REGEX MATCH "\\W*${flagName}\\W*" output "${CMAKE_CXX_FLAGS}")
	string(COMPARE EQUAL "${output}" "${flagName}" result)
	set(${variable} ${result} PARENT_SCOPE)
	message(STATUS "Flag \"${flagName}\" is ${result}.")
endfunction ()

# Used for compiler flags like -Wall
function (WC3LIB_ENABLE_COMPILE_FLAG flagName)
	WC3LIB_GET_COMPILE_FLAG(isSet "${flagName}")
	if (NOT isSet)
		message(STATUS "Enabling flag \"${flagName}\".")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flagName}" PARENT_SCOPE)
	endif ()
endfunction ()

function (WC3LIB_DISABLE_COMPILE_FLAG flagName)
	WC3LIB_GET_COMPILE_FLAG(isSet "${flagName}")
	message(STATUS "Is set is ${isSet}")
	if (isSet)
		message(STATUS "Disabling flag \"${flagName}\".")
		string(REPLACE "${flagName}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
	endif ()
endfunction ()

function (WC3LIB_REPLACE_COMPILE_FLAG flagName newFlagName)
	WC3LIB_GET_COMPILE_FLAG(isSet "${flagName}")
	if (isSet)
		message(STATUS "Replacing flag \"${flagName}\" by \"${newFlagName}\".")
		string(REPLACE "${flagName}" "${newFlagName}" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
	endif ()
endfunction ()

# get_property(input DIRECTORY PROPERTY COMPILE_DEFINITIONS)
# Used for -DBLA=1, cmake cache entry
function (WC3LIB_GET_COMPILE_DEFINITION variable definitionName)
	#set(variable 0 PARENT_SCOPE)
	#get_property(input DIRECTORY PROPERTY COMPILE_DEFINITIONS)
	#string(REGEX MATCH "\\W*-D ${definitionName}\\W*" output "${input}")
	#string(COMPARE EQUAL "${output}" "-D ${definitionName}" result)
	#if (result)
	#	set(variable 1 PARENT_SCOPE)
	#endif ()
	get_property(input CACHE "${definitionName}" PROPERTY VALUE)
	if (NOT input)
		set(input 0)
	endif ()
	set("${variable}" ${input} PARENT_SCOPE)
	message(STATUS "Definition \"${definitionName}\" is ${input}.")
endfunction ()

# TODO: Also setup C flags since we're using C code, too (e. g. huffman lib), CMAKE_C_FLAGS
function (WC3LIB_COMPILER_SETUP)
	# GCC options setup, has to be AFTER finding packages - Qt and KDE do overwrite CMAKE_CXX_FLAGS!
	if (CMAKE_COMPILER_IS_GNUCC)
		message(STATUS "Setting GCC compilation flags.")
		# Enable exception handling, disabled by Qt.
		# WC3LIB_REPLACE_COMPILE_FLAG("-fno-exceptions" "-fexceptions")
		# Required for dynamic linking
		WC3LIB_ENABLE_COMPILE_FLAG("-fPIC")
		# -fms-extensions is required by huffman lib since it uses anonymous struct which is not supported by ISO C!
		WC3LIB_ENABLE_COMPILE_FLAG("-fms-extensions")
		# NOTE For next ISO!
		# we're using some features of the new C++ ISO Standard such as default function templates arguments and scoped enums (via Boost)
		# WC3LIB_ENABLE_COMPILE_FLAG("-std=c\\+\\+0x")
		# set to parent scope since this is a function and these values should be the global C++ flags!
		# TODO WC3LIB_ENABLE_COMPILE_FLAG doesn't set the flag in this scope!
		# We need pthread support on Unix systems (used by Boost C++ Libraries).
		if (UNIX)
			WC3LIB_ENABLE_COMPILE_FLAG("-lpthread")
		endif ()
		set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} PARENT_SCOPE)
	endif ()
endfunction ()

# DEPRECATED
macro (WC3LIB_ENABLE_EXCEPTIONS)
endmacro ()

# operating system definitions
if (UNIX)
	add_definitions(-DUNIX=1)
	message(STATUS "Detected UNIX operating system.")
	if (APPLE)
		add_definitions(-DMAC=1)
		message(STATUS "Detected Mac operating system.")
	else ()
		add_definitions(-DLINUX=1)
		message(STATUS "Detected Linux operating system.")
	endif()
elseif (WIN32)
	add_definitions(-DWINDOWS=1)
	message(STATUS "Detected Windows operating system.")
else ()
	message(WARNING "Unknown operating system!")
endif ()

# custom compilation definitions
if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL debugfull OR CMAKE_BUILD_TYPE STREQUAL debugfull OR CMAKE_BUILD_TYPE STREQUAL Profile)
	message(STATUS "Detected debug mode: ${CMAKE_BUILD_TYPE}.")
	add_definitions(-DDEBUG=1)
	set(FLAG_DEBUG_USE 1)
endif ()
WC3LIB_GET_COMPILE_DEFINITION("FLAG_STATIC_USE" "STATIC")
WC3LIB_GET_COMPILE_DEFINITION("FLAG_NORUNTIME_USE" "NORUNTIME")
WC3LIB_GET_COMPILE_DEFINITION("FLAG_EDITOR_USE" "EDITOR")
WC3LIB_GET_COMPILE_DEFINITION("FLAG_PLUGINS_USE" "PLUGINS")
WC3LIB_GET_COMPILE_DEFINITION("FLAG_DOC_USE" "DOC")

if (FLAG_STATIC_USE)
	set(wc3lib_CREATION_MODE STATIC)
else ()
	set(wc3lib_CREATION_MODE SHARED)
endif ()

find_package(Gettext REQUIRED)

# Both libraries are required by Boost "iostreams" component which is used for the "mpq" module.
find_package(BZip2 REQUIRED)
include_directories(${BZIP2_INCLUDE_DIR})
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

# Boost
if (FLAG_STATIC_USE)
	set(Boost_USE_STATIC_LIBS ON)
else ()
	set(Boost_USE_STATIC_LIBS OFF)
endif ()
set(Boost_USE_MULTITHREADED ON)
if (FLAG_DEBUG_USE)
	set(Boost_DEBUG ON)
endif ()
set(Boost_DETAILED_FAILURE_MSG ON)
#set(Boost_ADDITIONAL_VERSIONS "1.40" "1.40.0" "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0")
find_package(Boost 1.46 COMPONENTS system filesystem thread serialization program_options iostreams REQUIRED)

# Static linking with JPEG (BLP library)
if (FLAG_STATIC_USE OR FLAG_NORUNTIME_USE)
	find_library(jpeg REQUIRED)
endif ()

if (FLAG_EDITOR_USE OR FLAG_PLUGINS_USE)
	message(WARNING "You're using -DEDITOR=1 which requires Qt and KDE and therefore will increase the compile time.")
	find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
	include(${QT_USE_FILE})
	find_package(KDE4 REQUIRED)
	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES} ${QT_INCLUDE_DIR})
	link_directories(${KDE4_LIB_DIR})
	# TODO Use cmake module to find and do not use lolib/extern/OGRE/ogre_src_v1-7-3/libcal version if possible (or in debug mode only - version 1.7.3)

	#option(OGRE_INCLUDE_DIRS "Directories of all necessary OGRE header files" "lib/extern/OGRE/ogre_src_v1-7-3/OgreMain/include/ lib/extern/OGRE/ogre_src_v1-7-3/include/ lib/extern/OGRE/ogre_src_v1-7-3/Components/Terrain/include/")
	#option(OGRE_LIB_DIR "Directory of all necessary OGRE libraries (OgreMain and OgreTerrain)" "lib/extern/OGRE/ogre_src_v1-7-3/lib")
	# TEST
	#set(OGRE_INCLUDE_DIRS lib/extern/OGRE/ogre_src_v1-7-3/OgreMain/include/ lib/extern/OGRE/ogre_src_v1-7-3/include/ lib/extern/OGRE/ogre_src_v1-7-3/Components/Terrain/include/)
	#set(OGRE_LIB_DIR lib/extern/OGRE/ogre_src_v1-7-3/lib)
	set(OGRE_INCLUDE_DIRS "${CMAKE_SYSTEM_INCLUDE_PATH}/OGRE" "${CMAKE_SYSTEM_INCLUDE_PATH}/OGRE/Terrain")
	set(OGRE_LIB_DIR "/usr/lib")

	if (NOT OGRE_INCLUDE_DIRS OR NOT OGRE_LIB_DIR)
		message(FATAL_ERROR "Define OGRE_INCLUDE_DIRS and OGRE_LIB_DIR!")
	endif ()

	message(STATUS "OGRE include directories:")
	foreach (dir IN LISTS OGRE_INCLUDE_DIRS)
		message(STATUS ${dir})
	endforeach ()
	message(STATUS "OGRE library directory: ${OGRE_LIB_DIR}")
	include_directories(${OGRE_INCLUDE_DIRS})
	link_directories(${OGRE_LIB_DIR})
	find_library(OgreMain REQUIRED)
	find_library(OgreTerrain REQUIRED)
	#include_directories(/usr/include/OGRE)
	#link_directories(/usr/lib/OGRE)
	set(OGRE_LIBRARIES OgreMain OgreTerrain) #  OgrePaging
endif ()

# TODO: is only required when using -DJPEG and -DSTATIC
#find_library(OPENJPEG_LIBS openjpeg) # todo Use custom cmake module to find openjpeg.

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${GETTEXT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

# Reset flags, Qt and KDE do set them to CRAP!
set(CMAKE_CXX_FLAGS "")
set(CMAKE_C_FLAGS "")
WC3LIB_COMPILER_SETUP()

message(STATUS "C++ Compile flags are: \"${CMAKE_CXX_FLAGS}\".")
message(STATUS "C Compile flags are: \"${CMAKE_C_FLAGS}\".")

# required for locale files - gettext
add_definitions(-DLOCALE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\")

include_directories(src lib)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(locale)
add_subdirectory(mime)
add_subdirectory(services)

#include(CPack)

