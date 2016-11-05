if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif ()

find_package(Boost COMPONENTS system unit_test_framework filesystem timer REQUIRED)

if (MPQ)
	# TODO get the correct version of StormLib
	#add_executable(extraction extraction.cpp extract.cpp)
	#target_link_libraries(extraction ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES} StormLib)
	#set_property(TARGET extraction PROPERTY LINK_INTERFACE_LIBRARIES "")
endif ()

if (JASS)
	add_test(NAME JassCompilerTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../jassc --fsyntax-only -i "script.j" --headers "common.j" "Blizzard.j" )

	include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../map/)

	configure_file(jass.j jass.j COPYONLY)
	add_executable(jasstranstest jasstrans.cpp)
	target_link_libraries(jasstranstest ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	add_test(NAME JasstransTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/jasstranstest)
endif ()

if (MAP)
	add_executable(wc3transtest wc3trans_test.cpp)
	target_link_libraries(wc3transtest ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	add_test(NAME Wc3TransTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/wc3transtest)

	configure_file(war3map_de.wts war3map_de.wts COPYONLY)
	configure_file(war3map_en.wts war3map_en.wts COPYONLY)
	configure_file(war3map_untranslated_en.wts war3map_untranslated_en.wts COPYONLY)
endif ()