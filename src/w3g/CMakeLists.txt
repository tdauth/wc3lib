if (W3G)
	WC3LIB_ENABLE_EXCEPTIONS()

	set(wc3lib_W3G_H
		platform.hpp
	)
	set(wc3lib_W3G_SRC
	)

	# install
	install(FILES ${wc3lib_W3G_H} DESTINATION include/wc3lib/w3g)
endif ()