if (APP AND GAME)
	# TODO Multiple definition errors on linking
	if (NOT WIN32)
		add_executable(wc3game wc3game.cpp)
        target_link_libraries(wc3game ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libgame)

		install(TARGETS wc3game DESTINATION bin)
	endif ()

	if (DEBUG)
		configure_file(ogre.cfg ogre.cfg COPYONLY)
		configure_file(plugins.cfg plugins.cfg COPYONLY)
	endif ()

	# Install all depending libraries which are not statically linked.
	# TODO For Qt5 on Windows this is a workaround since the deployment program is not available on Fedora.
	if (WIN32)
		find_package(JPEG REQUIRED)
		message(STATUS "Windows install libraries:")
		message(STATUS "JPEG libraries: ${JPEG_LIBRARIES}")
		install(FILES ${JPEG_LIBRARIES} DESTINATION bin)
		message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
		install(FILES ${Boost_LIBRARIES} DESTINATION bin)
		message(STATUS "Gettext libraries: ${GETTEXT_LIBRARIES}")
		install(FILES ${Boost_LIBRARIES} DESTINATION bin)
		get_property(Qt5_Core_LIBRARY TARGET Qt5::Core PROPERTY LOCATION)
		message(STATUS "Qt5 core libraries: ${Qt5_Core_LIBRARY}")
		install(FILES ${Qt5_Core_LIBRARY} DESTINATION bin)
		get_property(Qt5_Widgets_LIBRARY TARGET Qt5::Widgets PROPERTY LOCATION)
		message(STATUS "Qt5 widgets libraries: ${Qt5_Widgets_LIBRARY}")
		install(FILES ${Qt5_Widgets_LIBRARY} DESTINATION bin)
		get_property(Qt5_Multimedia_LIBRARY TARGET Qt5::Multimedia PROPERTY LOCATION)
		message(STATUS "Qt5 multimedia libraries: ${Qt5_Multimedia_LIBRARY}")
		install(FILES ${Qt5_Multimedia_LIBRARY} DESTINATION bin)
		#${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} Qt5::Core Qt5::Widgets Qt5::Multimedia

		find_package(ZLIB REQUIRED)
		message(STATUS "ZLIB libraries ${ZLIB_LIBRARIES}")
		install(FILES ${ZLIB_LIBRARIES} DESTINATION bin)
	endif ()
endif ()
