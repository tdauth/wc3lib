install(DIRECTORY man/man1 DESTINATION share/doc/man/)
install(DIRECTORY man/man5 DESTINATION share/doc/man/)

CMAKE_DEPENDENT_OPTION(DOXYGEN_HTML
"HTML output for API documentation."
ON
"DOC" OFF)
CMAKE_DEPENDENT_OPTION(DOXYGEN_LATEX
"LaTeX output for API documentation."
OFF
"DOC" OFF)
CMAKE_DEPENDENT_OPTION(DOXYGEN_RTF
"RTF output for API documentation."
OFF
"DOC" OFF)
CMAKE_DEPENDENT_OPTION(DOXYGEN_MAN
"MAN output for API documentation."
OFF
"DOC" OFF)
CMAKE_DEPENDENT_OPTION(DOXYGEN_XML
"XML output for API documentation."
OFF
"DOC" OFF)
CMAKE_DEPENDENT_OPTION(DOXYGEN_SEARCH
"Search engine support for API documentation."
ON
"DOC" OFF)

if (DOC)
	find_package(Doxygen REQUIRED)

	if (DOXYGEN_FOUND)
		# doxygen can reference external images with IMAGE_PATH, this is how we set it dynamically
		SET(CMAKE_DOXYGEN_IMAGE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/img"
		)

		# doxygen searches for source code (defined in FILE_PATTERNS, for example: *.cpp *.h)
		# with DOXYGEN_SOURCE_DIR we fill a list of directories and later we write it into
		# the Doxyfile with a REGEX REPLACE (see below)
		SET(DOXYGEN_SOURCE_DIR
		"${CMAKE_SOURCE_DIR}/src"
		)
		SET(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/api)

		# configure Doxygen options by CMake options
		if (DOXYGEN_HTML)
			set(INTERNAL_DOXYGEN_HTML YES)
		else ()
			set(INTERNAL_DOXYGEN_HTML NO)
		endif ()
		if (DOXYGEN_LATEX)
			set(INTERNAL_DOXYGEN_LATEX YES)
		else ()
			set(INTERNAL_DOXYGEN_LATEX NO)
		endif ()
		if (DOXYGEN_RTF)
			set(INTERNAL_DOXYGEN_RTF YES)
		else ()
			set(INTERNAL_DOXYGEN_RTF NO)
		endif ()
		if (DOXYGEN_MAN)
			set(INTERNAL_DOXYGEN_MAN YES)
		else ()
			set(INTERNAL_DOXYGEN_MAN NO)
		endif ()
		if (DOXYGEN_XML)
			set(INTERNAL_DOXYGEN_XML YES)
		else ()
			set(INTERNAL_DOXYGEN_XML NO)
		endif ()
		if (DOXYGEN_SEARCH)
			set(INTERNAL_DOXYGEN_SEARCH YES)
		else ()
			set(INTERNAL_DOXYGEN_SEARCH NO)
		endif ()

		CONFIGURE_FILE(doxygen.conf ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

		ADD_CUSTOM_TARGET(api ALL
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

		# install
		INSTALL(DIRECTORY "${DOXYGEN_OUTPUT_DIR}" DESTINATION usr/share/doc/wc3lib/)
		#execute_process(COMMAND ${DOXYGEN_EXECUTABLE} "doxygen.conf")
		#add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} doxygen.conf WORKING_DIRECTORY src COMMENT "Building documentation" VERBATIM)
	endif ()
endif ()

# install
#if (DOC)
#	install(DIRECTORY . DESTINATION share/doc/wc3lib/)
#endif ()