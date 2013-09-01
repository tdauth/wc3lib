if (BLP)
	# Static linking with JPEG (BLP library)
	find_package(JPEG REQUIRED)
	include_directories(${JPEG_INCLUDE_DIR})

	WC3LIB_ENABLE_EXCEPTIONS()

	set(wc3lib_BLP_H
		blp.hpp
		platform.hpp
	)
	set(wc3lib_BLP_SRC
		blp.cpp
	)

	add_library(wc3libblp SHARED ${wc3lib_BLP_SRC})
	target_link_libraries(wc3libblp wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${JPEG_LIBRARIES})

	# install
	install(TARGETS wc3libblp DESTINATION lib)
	install(FILES ${wc3lib_BLP_H} DESTINATION include/wc3lib/blp)
endif ()
