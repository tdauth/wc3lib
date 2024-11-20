cmake_minimum_required(VERSION 3.0.0)

project(wc3lib)

# The version number.
if (NOT DEFINED wc3lib_VERSION_MAJOR)
	message(STATUS "wc3lib_VERSION_MAJOR is set automatically.")
	set(wc3lib_VERSION_MAJOR 0)
endif ()
if (NOT DEFINED wc3lib_VERSION_MINOR)
	message(STATUS "wc3lib_VERSION_MINOR is set automatically.")
	set(wc3lib_VERSION_MINOR 1)
endif ()
if (NOT DEFINED wc3lib_VERSION_PATCH)
	message(STATUS "wc3lib_VERSION_PATCH is set automatically.")
	set(wc3lib_VERSION_PATCH 0)
endif ()
set(wc3lib_VERSION "${wc3lib_VERSION_MAJOR}.${wc3lib_VERSION_MINOR}.${wc3lib_VERSION_PATCH}")
message(STATUS "Building version ${wc3lib_VERSION}.")

enable_testing()

# required to generate the "DartConfiguration.tcl" file in the build directory and to find valgrind
include(CTest)

# Add C++20 support explicitely.
add_compile_options(-std=c++20)

include(CMakeDependentOption)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

option(APP
	"Builds various applications for all enabled modules, not only libraries."
	ON)
option(BLP
	"Builds BLP module. Required by options EDITOR and PLUGINS."
	ON)
option(JASS
	"JASS module supports Blizzard's scripting language JASS. It provides an abstract parser interface for the scripting language."
	OFF)
option(MAP
	"Builds map module. Required by option EDITOR."
	ON)
option(MDLX
	"Builds MDLX module. Required by by option EDITOR."
	OFF) # TODO Unfinished at the moment
option(MPQ
	"Builds MPQ module. Required by options EDITOR and PLUGINS."
	ON)
option(W3G
	"Builds W3G module."
	ON)
CMAKE_DEPENDENT_OPTION(JSON
	"JSON module supports converting all formats into JSON and from JSON."
	ON
	"BLP;MAP;MDLX" OFF)
CMAKE_DEPENDENT_OPTION(EDITOR
	"Wacraft III World Editor emulation is built (Qt5 is required, OGRE is only required when MDLX or USE_OGREBLP is enabled)."
	ON
	"BLP;MAP;MPQ" OFF)
CMAKE_DEPENDENT_OPTION(USE_KIOSLAVE
	"Creates a KIO slave for MPQ (KF5 is required)."
	OFF
	"MPQ" OFF)
CMAKE_DEPENDENT_OPTION(USE_QBLP
	"Creates a Qt image format plugin for BLP."
	ON
	"BLP" OFF)
CMAKE_DEPENDENT_OPTION(USE_OGREBLP
	"Creates a OGRE image format plugin for BLP (OGRE is required)."
	OFF
	"BLP" OFF)
CMAKE_DEPENDENT_OPTION(GAME
	"Wacraft III emulation is built (Qt5, OGRE and EDITOR is required)."
	ON
	"BLP;MAP;MPQ;MDLX;JASS;EDITOR;USE_OGREBLP" OFF)
option(USE_MIME
	"Installs MIME types (KF5 is required)"
	ON)
option(DOC
	"Generates doxygen documentation (doxygen is required)"
	OFF)
set(WC3_DIR "" CACHE STRING "Warcraft III: Frozen Throne directory required for certain unit tests.")

# custom compilation definitions
# run debug detection before configuring file to set proper preprocessor
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "debugfull" OR "${CMAKE_BUILD_TYPE}" STREQUAL "Profile" OR DEBUG)
	message(STATUS "Detected debug mode with CMake build type: \"${CMAKE_BUILD_TYPE}\".")
	set(DEBUG ON)
	add_definitions(-DDEBUG=1)

	# add best possible output to get information about code quality
	# TODO more generic setup of enabling all warnings/feedback the compiler offers
	#CHECK_CXX_COMPILER_FLAG("-Wall" supportsWall)

	#if (supportsWall)
	if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		message(STATUS "Enabled advanced warnings output.")
		add_compile_options(-Wall)
	else ()
		message(WARNING "Compiler does not support -Wall.")
	endif ()
endif ()

# required for locale files - gettext
# set definition before configuring file
add_definitions(-DLOCALE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\")

# NOTE config.h has to be installed since it is included in various header files and therefore required in the public API (for example #include "../config.h")
# it contains the defines for all possible CMake options and is expected to be in the top level src directory ("include/wc3lib/")
configure_file (
	"${PROJECT_SOURCE_DIR}/wc3libConfig.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)
install(FILES "${PROJECT_BINARY_DIR}/config.h" DESTINATION include/wc3lib)

# add the binary src tree to the search path for include files
# so that we will find config.h
include_directories("${PROJECT_BINARY_DIR}")

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

	# Build shared libraries on Unix systems.
	set (BUILD_SHARED_LIBS 1)
elseif (WIN32)
	add_definitions(-DWINDOWS=1)
	message(STATUS "Detected Windows operating system.")

	# Build static libraries on Windows systems.
	set (BUILD_SHARED_LIBS 0)
else ()
	message(WARNING "Unknown operating system!")
endif ()

include_directories(src lib)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(etc)
add_subdirectory(locale)
add_subdirectory(share)

# CPack

set(CPACK_PACKAGE_NAME "wc3lib")
set(CPACK_PACKAGE_VENDOR "https://github.com/tdauth/wc3lib")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "wc3lib - Warcraft III Library")
set(CPACK_PACKAGE_VERSION "${wc3lib_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${wc3lib_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${wc3lib_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${wc3lib_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README  "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")

# RPM, Fedora
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_URL "https://github.com/tdauth/wc3lib")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv2")
set(CPACK_RPM_PACKAGE_REQUIRES "boost, gettext")

if (MPQ)
	set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, bzip2, zlib")
endif ()

if (BLP)
	set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, libjpeg-turbo")
endif ()

if (EDITOR OR PLUGINS)
	set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, qt, kdelibs, ogre")
endif ()

# Debian/Ubuntu
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamino Dauth")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev, libgettextpo0")

if (MPQ)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, bzip2, zlib1g")
endif ()

if (BLP)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libjpeg-turbo8")
endif ()

if (EDITOR OR PLUGINS)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, qt5-default, kdelibs-bin, libogre-1.9.0v5")
endif ()

# TODO NSIS, include(InstallRequiredSystemLibraries)

include(CPack)
