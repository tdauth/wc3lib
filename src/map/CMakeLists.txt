if (MAP)
	WC3LIB_ENABLE_EXCEPTIONS()

	WC3LIB_GET_H(wc3lib_MAP_H)
	WC3LIB_GET_SRC(wc3lib_MAP_SRC)

	add_library(wc3libmap SHARED ${wc3lib_MAP_SRC})
	target_link_libraries(wc3libmap wc3libcore wc3libmpq wc3libblp ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

	# install
	install(TARGETS wc3libmap DESTINATION lib/wc3lib/)
	install(FILES ${wc3lib_MAP_H} PUBLIC_HEADER DESTINATION include/wc3lib/map)
endif ()