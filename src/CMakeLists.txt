find_package(Gettext REQUIRED)

# Boost
if (DEBUG)
	#set(Boost_DEBUG ON)
	add_definitions(-DBOOST_TEST_DYN_LINK=1)
endif ()

find_package(Boost COMPONENTS system filesystem serialization program_options iostreams REQUIRED)

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${GETTEXT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

set(wc3lib_CORE_H
	blp.hpp
	color.hpp
	core.hpp
	editor.hpp
	exception.hpp
	format.hpp
	i18n.hpp
	jass.hpp
	map.hpp
	mdlx.hpp
	mpq.hpp
	platform.hpp
	utilities.hpp
	vertex.hpp
	wc3lib.hpp
)
set(wc3lib_CORE_SRC
	color.cpp
	exception.cpp
	format.cpp
	spirit.cpp
	utilities.cpp
	vertex.cpp
)

if (EDITOR OR PLUGINS)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/editor) # include binary dir since the KDE configuration class will be generated there and has to be included (also include for moc cxx files)
endif ()

add_library(wc3libcore ${wc3lib_CREATION_MODE} ${wc3lib_CORE_SRC})
if (UNIX)
	# TODO: dl is required for using dlopen etc. (cross platform?)
	target_link_libraries(wc3libcore dl)
endif ()
target_link_libraries(wc3libcore ${Boost_LIBRARIES})

if (BLP)
	add_subdirectory(blp)
endif ()

if (MDLX)
	add_subdirectory(mdlx)
endif ()
if (MPQ)
	add_subdirectory(mpq)
endif ()
if (MAP)
	add_subdirectory(map)
endif ()
if (JASS)
	add_subdirectory(jass)
endif ()

# libraries without editor
set(wc3lib_CORE_LIBRARIES wc3libcore)
if (BLP)
	list(APPEND wc3lib_CORE_LIBRARIES wc3libblp)
endif ()
if (MDLX)
	list(APPEND wc3lib_CORE_LIBRARIES wc3libmdlx)
endif ()
if (MPQ)
	list(APPEND wc3lib_CORE_LIBRARIES wc3libmpq)
endif ()
if (MAP)
	list(APPEND wc3lib_CORE_LIBRARIES wc3libmap)
endif ()
if (JASS)
	list(APPEND wc3lib_CORE_LIBRARIES wc3libjass)
endif ()

if (EDITOR OR PLUGINS)
	add_subdirectory(editor) # last one
endif ()

# all available libraries
set(wc3lib_LIBRARIES ${wc3lib_CORE_LIBRARIES})
set(wc3lib_SRC ${wc3lib_CORE_SRC})
if (BLP)
	set(wc3lib_SRC ${wc3lib_SRC} ${wc3lib_BLP_SRC})
endif ()
if (MDLX)
	set(wc3lib_SRC ${wc3lib_SRC} ${wc3lib_MDLX_SRC})
endif ()
if (MPQ)
	set(wc3lib_SRC ${wc3lib_SRC} ${wc3lib_MPQ_SRC})
endif ()
if (MAP)
	set(wc3lib_SRC ${wc3lib_SRC} ${wc3lib_MAP_SRC})
endif ()
if (JASS)
	set(wc3lib_SRC ${wc3lib_SRC} ${wc3lib_JASS_SRC})
endif ()

if (EDITOR)
	set(wc3lib_LIBRARIES ${wc3lib_LIBRARIES} wc3libeditor)
	set(wc3lib_SRC ${wc3lib_SRC} ${wc3lib_EDITOR_SRC})
endif ()

if (APP)
	add_subdirectory(app)
endif ()

# install
install(TARGETS wc3libcore DESTINATION lib)
install(FILES ${wc3lib_CORE_H} DESTINATION include/wc3lib)