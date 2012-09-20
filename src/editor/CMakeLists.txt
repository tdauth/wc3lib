WC3LIB_ENABLE_EXCEPTIONS()

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
	WC3LIB_GET_H(wc3lib_EDITOR_H)
	WC3LIB_GET_SRC(wc3lib_EDITOR_SRC) # NOTE get before generating UI!

	add_subdirectory(ui) # wc3lib_EDITOR_UI is set here
	kde4_add_ui_files(wc3lib_EDITOR_UI_H ${wc3lib_EDITOR_UI})

	kde4_add_library(wc3libeditor SHARED ${wc3lib_EDITOR_SRC} ${wc3lib_EDITOR_UI_H}  ${wc3lib_EDITOR_KIO_MPQ_SRC}) # MpqProtocol::protocol is required
	#add_dependencies(wc3libeditor UI)
	target_link_libraries(wc3libeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} Plugin_BlpCodec qblp)

	# install
	install(FILES ${wc3lib_EDITOR_H} ${wc3lib_EDITOR_UI_H}  PUBLIC_HEADER DESTINATION include/wc3lib/editor)
	install(FILES editor.kcfg DESTINATION ${KCFG_INSTALL_DIR})
endif ()