if (BLP)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/blp")

	# Static linking with JPEG (BLP library)
	find_package(JPEG REQUIRED)
	include_directories(${JPEG_INCLUDE_DIR})

	set(wc3lib_BLP_H
		blp.hpp
		jpeg.hpp
		platform.hpp
	)
	set(wc3lib_BLP_SRC
		blp.cpp
		jpeg.cpp
	)

        add_library(wc3libblp ${wc3lib_BLP_SRC})
        message(STATUS "JPEG libraries: ${JPEG_LIBRARIES}")
        target_link_libraries(wc3libblp wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${JPEG_LIBRARIES})

	if (DEBUG)
		add_subdirectory(test)
	endif ()

	# install
	install(TARGETS wc3libblp DESTINATION lib)
	install(FILES ${wc3lib_BLP_H} DESTINATION include/wc3lib/blp)
endif ()
