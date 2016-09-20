if (EDITOR OR USE_OGREBLP OR USE_KIOSLAVE OR USE_QBLP OR USE_MIME)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/editor")

	# Custom CMake module to find OGRE on different platforms.
	include(FindOgreForwc3lib.cmake)

	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)

	# Handle .rcc files automatically
	set(CMAKE_AUTORCC ON)

	# Find all Qt modules which are required
	find_package(Qt5Core)
	find_package(Qt5Widgets)
	find_package(Qt5Multimedia)
	find_package(Qt5LinguistTools)

	if (DEBUG)
		find_package(Qt5Test)
	endif ()

	# NOTE find_package(KDE4 ... ) adds C++ compile flags such as -fvisibilit=hidden which leads to undefined reference errors.
	# You have to use KDE_EXPORT before every class declaration to prevent this or to manually set the compile flags.
	# This workaround restores the old compile flags.
	#set(CMAKE_CXX_FLAGS_OLD "${CMAKE_CXX_FLAGS}")

	message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS}")
	message(STATUS "C+ Flags: ${CMAKE_C_FLAGS}")

	if (USE_OGREBLP)
		add_subdirectory(Plugin_BlpCodec)
	endif ()
	if (USE_KIOSLAVE)
		add_subdirectory(kio_mpq)
		add_subdirectory(services)
	endif ()
	if (USE_QBLP)
		add_subdirectory(qblp)
	endif ()
	if (USE_MIME)
		add_subdirectory(mime)
	endif ()

	if (EDITOR)
		set(wc3lib_EDITOR_H
			abilitydata.hpp
			buffdata.hpp
			collisionshape.hpp
			destructabledata.hpp
			doodaddata.hpp
			editor.hpp
			itemdata.hpp
			listfilesdialog.hpp
			map.hpp
			metadata.hpp
			miscdata.hpp
			modelview.hpp
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
			resource.hpp
			resourcesdialog.hpp
			root.hpp
			settings.hpp
			settingsinterface.hpp
			sharedobjectdata.hpp
			skindata.hpp
			sourcesdialog.hpp
			splashscreen.hpp
			teamcolordialog.hpp
			texture.hpp
			unitdata.hpp
			upgradedata.hpp
			warcraftiiishared.hpp
			waterdata.hpp
			weatherdata.hpp
			windowsmenu.hpp
		)
		set(wc3lib_EDITOR_SRC
			abilitydata.cpp
			buffdata.cpp
			collisionshape.cpp
			destructabledata.cpp
			doodaddata.cpp
			editor.cpp
			itemdata.cpp
			listfilesdialog.cpp
			map.cpp
			metadata.cpp
			miscdata.cpp
			modelview.cpp
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
			resource.cpp
			resourcesdialog.cpp
			root.cpp
			settings.cpp
			settingsinterface.cpp
			sharedobjectdata.cpp
			skindata.cpp
			sourcesdialog.cpp
			splashscreen.cpp
			teamcolordialog.cpp
			texture.cpp
			unitdata.cpp
			upgradedata.cpp
			warcraftiiishared.cpp
			waterdata.cpp
			weatherdata.cpp
			windowsmenu.cpp
		)

		add_subdirectory(ui) # wc3lib_EDITOR_UI is set here

		qt5_wrap_ui(wc3lib_EDITOR_UI_H ${wc3lib_EDITOR_UI})

		add_library(wc3libeditor SHARED ${wc3lib_EDITOR_SRC} ${wc3lib_EDITOR_UI_H})
		# NOTE Plugin_BlpCodec qblp are required for class Texture which converts directly for Qt and OGRE
		target_link_libraries(wc3libeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} Qt5::Core Qt5::Widgets Qt5::Multimedia Plugin_BlpCodec qblp)

		add_subdirectory(mapsettingseditor)
		add_subdirectory(triggereditor)
		add_subdirectory(modeleditor)
		add_subdirectory(mpqeditor)
		add_subdirectory(objecteditor)
		add_subdirectory(terraineditor)
		add_subdirectory(textureeditor)

		add_subdirectory(app)

		if (DEBUG)
			add_subdirectory(test)
		endif ()

		# install
		install(TARGETS wc3libeditor DESTINATION lib)
		install(FILES ${wc3lib_EDITOR_H} ${wc3lib_EDITOR_UI_H} DESTINATION include/wc3lib/editor)
	endif ()

endif ()