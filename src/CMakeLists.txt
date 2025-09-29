find_package(Gettext REQUIRED)

# Boost
if (DEBUG AND NOT WIN32)
	#set(Boost_DEBUG ON)
	add_definitions(-DBOOST_TEST_DYN_LINK=1)
endif ()

if (WIN32)
	#add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED=1)
        # TODO static libs are not found on Windows - finds only .lib files
	set(Boost_USE_STATIC_LIBS ON)
        # http://stackoverflow.com/questions/14181351/i-got-duplicate-section-errors-when-compiling-boost-regex-with-size-optimizati
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64")
        # set(Boost_USE_STATIC_RUNTIME ON)
	#set(Boost_COMPILER "-vc140")
	include_directories("C:/Program Files/GnuWin32/include")
endif ()

# regex is required for listfile in mpq module
find_package(Boost COMPONENTS system filesystem serialization program_options iostreams regex json REQUIRED)

link_directories(${Boost_LIBRARY_DIRS})
message(STATUS "Gettext include dir: ${GETTEXT_INCLUDE_DIR}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")
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
	# include binary dir since the KDE configuration class will be generated there and has to be included (also include for moc cxx files)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/editor)
endif ()

add_library(wc3libcore ${wc3lib_CORE_SRC})
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
if (JSON)
	add_subdirectory(json)
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
if (JSON)
	list(APPEND wc3lib_CORE_LIBRARIES wc3libjson)
endif ()

if (EDITOR OR USE_KIOSLAVE OR USE_QBLP OR USE_OGREBLP)
	add_subdirectory(editor) # last one
endif ()

if (GAME AND EDITOR AND BLP AND MDLX AND MPQ AND MAP AND JASS AND USE_OGREBLP)
	add_subdirectory(game) # last one
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
if (JSON)
	install(FILES json.hpp DESTINATION include/wc3lib)
endif ()

if (EDITOR)
	install(FILES editor.hpp DESTINATION include/wc3lib)
endif ()

if (GAME)
	install(FILES game.hpp DESTINATION include/wc3lib)
endif ()

if (APP)
	add_subdirectory(app)
endif ()

# install
install(TARGETS wc3libcore DESTINATION lib)
install(FILES ${wc3lib_CORE_H} DESTINATION include/wc3lib)
