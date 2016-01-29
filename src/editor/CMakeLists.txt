if (EDITOR OR PLUGINS)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/editor")

	include(FindOgreForwc3lib.cmake)

	if (EDITOR)
		find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
	elseif (PLUGINS)
		find_package(Qt4 COMPONENTS QtCore QtGui)
	endif ()

	if (${QT4_FOUND})
		include(${QT_USE_FILE})
		include_directories(${QT_INCLUDE_DIR})
	endif ()

	# NOTE find_package(KDE4 ... ) adds C++ compile flags such as -fvisibilit=hidden which leads to undefined reference errors.
	# You have to use KDE_EXPORT before every class declaration to prevent this or to manually set the compile flags.
	# This workaround restores the old compile flags.
	#set(CMAKE_CXX_FLAGS_OLD "${CMAKE_CXX_FLAGS}")

	if (EDITOR)
		find_package(KDE4 REQUIRED)
	elseif (PLUGINS)
		find_package(KDE4) # only for MPQ plugins
	endif ()

	if (${KDE4_FOUND})
		include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES})
		link_directories(${KDE4_LIB_DIR})
		# NOTE enable exception handling which might be disabled by the KDE CMake module
		# NOTE enable STL support which might be disabled by the KDE CMake module
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -UQT_NO_STL")
	endif ()
endif ()

message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C+ Flags: ${CMAKE_C_FLAGS}")

if ((EDITOR OR PLUGINS) AND BLP AND ${OGRE_FOUND})
	add_subdirectory(Plugin_BlpCodec)
endif ()
if ((EDITOR OR PLUGINS) AND MPQ AND ${KDE4_FOUND})
	add_subdirectory(kio_mpq)
	add_subdirectory(services)
endif ()
if ((EDITOR OR PLUGINS) AND BLP AND ${QT4_FOUND})
	add_subdirectory(qblp)
endif ()

if (EDITOR)
	set(wc3lib_EDITOR_H
		abilitydata.hpp
		collisionshape.hpp
		colorpalettedialog.hpp
		editor.hpp
		itemdata.hpp
		listfilesdialog.hpp
		map.hpp
		metadata.hpp
		miscdata.hpp
		modelview.hpp
		modelviewsettings.hpp
		module.hpp
		modulemenu.hpp
		moduletoolbar.hpp
		mpqprioritylist.hpp
		newmapdialog.hpp
		objectdata.hpp
		ogreenvironment.hpp
		ogremdlx.hpp
		ogremdlxentity.hpp
		platform.hpp
		polygonmodecombobox.hpp
		renderstatswidget.hpp
		rendersystemsettings.hpp
		resource.hpp
		root.hpp
		settings.hpp
		settingsinterface.hpp
		sharedobjectdata.hpp
		sourcesdialog.hpp
		splashscreen.hpp
		teamcolordialog.hpp
		texture.hpp
		unitdata.hpp
		viewportsettings.hpp
		warcraftiiishared.hpp
		weatherdata.hpp
		windowsmenu.hpp
	)
	set(wc3lib_EDITOR_SRC
		abilitydata.cpp
		collisionshape.cpp
		colorpalettedialog.cpp
		editor.cpp
		itemdata.cpp
		listfilesdialog.cpp
		map.cpp
		metadata.cpp
		miscdata.cpp
		modelview.cpp
		modelviewsettings.cpp
		module.cpp
		modulemenu.cpp
		moduletoolbar.cpp
		mpqprioritylist.cpp
		newmapdialog.cpp
		objectdata.cpp
		ogreenvironment.cpp
		ogremdlx.cpp
		ogremdlxentity.cpp
		renderstatswidget.cpp
		rendersystemsettings.cpp
		resource.cpp
		root.cpp
		settings.cpp
		settingsinterface.cpp
		sharedobjectdata.cpp
		sourcesdialog.cpp
		splashscreen.cpp
		teamcolordialog.cpp
		texture.cpp
		unitdata.cpp
		viewportsettings.cpp
		warcraftiiishared.cpp
		weatherdata.cpp
		windowsmenu.cpp
	)

	add_subdirectory(ui) # wc3lib_EDITOR_UI is set here

	include_directories(${QT_INCLUDE} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files

	kde4_add_kcfg_files(wc3lib_EDITOR_SRC editorconfig.kcfgc)
	kde4_add_ui_files(wc3lib_EDITOR_UI_H ${wc3lib_EDITOR_UI})

	kde4_add_library(wc3libeditor SHARED ${wc3lib_EDITOR_SRC} ${wc3lib_EDITOR_UI_H})
	# NOTE Plugin_BlpCodec qblp are required for class Texture which converts directly for Qt and OGRE
	target_link_libraries(wc3libeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} Plugin_BlpCodec qblp)

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

	add_subdirectory(mime)
	add_subdirectory(app)

	if (DEBUG)
		add_subdirectory(test)
	endif ()

	# install
	install(TARGETS wc3libeditor DESTINATION lib)
	install(FILES ${wc3lib_EDITOR_H} ${wc3lib_EDITOR_UI_H}  DESTINATION include/wc3lib/editor)
	install(FILES editorconfig.kcfg DESTINATION ${KCFG_INSTALL_DIR})
endif ()