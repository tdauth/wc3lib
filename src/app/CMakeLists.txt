if (DEBUG)
	add_subdirectory(test)
endif ()

if (BLP AND MPQ AND MDLX AND MAP)
	include_directories("${CMAKE_CURRENT_BINARY_DIR}/../map")

	add_executable(wc3converter wc3converter.cpp)
	target_link_libraries(wc3converter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS wc3converter DESTINATION bin)
endif ()

if (JASS)
	add_executable(jassc jassc.cpp)
	target_link_libraries(jassc ${wc3lib_CORE_LIBRARIES} wc3libjass ${Boost_LIBRARIES})
	install(TARGETS jassc DESTINATION bin)
endif ()

if (MAP AND JASS)
	add_executable(jasstrans jasstrans.cpp)
	target_link_libraries(jasstrans ${wc3lib_CORE_LIBRARIES} wc3libjass ${Boost_LIBRARIES})
	install(TARGETS jasstrans DESTINATION bin)
endif ()

if (MAP)
	add_executable(tilesetter tilesetter.cpp)
	target_link_libraries(tilesetter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS tilesetter DESTINATION bin)
endif ()

if (MPQ)
	add_executable(mpq mpq.cpp)
	target_link_libraries(mpq ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	# TODO fix StormLib version
	#if (DEBUG)
	#	target_link_libraries(mpq StormLib)
	#endif ()
	install(TARGETS mpq DESTINATION bin)
endif ()
