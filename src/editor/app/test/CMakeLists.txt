if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif ()

find_package(Boost COMPONENTS system unit_test_framework REQUIRED)

message(STATUS "Using Warcraft III directory: \"$ENV{WC3_DIR}\"")

if ("$ENV{WC3_DIR}" STREQUAL "")
	message(WARNING "Warcraft III directory is not defined. Some Unit Tests may not work.")
endif ()

add_definitions(-DWC3_DIR="$ENV{WC3_DIR}")

add_executable(wc3objectvalidate_test wc3objectvalidate_test.cpp)
target_link_libraries(wc3objectvalidate_test ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES} wc3libobjecteditor)
add_test(NAME Wc3ObjectValidateTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/wc3objectvalidate_test)

configure_file(war3map_de.wts war3map_de.wts COPYONLY)
configure_file(ObjectData.w3o ObjectData.w3o COPYONLY)