find_package(Boost COMPONENTS system unit_test_framework filesystem timer REQUIRED)

if (MPQ)
	# TODO get the correct version of StormLib
	#add_executable(extraction extraction.cpp extract.cpp)
	#target_link_libraries(extraction ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES} StormLib)
	#set_property(TARGET extraction PROPERTY LINK_INTERFACE_LIBRARIES "")
endif ()

if (JASS)
	add_test(NAME JassCompilerTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../jassc --fsyntax-only --headers "common.j" "Blizzard.j" -i "script.j" )
	
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../map/)
	
	configure_file(jass.j jass.j COPYONLY)
	add_executable(jasstranstest jasstrans.cpp)
	target_link_libraries(jasstranstest ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	add_test(NAME JasstransTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/jasstranstest)
endif ()