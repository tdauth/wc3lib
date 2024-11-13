# NOTE search for OGRE before searching for KDE since KDE changes the variable CMAKE_MODULE_PATH
# OGRE does not install the CMake file into the module path. It uses the library path instead.
find_path(OGRE_CMAKE_MODULE_DIR FindOGRE.cmake PATHS ${CMAKE_MODULE_PATH})
message(STATUS "Searching for OGRE in ${CMAKE_MODULE_PATH}")

if (NOT OGRE_CMAKE_MODULE_DIR)
	find_path(OGRE_CMAKE_MODULE_DIR FindOGRE.cmake PATHS ${CMAKE_SYSTEM_LIBRARY_PATH} PATH_SUFFIXES OGRE/cmake)
endif ()

if (OGRE_CMAKE_MODULE_DIR)
	message(STATUS "Found OGRE CMake module in directory: ${OGRE_CMAKE_MODULE_DIR}")
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OGRE_CMAKE_MODULE_DIR})
else ()
	message(WARNING "Did not find OGRE CMake module.")
endif ()

find_package(OGRE "1.12" COMPONENTS Paging Terrain REQUIRED)

if (${OGRE_FOUND})
	# SYSTEM suppresses all the warnings of OGRE
	include_directories(SYSTEM ${OGRE_INCLUDE_DIRS})
	link_directories(${OGRE_LIB_DIR})

	add_definitions(-DHAS_OGRE=1)
endif ()