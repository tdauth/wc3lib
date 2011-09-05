# TODO: generate API documentation
if (DOC)
	find_package(Doxygen REQUIRED)

	if (DOXYGEN_FOUND)
		#execute_process(COMMAND ${DOXYGEN_EXECUTABLE} "doxygen.conf")
		add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} doxygen.conf WORKING_DIRECTORY src COMMENT "Building documentation" VERBATIM)
	endif ()
endif ()

# install
if (DOC)
	install(DIRECTORY . DESTINATION share/doc/wc3lib/)
endif ()