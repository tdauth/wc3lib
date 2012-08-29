if (MDLX)
	WC3LIB_ENABLE_EXCEPTIONS()

	WC3LIB_GET_H(wc3lib_MDLX_H)
	WC3LIB_GET_SRC(wc3lib_MDLX_SRC)

	add_library(wc3libmdlx SHARED ${wc3lib_MDLX_SRC})
	target_link_libraries(wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

	# install

	install(TARGETS wc3libmdlx DESTINATION lib/wc3lib/)
	install(FILES ${wc3lib_MDLX_H} PUBLIC_HEADER DESTINATION include/wc3lib/mdlx)
endif ()