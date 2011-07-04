WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_CORE_H)
WC3LIB_GET_SRC(wc3lib_CORE_SRC)

message(STATUS "These are the source files of core: ${wc3lib_CORE_SRC}")
add_library(wc3libcore SHARED ${wc3lib_CORE_SRC})
# TODO: dl is required for using dlopen etc. (cross platform?)
message("Boost libraries are ${Boost_LIBRARIES}")
target_link_libraries(wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} dl)

add_subdirectory(blp)
# lang is unusable
add_subdirectory(lang)
add_subdirectory(mdlx)
add_subdirectory(mpq)
add_subdirectory(slk)
add_subdirectory(vm)
add_subdirectory(map)
message(STATUS "Is editor in use: ${FLAG_EDITOR_USE}")
if (FLAG_EDITOR_USE)
	add_subdirectory(editor) # last one
endif ()

set(wc3lib_LIBRARIES wc3libblp wc3libcore wc3libmdlx wc3libmpq wc3libslk wc3libvm wc3libmap)
# wc3liblang
if (FLAG_EDITOR_USE)
	set(wc3lib_LIBRARIES "${wc3lib_LIBRARIES} wc3libeditor")
endif ()

add_subdirectory(app)

# TODO: generate API documentation
if (BUILD_DOCUMENTATION)
	find_package(Doxygen REQUIRED)

	if (DOXYGEN_FOUND)
		#execute_process(COMMAND ${DOXYGEN_EXECUTABLE} "doxygen.conf")
	endif ()
endif ()

# install
install(TARGETS wc3libcore DESTINATION lib/wc3lib)
install(FILES ${wc3lib_CORE_H} DESTINATION include/wc3lib)
if (BUILD_DOCUMENTATION)
	install(FILES ../doc/api DESTINATION share/doc/wc3lib/api)
endif ()