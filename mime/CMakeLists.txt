if (EDITOR)
	find_package(KDE4 REQUIRED)

	if (${KDE4_FOUND})
		add_definitions(${KDE4_DEFINITIONS})
		include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES})
		link_directories(${KDE4_LIB_DIR})
	endif ()

	find_package(SharedMimeInfo REQUIRED)

	install(FILES wc3lib.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
	update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
endif ()