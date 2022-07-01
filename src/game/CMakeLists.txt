if (GAME)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/game")

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

	if (GAME)
		set(wc3lib_GAME_H
			mainmenu.hpp
		)
		set(wc3lib_GAME_SRC
			mainmenu.cpp
		)

		add_subdirectory(ui) # wc3lib_GAME_UI is set here

		qt5_wrap_ui(wc3lib_GAME_UI_H ${wc3lib_GAME_UI})

		add_library(wc3libgame ${wc3lib_GAME_SRC} ${wc3lib_GAME_UI_H})
		# NOTE Plugin_BlpCodec qblp are required for class Texture which converts directly for Qt and OGRE
		target_link_libraries(wc3libgame ${wc3lib_CORE_LIBRARIES} ${wc3lib_EDITOR_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} Qt5::Core Qt5::Widgets Qt5::Multimedia)

		add_subdirectory(app)

		if (DEBUG)
			add_subdirectory(test)
		endif ()

		# install
		install(TARGETS wc3libgame DESTINATION lib)
		install(FILES ${wc3lib_GAME_H} ${wc3lib_GAME_UI_H} DESTINATION include/wc3lib/game)

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

			# TODO windeployqt is not available on Fedora
			#add_custom_target(deployqt ALL ${WINDEPLOYQT_PROGRAM} $<TARGET_FILE:wc3libeditor> ${WINDEPLOYQT_ARGS} --dir "${CMAKE_BINARY_DIR}/qtdeployment" DEPENDS wc3libeditor)
			#install(DIRECTORY "${CMAKE_BINARY_DIR}/qtdeployment/" DESTINATION lib)
		endif ()
	endif ()
endif ()
