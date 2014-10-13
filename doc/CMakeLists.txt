install(DIRECTORY man/man1 DESTINATION share/man)
install(DIRECTORY man/man5 DESTINATION share/man)

# create man pages with groff binary to test man page formatting
# PostScript files can be reviewed by any supporting program
if (DEBUG)
	find_program(GROFF_BINARY groff)

	if (GROFF_BINARY)
		add_custom_target(manpages ALL
			${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man1/jassc.1 > jassc.1.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man1/mpq.1 > mpq.1.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man1/wc3converter.1 > wc3converter.1.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man1/wc3editor.1 > wc3editor.1.ps
		# man5
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man5/blp.5 > blp.5.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man5/mdl.5 > mdl.5.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man5/mdx.5 > mdx.5.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man5/mpq.5 > mpq.5.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man5/w3g.5 > w3g.5.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man5/w3n.5 > w3n.5.ps
		COMMAND ${GROFF_BINARY} -man -Tps ${CMAKE_CURRENT_SOURCE_DIR}/man/man5/w3x.5 > w3x.5.ps
		)
	endif ()
endif ()

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
		set(CMAKE_DOXYGEN_IMAGE_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/img"
		)

		# doxygen searches for source code (defined in FILE_PATTERNS, for example: *.cpp *.h)
		# with DOXYGEN_SOURCE_DIR we fill a list of directories and later we write it into
		# the Doxyfile with a REGEX REPLACE (see below)
		set(DOXYGEN_SOURCE_DIR
			"${CMAKE_SOURCE_DIR}/src"
		)
		set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/api)

		# add all include paths from CMake
		get_directory_property(OBJECTEDITOR_INCLUDE DIRECTORY ${CMAKE_SOURCE_DIR}/src/editor/objecteditor INCLUDE_DIRECTORIES)
		get_directory_property(MPQEDITOR_INCLUDE DIRECTORY ${CMAKE_SOURCE_DIR}/src/editor/mpqeditor INCLUDE_DIRECTORIES)
		get_directory_property(TRIGGEREDITOR_INCLUDE DIRECTORY ${CMAKE_SOURCE_DIR}/src/editor/triggereditor INCLUDE_DIRECTORIES)
		get_directory_property(MODELEDITOR_INCLUDE DIRECTORY ${CMAKE_SOURCE_DIR}/src/editor/modeleditor INCLUDE_DIRECTORIES)
		get_directory_property(TEXTUREEDITOR_INCLUDE DIRECTORY ${CMAKE_SOURCE_DIR}/src/editor/textureeditor INCLUDE_DIRECTORIES)
		#  ${CMAKE_INCLUDE} ${OBJECTEDITOR_INCLUDE} ${MPQEDITOR_INCLUDE} ${TRIGGEREDITOR_INCLUDE} ${MODELEDITOR_INCLUDE} ${TEXTUREEDITOR_INCLUDE}
		message(STATUS "Doxygen 1: ${TEXTUREEDITOR_INCLUDE}")
		string(REPLACE ";" ":" DOXYGEN_TMP_INCLUDE_PATH ${TEXTUREEDITOR_INCLUDE})
		message(STATUS "Doxygen include path: ${DOXYGEN_TMP_INCLUDE_PATH}")

		set(DOXYGEN_INCLUDE_PATH ${CMAKE_INCLUDE} ${OBJECTEDITOR_INCLUDE} ${MPQEDITOR_INCLUDE} ${TRIGGEREDITOR_INCLUDE} ${MODELEDITOR_INCLUDE} ${TEXTUREEDITOR_INCLUDE})
		#string(REPLACE ";" ":" DOXYGEN_INCLUDE_PATH ${DOXYGEN_INCLUDE_PATH})

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

		configure_file(doxygen.conf ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

		add_custom_target(api ALL
			${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

		# install
		install(DIRECTORY "${DOXYGEN_OUTPUT_DIR}" DESTINATION usr/share/doc/wc3lib/)
		#execute_process(COMMAND ${DOXYGEN_EXECUTABLE} "doxygen.conf")
		#add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} doxygen.conf WORKING_DIRECTORY src COMMENT "Building documentation" VERBATIM)
	endif ()
endif ()

# install
#if (DOC)
#	install(DIRECTORY . DESTINATION share/doc/wc3lib/)
#endif ()