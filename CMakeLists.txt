cmake_minimum_required(VERSION 2.8.4)

project(wc3lib)

# The version number.
set(wc3lib_VERSION_MAJOR 0)
set(wc3lib_VERSION_MINOR 1)
set(wc3lib_VERSION_PATCH 0)
set(wc3lib_VERSION "${wc3lib_VERSION_MAJOR}.${wc3lib_VERSION_MINOR}.${wc3lib_VERSION_PATCH}")

enable_testing()

include(CMakeDependentOption)

option(APP
	"Builds various applications for all enabled modules, not only libraries."
	ON)
option(BLP
	"Builds BLP module. Required by options EDITOR and PLUGINS."
	ON)
option(JASS
	"JASS module supports Blizzard's scripting language JASS. It provides an abstract parser interface for the scripting language."
	ON)
CMAKE_DEPENDENT_OPTION(JASS_LLVM
	"Builds LLVM bindings for the JASS module."
	OFF
	"JASS" OFF)
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
CMAKE_DEPENDENT_OPTION(USE_ENCRYPTION
	"Enables encryption support for MPQ archives which requires Crypto++. Otherwise, only OpenSSL or something similar is required for an MD5 function."
	ON
	"MPQ" OFF)
option(DOC
	"Generates doxygen documentation (doxygen is required)"
	OFF)

# custom compilation definitions
# run debug detection before configuring file to set proper preprocessor
if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL debugfull OR CMAKE_BUILD_TYPE STREQUAL debugfull OR CMAKE_BUILD_TYPE STREQUAL Profile OR DEBUG)
	message(STATUS "Detected debug mode with CMake build type: \"${CMAKE_BUILD_TYPE}\".")
	set(DEBUG ON)
	add_definitions(-DDEBUG=1)
endif ()

# required for locale files - gettext
# set definition before configuring file
add_definitions(-DLOCALE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\")

# NOTE config.h has to be installed since it is included in various header files and therefore required in the public API (for example #include "../config.h")
# it contains the defines for all possible CMake options and is expected to be in the top level src directory ("include/wc3lib/")
configure_file (
	"${PROJECT_SOURCE_DIR}/wc3libConfig.h.in"
	"${PROJECT_BINARY_DIR}/src/config.h"
)
install(FILES "${PROJECT_BINARY_DIR}/src/config.h" DESTINATION include/wc3lib)

# add the binary src tree to the search path for include files
# so that we will find config.h
include_directories("${PROJECT_BINARY_DIR}/src")

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

include_directories(src lib)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(locale)
add_subdirectory(mime)
add_subdirectory(services)
add_subdirectory(share)

# CPack

set(CPACK_COMPONENT_GROUP_APPLICATIONS_DESCRIPTION "Additional tools which help you to use the formats of Blizzard Entertainment.")
set(CPACK_COMPONENT_GROUP_CORE_DESCRIPTION "The core of the wc3lib which supports in and output for basic formats.")
set(CPACK_COMPONENT_GROUP_EDITOR_DESCRIPTION "The GUI layer for all basic formats.")

set(CPACK_COMPONENT_APP_GROUP "Applications")

if (BLP)
	set(CPACK_COMPONENT_BLP_DISPLAY_NAME "BLP support")
	set(CPACK_COMPONENT_BLP_DESCRIPTION "Adds image format support.")
	set(CPACK_COMPONENT_BLP_GROUP "Core")
endif ()

if (MAP)
	set(CPACK_COMPONENT_MAP_DISPLAY_NAME "Map (W3M/W3X/W3N) support")
	set(CPACK_COMPONENT_MAP_GROUP "Core")
endif ()

if (MDLX)
	set(CPACK_COMPONENT_MDLX_DISPLAY_NAME "MDX and MDL support")
	set(CPACK_COMPONENT_MDLX_GROUP "Core")
endif ()

if (EDITOR)
	set(CPACK_COMPONENT_EDITOR_GROUP "Editor")
	# TODO add dependencies and add options as dependencies!
	set(CPACK_COMPONENT_EDITOR_DEPENDS blp mdlx)
endif ()

if (PLUGINS)
	set(CPACK_COMPONENT_PLUGINS_GROUP "Editor")
endif ()

#set(CPACK_COMPONENTS_ALL blp mdlx)
set(CPACK_PACKAGE_NAME "wc3lib")
set(CPACK_PACKAGE_VENDOR "wc3lib.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "wc3lib - Warcraft III Library")
set(CPACK_PACKAGE_VERSION "${wc3lib_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${wc3lib_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${wc3lib_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${wc3lib_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CPack Component Example")
set(CPACK_PACKAGE_CONTACT "wc3lib@wc3lib.org")

include(CPack)
