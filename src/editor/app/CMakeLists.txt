if (APP AND EDITOR)
	kde4_add_executable(wc3editor wc3editor.cpp)
	target_link_libraries(wc3editor ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtextureeditor wc3libmodeleditor wc3libtriggereditor wc3libobjecteditor wc3libmpqeditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS} ${OGRE_LIBRARIES})
	install(TARGETS wc3editor DESTINATION bin)

	kde4_add_executable(wc3object wc3object.cpp)
	target_link_libraries(wc3object ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libobjecteditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3object DESTINATION bin)

	kde4_add_executable(wc3texture wc3texture.cpp)
	target_link_libraries(wc3texture ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtextureeditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3texture DESTINATION bin)

	include_directories(${CMAKE_CURRENT_BINARY_DIR}/../mpqeditor)
	kde4_add_executable(wc3mpq wc3mpq.cpp)
	target_link_libraries(wc3mpq ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libmpqeditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS} ${OGRE_LIBRARIES})
	install(TARGETS wc3mpq DESTINATION bin)

	kde4_add_executable(wc3model wc3model.cpp)
	target_link_libraries(wc3model ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libmodeleditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS} ${OGRE_LIBRARIES})
	install(TARGETS wc3model DESTINATION bin)

	kde4_add_executable(wc3trigger wc3trigger.cpp)
	target_link_libraries(wc3trigger ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libtriggereditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3trigger DESTINATION bin)

	kde4_add_executable(wc3ability wc3ability.cpp)
	target_link_libraries(wc3ability ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libobjecteditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3ability DESTINATION bin)

	kde4_add_executable(wc3unit wc3unit.cpp)
	target_link_libraries(wc3unit ${wc3lib_CORE_LIBRARIES} wc3libeditor wc3libobjecteditor ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS})
	install(TARGETS wc3unit DESTINATION bin)

	if (DEBUG)
		configure_file(ogre.cfg ogre.cfg COPYONLY)
		configure_file(plugins.cfg plugins.cfg COPYONLY)
	endif ()
endif ()