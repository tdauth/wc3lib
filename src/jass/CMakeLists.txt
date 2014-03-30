if (JASS)

	# use the latest Boost Spirit framework since it provides new functions (including Phoenix)
	find_package(Boost 1.54 COMPONENTS system REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})

	set (wc3lib_JASS_H
		ast.hpp
		client.hpp
		grammar.hpp
	)
	set (wc3lib_JASS_SRC
		grammar.cpp
	)

	add_library(wc3libjass SHARED ${wc3lib_JASS_SRC})
	target_link_libraries(wc3libjass wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
	
	if (DEBUG)
		add_subdirectory(test)
	endif ()
	
	if (JASS_LLVM)
		add_subdirectory(llvm)
	endif ()

	# install
	install(TARGETS wc3libjass DESTINATION lib)
	install(FILES ${wc3lib_JASS_H} DESTINATION include/wc3lib/jass)
endif ()