add_executable(integersumtest integersumtest.cpp)
target_link_libraries(integersumtest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME IntegerSumTest COMMAND integersumtest)