if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif ()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

message(STATUS "Unit test Boost libraries: ${Boost_LIBRARIES}")

add_executable(readtest read.cpp)
target_link_libraries(readtest wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ReadTest COMMAND readtest)

add_executable(vertextest vertex.cpp)
target_link_libraries(vertextest wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME VertexTest COMMAND vertextest)
