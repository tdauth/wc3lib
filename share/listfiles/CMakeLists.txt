if (MPQ)
	WC3LIB_GET_SRC(wc3lib_LISTFILES)

	install(FILES ${wc3lib_LISTFILES} DESTINATION share/wc3lib/listfiles)
endif ()