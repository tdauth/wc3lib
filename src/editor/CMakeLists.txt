if (EDITOR OR USE_OGREBLP OR USE_KIOSLAVE OR USE_QBLP OR USE_MIME)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/editor")

	if (USE_OGREBLP OR MDLX)
	    # Custom CMake module to find OGRE on different platforms.
	    include(FindOgreForwc3lib.cmake)

	    # OGRE needs to detect the Windows platform, otherwise it includes the wrong files.
	    if (WIN32)
		add_definitions(-D_WIN32=1 -DOGRE_PLATFORM=1) # OGRE_PLATFORM_WIN32
		# This leads to proper DLL export markers
		add_definitions(-DOGRE_NONCLIENT_BUILD=1)
	    endif ()
	else ()
		add_definitions(-DHAS_OGRE=0)
	endif ()

	# Find includes in corresponding build directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	# Instruct CMake to run moc automatically when needed.
	set(CMAKE_AUTOMOC ON)

	# Handle .rcc files automatically
	set(CMAKE_AUTORCC ON)

	if (WIN32)
	    set (CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.0.1\\5.0.1\\msvc2010\\")
	endif ()

	# Find all Qt modules which are required
	find_package(Qt5Core REQUIRED)
	find_package(Qt5Gui REQUIRED)
	find_package(Qt5Widgets REQUIRED)
	find_package(Qt5Multimedia REQUIRED)

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
			destructabledata.hpp
			doodaddata.hpp
			editor.hpp
			itemdata.hpp
			listfilesdialog.hpp
			map.hpp
			metadata.hpp
			miscdata.hpp
			module.hpp
			modulemenu.hpp
			moduletoolbar.hpp
			mpqprioritylist.hpp
			newmapdialog.hpp
			objectdata.hpp
			platform.hpp
			resource.hpp
			resourcesdialog.hpp
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
			destructabledata.cpp
			doodaddata.cpp
			editor.cpp
			itemdata.cpp
			listfilesdialog.cpp
			map.cpp
			metadata.cpp
			miscdata.cpp
			module.cpp
			modulemenu.cpp
			moduletoolbar.cpp
			mpqprioritylist.cpp
			newmapdialog.cpp
			objectdata.cpp
			resource.cpp
			resourcesdialog.cpp
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

		if (MDLX)
			set(wc3lib_EDITOR_H
				${wc3lib_EDITOR_H}
				collisionshape.hpp
				modelview.hpp
				ogremdlx.hpp
				ogremdlxentity.hpp
				ogreenvironment.hpp
				root.hpp
				polygonmodecombobox.hpp
				renderstatswidget.hpp
			)

			set(wc3lib_EDITOR_SRC
				${wc3lib_EDITOR_SRC}
				collisionshape.cpp
				modelview.cpp
				ogremdlx.cpp
				ogremdlxentity.cpp
				ogreenvironment.cpp
				root.cpp
				renderstatswidget.cpp
			)
		endif ()

		add_subdirectory(ui) # wc3lib_EDITOR_UI is set here

		qt5_wrap_ui(wc3lib_EDITOR_UI_H ${wc3lib_EDITOR_UI})

		add_library(wc3libeditor ${wc3lib_EDITOR_SRC} ${wc3lib_EDITOR_UI_H})
		# NOTE Plugin_BlpCodec qblp are required for class Texture which converts directly for Qt and OGRE
		target_link_libraries(wc3libeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} Qt5::Core Qt5::Widgets Qt5::Multimedia)

		if (MDLX)
		    target_link_libraries(wc3libeditor ${OGRE_LIBRARIES})
		endif ()

		if (USE_QBLP)
			target_link_libraries(wc3libeditor qblp)
		endif ()

		if (USE_OGREBLP)
			target_link_libraries(wc3libeditor Plugin_BlpCodec)
		endif ()

		add_subdirectory(mapsettingseditor)
		add_subdirectory(triggereditor)

                if (MDLX)
			message(STATUS "Adding wc3libmodeleditor since MDLX is enabled.")
			add_subdirectory(modeleditor)
			add_subdirectory(terraineditor)
                endif ()

		add_subdirectory(mpqeditor)
		add_subdirectory(objecteditor)
		add_subdirectory(textureeditor)

		add_subdirectory(app)

		if (DEBUG)
			add_subdirectory(test)
		endif ()

		# install
		install(TARGETS wc3libeditor DESTINATION lib)
		install(FILES ${wc3lib_EDITOR_H} ${wc3lib_EDITOR_UI_H} DESTINATION include/wc3lib/editor)

		# Installs all required Qt binary files on Windows.
		# The program windeployqt generates all files into the binary directory which have to be installed
		if (WIN32)
			# Make sure the output directory is cleaned everytime a clean is done since there might be different DLLs when distributing the relase version.
			set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_BINARY_DIR}/qtdeployment")
			find_program(WINDEPLOYQT_PROGRAM windeployqt)

			if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
				set(WINDEPLOYQT_ARGS "--debug")
			else ()
				set(WINDEPLOYQT_ARGS "--release")
			endif ()
			add_custom_target(deployqt ALL ${WINDEPLOYQT_PROGRAM} $<TARGET_FILE:wc3libeditor> ${WINDEPLOYQT_ARGS} --dir "${CMAKE_BINARY_DIR}/qtdeployment" DEPENDS wc3libeditor)
			install(DIRECTORY "${CMAKE_BINARY_DIR}/qtdeployment/" DESTINATION lib)
		endif ()
	endif ()

endif ()
