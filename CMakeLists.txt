cmake_minimum_required(VERSION 2.8.4)

project(wc3lib)

# The version number.
set(wc3lib_VERSION_MAJOR 0)
set(wc3lib_VERSION_MINOR 1)
set(wc3lib_VERSION "${wc3lib_VERSION_MAJOR}.${wc3lib_VERSION_MINOR}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/wc3libConfig.h.in"
	"${PROJECT_BINARY_DIR}/wc3libConfig.h"
)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

include(CMakeDependentOption)

option(STATIC
	"Everything will be linked statically with applications if possible (creates big static wc3lib library)."
	OFF)
option(NORUNTIME
	"Dynamic format libraries (e. g. jpeg library) will be linked during compilation and won't be loaded dynamically at runtime."
	OFF)
option(APP
	"Builds various applications for all enabled modules, not only libraries."
	ON)
option(BLP
	"Builds BLP module. Required by options EDITOR and PLUGINS."
	ON)
option(MAP
	"Builds map module. Required by option EDITOR."
	ON)
option(MDLX
	"Builds MDLX module. Required by by option EDITOR."
	ON)
option(MPQ
	"Builds MPQ module. Required by options EDITOR and PLUGINS."
	ON)
option(W3G
	"Builds W3G module."
	ON)
CMAKE_DEPENDENT_OPTION(EDITOR
	"Wacraft III World Editor emulation is built. Qt/KDE is required!"
	ON
	"BLP;MAP;MDLX;MPQ" OFF)
CMAKE_DEPENDENT_OPTION(PLUGINS
	"Could be used without EDITOR and installs Qt/KDE and OGRE format plugins only. Does not build the whole World Editor emulation."
	ON
	"BLP;MPQ" OFF)
CMAKE_DEPENDENT_OPTION(ENCRYPTION
	"Enables encryption support for MPQ archives which requires Crypto++. Otherwise, only OpenSSL or something similar is required for an MD5 function."
	ON
	"MPQ" OFF)
option(DOC
	"Generates doxygen documentation (doxygen is required)"
	ON)

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

function (WC3LIB_GET_TXT resultName)
	file(GLOB list *.txt)
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
		#WC3LIB_ENABLE_COMPILE_FLAG("-std=c\\+\\+0x")
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
if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL debugfull OR CMAKE_BUILD_TYPE STREQUAL debugfull OR CMAKE_BUILD_TYPE STREQUAL Profile OR DEBUG)
	message(STATUS "Detected debug mode: ${CMAKE_BUILD_TYPE}.")
	set(DEBUG ON)
	add_definitions(-DDEBUG=1)
endif ()

if (STATIC)
	set(wc3lib_CREATION_MODE STATIC)
else ()
	set(wc3lib_CREATION_MODE SHARED)
endif ()

find_package(Gettext REQUIRED)

# Both compression libraries are required by Boost "iostreams" component which is used for the "mpq" module.
# Crypto++ library is required for RSA encryption and SHA1 digest and MD5 checksum calculation (Boost does only support CRC).
if (MPQ)
	find_package(BZip2 REQUIRED)
	include_directories(${BZIP2_INCLUDE_DIR})
	find_package(ZLIB REQUIRED)
	include_directories(${ZLIB_INCLUDE_DIRS})

	if (ENCRYPTION)
		set(CRYPTOPP_INCLUDE_DIRS "${CMAKE_SYSTEM_INCLUDE_PATH}")
		set(CRYPTOPP_LIB_DIR "${CMAKE_SYSTEM_LIBRARY_PATH}")

		find_path(tmp "rsa.h" "${CRYPTOPP_INCLUDE_DIRS}/crypto++")

		if (NOT (${tmp} EQUAL "tmp-NOTFOUND"))
			include_directories(${CRYPTOPP_INCLUDE_DIRS})
			link_directories(${CRYPTOPP_LIB_DIR})
			find_library(CRYPTOPP_LIB NAMES crypto++ REQUIRED "${CRYPTOPP_LIB_DIR}")

			if (NOT (CRYPTOPP_LIB EQUAL "CRYPTOPP_LIB-NOTFOUND"))
				set(CRYPTOPP_LIBRARIES ${CRYPTOPP_LIB})
				message(STATUS "Crypto++ libraries: ${CRYPTOPP_LIBRARIES}")
				set(CRYPTOPP_FOUND 1)
			else ()
				set(CRYPTOPP_FOUND 0)
			endif ()
		else ()
			message(FATAL_ERROR "\"rsa.h\" could not be found in paths: ${CRYPTOPP_INCLUDE_DIRS}.")
		endif ()
	else ()
		find_package(OpenSSL REQUIRED)
		include_directories(${OPENSSL_INCLUDE_DIR})
	endif ()

endif ()

# Boost
if (STATIC)
	set(Boost_USE_STATIC_LIBS ON)
else ()
	set(Boost_USE_STATIC_LIBS OFF)
endif ()
set(Boost_USE_MULTITHREADED ON)
if (DEBUG)
	set(Boost_DEBUG ON)
endif ()
#set(Boost_DETAILED_FAILURE_MSG ON)
#set(Boost_ADDITIONAL_VERSIONS "1.40" "1.40.0" "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0")
find_package(Boost 1.46 COMPONENTS system filesystem thread serialization program_options iostreams REQUIRED)

# Static linking with JPEG (BLP library)
if (BLP)
	if (STATIC OR NORUNTIME)
		find_library(jpeg REQUIRED)
	endif ()
endif ()

if (EDITOR OR PLUGINS)
	if (EDITOR)
		find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
	elseif (PLUGINS)
		find_package(Qt4 COMPONENTS QtCore QtGui)
	endif ()

	if (${QT4_FOUND})
		include(${QT_USE_FILE})
		add_definitions(${QT_DEFINITIONS})
		include_directories(${QT_INCLUDE_DIR})
	endif ()

	if (EDITOR)
		find_package(KDE4 REQUIRED)
	elseif (PLUGINS)
		find_package(KDE4) # only for MPQ plugins
	endif ()

	if (${KDE4_FOUND})
		add_definitions(${KDE4_DEFINITIONS})
		include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES})
		link_directories(${KDE4_LIB_DIR})
	endif ()

	set (OGRE_INCLUDE_DIRS "${CMAKE_SYSTEM_INCLUDE_PATH}/OGRE" "${CMAKE_SYSTEM_INCLUDE_PATH}/OGRE/Terrain")
	set(OGRE_LIB_DIR "${CMAKE_SYSTEM_LIBRARY_PATH}")

	find_path(tmp "Ogre.h" ${OGRE_INCLUDE_DIRS})

	if (tmp)
		include_directories(${OGRE_INCLUDE_DIRS})
		link_directories(${OGRE_LIB_DIR})

		if (EDITOR)
			find_library(OGRE_MAIN NAMES OgreMain REQUIRED "${OGRE_LIB_DIR}")
			find_library(OGRE_TERRAIN NAMES OgreTerrain REQUIRED "${OGRE_LIB_DIR}")
		elseif (PLUGINS)
			find_library(OGRE_MAIN NAMES OgreMain "${OGRE_LIB_DIR}")
		endif ()

		if (NOT (OGRE_MAIN EQUAL "OGRE_MAIN-NOTFOUND") AND (NOT EDITOR OR NOT (OGRE_TERRAIN EQUAL "OGRE_TERRAIN-NOTFOUND")))
			set(OGRE_LIBRARIES ${OGRE_MAIN})
			if (EDITOR)
				set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${OGRE_TERRAIN})
			endif ()
			message(STATUS "OGRE libraries: ${OGRE_LIBRARIES}")
			set(OGRE_FOUND 1)
		else ()
			set(OGRE_FOUND 0)
		endif ()
	elseif (EDITOR)
		message(FATAL_ERROR "\"Ogre.h\" could not be found in paths: ${OGRE_INCLUDE_DIRS}.")
	else ()
		set(OGRE_FOUND 0) # for plugins
	endif ()
else ()
	set(QT4_FOUND 0)
	set(KDE4_FOUND 0)
	set(OGRE_FOUND 0)
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
add_subdirectory(share)

#include(CPack)