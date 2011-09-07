WC3LIB_ENABLE_EXCEPTIONS()

if (EDITOR OR PLUGINS)
	include_directories(${KDE4_INCLUDE_DIR} ${QT_INCLUDE_DIR})

	include(${QT_USE_FILE})
endif ()

add_subdirectory(Plugin_BlpCodec)
add_subdirectory(kio_mpq)
add_subdirectory(qblp)

if (EDITOR)
	add_subdirectory(ui)

	WC3LIB_GET_H(wc3lib_EDITOR_H)
	WC3LIB_GET_SRC(wc3lib_EDITOR_SRC)

	kde4_add_library(wc3libeditor SHARED ${wc3lib_EDITOR_SRC}  ${wc3lib_EDITOR_KIO_MPQ_SRC}) # MpqProtocol::protocol is required
	add_dependencies(wc3libeditor UI)
	target_link_libraries(wc3libeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} qblp Plugin_BlpCodec)

	# install
	install(FILES ${wc3lib_EDITOR_H} PUBLIC_HEADER DESTINATION include/wc3lib/editor)
	install(FILES editor.kcfg DESTINATION ${KCFG_INSTALL_DIR})
endif ()