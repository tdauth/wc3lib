# install
if (EDITOR OR PLUGINS)
	find_package(KDE4 REQUIRED)

	if (${KDE4_FOUND})
		add_definitions(${KDE4_DEFINITIONS})
		include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES})
		link_directories(${KDE4_LIB_DIR})
	endif ()

	install(FILES mpq.protocol DESTINATION ${SERVICES_INSTALL_DIR})
	install(FILES blp.desktop DESTINATION ${SERVICES_INSTALL_DIR}/qimageioplugins/)
endif ()