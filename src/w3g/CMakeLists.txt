if (W3G)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/w3g")

	set(wc3lib_W3G_H
		platform.hpp
	)
	set(wc3lib_W3G_SRC
	)

	# install
	install(FILES ${wc3lib_W3G_H} DESTINATION include/wc3lib/w3g)
endif ()