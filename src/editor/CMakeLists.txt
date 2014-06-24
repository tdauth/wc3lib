if (EDITOR OR PLUGINS)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/editor")

	if (EDITOR)
		find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
	elseif (PLUGINS)
		find_package(Qt4 COMPONENTS QtCore QtGui)
	endif ()

	if (${QT4_FOUND})
		include(${QT_USE_FILE})
		include_directories(${QT_INCLUDE_DIR})
	endif ()

	if (EDITOR)
		find_package(KDE4 REQUIRED)
	elseif (PLUGINS)
		find_package(KDE4) # only for MPQ plugins
	endif ()

	if (${KDE4_FOUND})
		include(KDE4Defaults)
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
endif ()

if (BLP AND ${OGRE_FOUND})
	add_subdirectory(Plugin_BlpCodec)
endif ()
if (MPQ AND ${KDE4_FOUND})
	add_subdirectory(kio_mpq)
endif ()
if (BLP AND ${QT4_FOUND})
	add_subdirectory(qblp)
endif ()

if (EDITOR)
	set(wc3lib_EDITOR_H
		colorpalettedialog.hpp
		editor.hpp
		filemetainfo.hpp
		map.hpp
		metadata.hpp
		modelview.hpp
		modelviewsettings.hpp
		module.hpp
		modulemenu.hpp
		moduletoolbar.hpp
		mpqprioritylist.hpp
		newmapdialog.hpp
		ogreenvironment.hpp
		ogremdlx.hpp
		ogremdlxentity.hpp
		platform.hpp
		polygonmodecombobox.hpp
		renderstatswidget.hpp
		rendersystemsettings.hpp
		resource.hpp
		settings.hpp
		settingsinterface.hpp
		sourcesdialog.hpp
		splashscreen.hpp
		teamcolordialog.hpp
		texture.hpp
		viewportsettings.hpp
		warcraftiiishared.hpp
		windowsmenu.hpp
	)
	set(wc3lib_EDITOR_SRC
		colorpalettedialog.cpp
		editor.cpp
		filemetainfo.cpp
		map.cpp
		metadata.cpp
		modelview.cpp
		modelviewsettings.cpp
		module.cpp
		modulemenu.cpp
		moduletoolbar.cpp
		mpqprioritylist.cpp
		newmapdialog.cpp
		ogreenvironment.cpp
		ogremdlx.cpp
		ogremdlxentity.cpp
		renderstatswidget.cpp
		rendersystemsettings.cpp
		resource.cpp
		settings.cpp
		settingsinterface.cpp
		sourcesdialog.cpp
		splashscreen.cpp
		teamcolordialog.cpp
		texture.cpp
		viewportsettings.cpp
		warcraftiiishared.cpp
		windowsmenu.cpp
	)

	add_subdirectory(ui) # wc3lib_EDITOR_UI is set here

	include_directories(${QT_INCLUDE} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files

	kde4_add_kcfg_files(wc3lib_EDITOR_SRC editorconfig.kcfgc)
	kde4_add_ui_files(wc3lib_EDITOR_UI_H ${wc3lib_EDITOR_UI})

	# kde4_add_library
	add_library(wc3libeditor SHARED ${wc3lib_EDITOR_SRC} ${wc3lib_EDITOR_UI_H})
	# NOTE Plugin_BlpCodec qblp are required for class Texture which converts directly for Qt and OGRE
	target_link_libraries(wc3libeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} Plugin_BlpCodec qblp)
	get_target_property(LIBS wc3libeditor INTERFACE_LINK_LIBRARIES)
	message(STATUS "Var: ${LIBS} and ${INTERFACE_LINK_LIBRARIES}")
	set_target_properties(wc3libeditor PROPERTIES LINK_INTERFACE_LIBRARIES "${LIBS}")

	add_subdirectory(mapsettingseditor)
	add_subdirectory(aieditor)
	add_subdirectory(campaigneditor)
	add_subdirectory(triggereditor)
	add_subdirectory(modeleditor)
	add_subdirectory(mpqeditor)
	add_subdirectory(objecteditor)
	add_subdirectory(objectmanager)
	add_subdirectory(terraineditor)
	add_subdirectory(textureeditor)
	add_subdirectory(soundeditor)
	add_subdirectory(importmanager)

	# install
	install(TARGETS wc3libeditor DESTINATION lib)
	install(FILES ${wc3lib_EDITOR_H} ${wc3lib_EDITOR_UI_H}  DESTINATION include/wc3lib/editor)
	install(FILES editorconfig.kcfg DESTINATION ${KCFG_INSTALL_DIR})
endif ()