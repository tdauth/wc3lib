find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(colors colors.cpp)
target_link_libraries(colors wc3libblp wc3libcore ${Boost_LIBRARIES})
add_test(NAME ColorsTest COMMAND colors)