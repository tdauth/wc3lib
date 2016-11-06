if (APP AND EDITOR)
	# TODO Multiple definition errors on linking
	if (NOT WIN32)
		add_executable(wc3editor wc3editor.cpp)
		target_link_libraries(wc3editor ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtextureeditor wc3libtriggereditor wc3libobjecteditor wc3libmpqeditor)

		if (MDLX)
			target_link_libraries(wc3editor wc3libmodeleditor)
		endif ()

		install(TARGETS wc3editor DESTINATION bin)
	endif ()

	add_executable(wc3object wc3object.cpp)
	target_link_libraries(wc3object ${wc3lib_CORE_LIBRARIES} wc3libobjecteditor)
	install(TARGETS wc3object DESTINATION bin)

	add_executable(wc3texture wc3texture.cpp)
	target_link_libraries(wc3texture ${wc3lib_CORE_LIBRARIES} wc3libtextureeditor)
	install(TARGETS wc3texture DESTINATION bin)

	include_directories(${CMAKE_CURRENT_BINARY_DIR}/../mpqeditor)
	add_executable(wc3mpq wc3mpq.cpp)
	target_link_libraries(wc3mpq ${wc3lib_CORE_LIBRARIES} wc3libmpqeditor)
	install(TARGETS wc3mpq DESTINATION bin)

	if (MDLX)
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
endif ()