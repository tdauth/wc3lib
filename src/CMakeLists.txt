find_package(Gettext REQUIRED)

# Boost
if (DEBUG)
	#set(Boost_DEBUG ON)
	add_definitions(-DBOOST_TEST_DYN_LINK=1)
endif ()

# regex is required for listfile in mpq module
find_package(Boost COMPONENTS system filesystem serialization program_options iostreams regex REQUIRED)

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${GETTEXT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

set(wc3lib_CORE_H
	color.hpp
	core.hpp
	exception.hpp
	format.hpp
	i18n.hpp
	platform.hpp
	qi.hpp
	spirit.hpp
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

if (EDITOR OR USE_KIOSLAVE OR USE_QBLP OR USE_OGREBLP)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/editor) # include binary dir since the KDE configuration class will be generated there and has to be included (also include for moc cxx files)
endif ()

add_library(wc3libcore SHARED ${wc3lib_CORE_SRC})
target_link_libraries(wc3libcore ${Boost_LIBRARIES})

if (DEBUG)
	add_subdirectory(test)
endif ()

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

if (EDITOR OR USE_KIOSLAVE OR USE_QBLP OR USE_OGREBLP)
	add_subdirectory(editor) # last one
endif ()

if (BLP)
	install(FILES blp.hpp DESTINATION include/wc3lib)
endif ()
if (MDLX)
	install(FILES mdlx.hpp DESTINATION include/wc3lib)
endif ()
if (MPQ)
	install(FILES mpq.hpp DESTINATION include/wc3lib)
endif ()
if (MAP)
	install(FILES map.hpp DESTINATION include/wc3lib)
endif ()
if (JASS)
	install(FILES jass.hpp DESTINATION include/wc3lib)
endif ()

if (EDITOR)
	install(FILES editor.hpp DESTINATION include/wc3lib)
endif ()

if (APP)
	add_subdirectory(app)
endif ()

# install
install(TARGETS wc3libcore DESTINATION lib)
install(FILES ${wc3lib_CORE_H} DESTINATION include/wc3lib)