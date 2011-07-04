# TODO: generate API documentation
if (FLAG_DOC_USE)
	find_package(Doxygen REQUIRED)

	if (DOXYGEN_FOUND)
		#execute_process(COMMAND ${DOXYGEN_EXECUTABLE} "doxygen.conf")
		add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} doxygen.conf WORKING_DIRECTORY src COMMENT "Building documentation" VERBATIM)
	endif ()
endif ()

# install
if (FLAG_DOC_USE)
	install(DIRECTORY . DESTINATION share/doc/wc3lib/)
endif ()