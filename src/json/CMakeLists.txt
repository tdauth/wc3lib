if (JSON)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/json")

	set(wc3lib_JSON_H
		customobjects.hpp
		mdlx.hpp
	)
	set(wc3lib_JSON_SRC
		customobjects.cpp
		mdlx.cpp
	)

	add_library(wc3libjson ${wc3lib_JSON_SRC})
	target_link_libraries(wc3libjson wc3libcore wc3libmdlx wc3libmap ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} Boost::json)

	if (DEBUG)
		add_subdirectory(test)
	endif ()

	# install

	install(TARGETS wc3libjson DESTINATION lib)
	install(FILES ${wc3lib_JSON_H} DESTINATION include/wc3lib/json)
endif ()
