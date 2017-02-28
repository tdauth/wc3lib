if (APP AND EDITOR)
	# TODO Multiple definition errors on linking
	if (NOT WIN32)
		add_executable(wc3editor wc3editor.cpp)
		target_link_libraries(wc3editor ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtextureeditor wc3libtriggereditor wc3libobjecteditor wc3libmpqeditor)

		if (MDLX AND USE_OGREBLP)
			target_link_libraries(wc3editor wc3libmodeleditor)
		endif ()

		install(TARGETS wc3editor DESTINATION bin)
	endif ()

	add_executable(wc3object wc3object.cpp)
	target_link_libraries(wc3object ${wc3lib_CORE_LIBRARIES} wc3libobjecteditor)
	install(TARGETS wc3object DESTINATION bin)

	add_executable(wc3objectvalidate wc3objectvalidate.cpp)
	target_link_libraries(wc3objectvalidate ${wc3lib_CORE_LIBRARIES} wc3libobjecteditor)
	install(TARGETS wc3objectvalidate DESTINATION bin)

	add_executable(wc3texture wc3texture.cpp)
	target_link_libraries(wc3texture ${wc3lib_CORE_LIBRARIES} wc3libtextureeditor)
	install(TARGETS wc3texture DESTINATION bin)

	include_directories(${CMAKE_CURRENT_BINARY_DIR}/../mpqeditor)
	add_executable(wc3mpq wc3mpq.cpp)
	target_link_libraries(wc3mpq ${wc3lib_CORE_LIBRARIES} wc3libmpqeditor)
	install(TARGETS wc3mpq DESTINATION bin)

	if (MDLX AND USE_OGREBLP)
		add_executable(wc3model wc3model.cpp)
		target_link_libraries(wc3model ${wc3lib_CORE_LIBRARIES} wc3libmodeleditor)
		install(TARGETS wc3model DESTINATION bin)
	endif ()

	add_executable(wc3trigger wc3trigger.cpp)
	target_link_libraries(wc3trigger ${wc3lib_CORE_LIBRARIES} wc3libtriggereditor)
	install(TARGETS wc3trigger DESTINATION bin)

	add_executable(wc3ability wc3ability.cpp)
	target_link_libraries(wc3ability ${wc3lib_CORE_LIBRARIES} wc3libobjecteditor)
	install(TARGETS wc3ability DESTINATION bin)

	add_executable(wc3unit wc3unit.cpp)
	target_link_libraries(wc3unit ${wc3lib_CORE_LIBRARIES} wc3libobjecteditor)
	install(TARGETS wc3unit DESTINATION bin)

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

	add_subdirectory(test)
endif ()