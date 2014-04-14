if (JASS)

	# http://www.guyrutenberg.com/2014/01/05/enabling-c11-c0x-in-cmake/
	# include(CheckCXXCompilerFlag)
	# CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	# CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	# if(COMPILER_SUPPORTS_CXX11)
	# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	# elseif(COMPILER_SUPPORTS_CXX0X)
	# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	# else()
	#	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	#endif()

	# use the latest Boost Spirit framework since it provides new functions (including Phoenix)
	find_package(Boost 1.54 COMPONENTS system REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})

	set (wc3lib_JASS_H
		#analyzer.hpp
		ast.hpp
		client.hpp
		grammar.hpp
		#report.hpp
	)
	set (wc3lib_JASS_SRC
		#analyzer.cpp
		grammar.cpp
		#report.cpp
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