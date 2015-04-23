if (DEBUG)
	add_subdirectory(test)
endif ()

if (BLP AND MPQ AND MDLX AND MAP)
	include_directories("${CMAKE_CURRENT_BINARY_DIR}/../map")

	add_executable(wc3converter wc3converter.cpp)
	target_link_libraries(wc3converter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS wc3converter DESTINATION bin)
endif ()

if (EDITOR)
	find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)

	if (${QT4_FOUND})
		include(${QT_USE_FILE})
		add_definitions(${QT_DEFINITIONS})
		include_directories(${QT_INCLUDE_DIR})
	endif ()

	find_package(KDE4 REQUIRED)

	if (${KDE4_FOUND})
		add_definitions(${KDE4_DEFINITIONS})
		include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES})
		link_directories(${KDE4_LIB_DIR})
	endif ()

	# OGRE does not install the CMake file into the module path. It uses the library path instead.
	find_path(OGRE_CMAKE_MODULE_DIR FindOGRE.cmake PATHS ${CMAKE_MODULE_PATH} ${CMAKE_SYSTEM_LIBRARY_PATH} PATH_SUFFIXES OGRE/cmake)

	if (OGRE_CMAKE_MODULE_DIR)
		message(STATUS "Found OGRE CMake module in directory: ${OGRE_CMAKE_MODULE_DIR}")
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OGRE_CMAKE_MODULE_DIR})
	else ()
		message(FATAL_ERROR "Did not find OGRE CMake module.")
	endif ()

	find_package(OGRE COMPONENTS Paging Terrain REQUIRED)

	if (${OGRE_FOUND})
		include_directories(${OGRE_INCLUDE_DIRS})
		link_directories(${OGRE_LIB_DIR})
	endif ()

	# NOTE enable exception handling which might be disabled by the KDE CMake module
	# NOTE enable STL support which might be disabled by the KDE CMake module
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -UQT_NO_STL")

	kde4_add_executable(wc3editor wc3editor.cpp)
	target_link_libraries(wc3editor ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtextureeditor wc3libmodeleditor wc3libtriggereditor wc3libobjecteditor wc3libmpqeditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS} ${OGRE_LIBRARIES})
	install(TARGETS wc3editor DESTINATION bin)

	kde4_add_executable(wc3object wc3object.cpp)
	target_link_libraries(wc3object ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libobjecteditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3object DESTINATION bin)

	kde4_add_executable(wc3texture wc3texture.cpp)
	target_link_libraries(wc3texture ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtextureeditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3texture DESTINATION bin)

	include_directories("${CMAKE_CURRENT_BINARY_DIR}/../editor/mpqeditor")
	kde4_add_executable(wc3mpq wc3mpq.cpp)
	target_link_libraries(wc3mpq ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libmpqeditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS} ${OGRE_LIBRARIES})
	install(TARGETS wc3mpq DESTINATION bin)

	kde4_add_executable(wc3model wc3model.cpp)
	target_link_libraries(wc3model ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libmodeleditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS} ${OGRE_LIBRARIES})
	install(TARGETS wc3model DESTINATION bin)

	kde4_add_executable(wc3trigger wc3trigger.cpp)
	target_link_libraries(wc3trigger ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtriggereditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3trigger DESTINATION bin)

	kde4_add_executable(wc3unit wc3unit.cpp)
	target_link_libraries(wc3unit ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libobjecteditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3unit DESTINATION bin)

	if (DEBUG)
		configure_file(ogre.cfg ogre.cfg COPYONLY)
		configure_file(plugins.cfg plugins.cfg COPYONLY)
	endif ()
endif ()

if (JASS)
	add_executable(jassc jassc.cpp)
	target_link_libraries(jassc ${wc3lib_CORE_LIBRARIES} wc3libjass ${Boost_LIBRARIES})
	install(TARGETS jassc DESTINATION bin)
endif ()

if (MAP)
	#add_executable(jasstrans jasstrans.cpp)
	#target_link_libraries(jasstrans ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	#install(TARGETS jasstrans DESTINATION bin)

	add_executable(tilesetter tilesetter.cpp)
	target_link_libraries(tilesetter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS tilesetter DESTINATION bin)
endif ()

if (MPQ)
	add_executable(mpq mpq.cpp)
	target_link_libraries(mpq ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	# TODO fix StormLib version
	#if (DEBUG)
	#	target_link_libraries(mpq StormLib)
	#endif ()
	install(TARGETS mpq DESTINATION bin)
endif ()
