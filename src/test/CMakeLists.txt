find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(readtest read.cpp)
target_link_libraries(readtest wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ReadTest COMMAND readtest)

add_executable(vertextest vertex.cpp)
target_link_libraries(vertextest wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME VertexTest COMMAND vertextest)