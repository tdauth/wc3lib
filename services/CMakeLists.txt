# install
if (EDITOR OR PLUGINS)
	install(FILES mpq.protocol DESTINATION ${SERVICES_INSTALL_DIR})
	install(FILES blp.desktop DESTINATION ${SERVICES_INSTALL_DIR}/qimageioplugins/)
endif ()