add_executable(variantstest variants.cpp)
target_link_libraries(variantstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME VariantsTest COMMAND variantstest)
