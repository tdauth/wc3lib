find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(archivetest archive.cpp)
target_link_libraries(archivetest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ArchiveTest COMMAND archivetest)

add_executable(listfiletest listfile.cpp)
target_link_libraries(listfiletest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ListfileTest COMMAND listfiletest)

#smpq -SM 1 -c testattributes.mpq test.txt
add_executable(attributestest attributes.cpp)
target_link_libraries(attributestest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME AttributesTest COMMAND attributestest)

message(STATUS "Using Warcraft III directory: \"$ENV{WC3_DIR}\"")

if ("$ENV{WC3_DIR}" STREQUAL "")
	message(WARNING "Warcraft III directory is not defined. Some Unit Tests may not work.")
endif ()

add_definitions(-DWC3_DIR="$ENV{WC3_DIR}")

# TODO find StormLib with a CMake module
find_file(STORMLIB_HEADER "StormLib.h")
find_file(STORMLIB_PORT_HEADER "StormPort.h")

if (STORMLIB_HEADER AND STORMLIB_PORT_HEADER)
	add_executable(performancetest performance.cpp)
	target_link_libraries(performancetest wc3libmpq wc3libcore storm ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
	add_test(NAME PerformanceTest COMMAND performancetest)
endif ()

add_executable(extracttest extract.cpp)
target_link_libraries(extracttest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ExtractTest COMMAND extracttest)

configure_file(testattributes.mpq testattributes.mpq COPYONLY) # MPQ archive with extended attributes
configure_file(ladik_mpq1_all_extended_attributes.mpq ladik_mpq1_all_extended_attributes.mpq COPYONLY)
configure_file(mpqmaster_mpq1_no_extended_attributes.mpq mpqmaster_mpq1_no_extended_attributes.mpq COPYONLY)