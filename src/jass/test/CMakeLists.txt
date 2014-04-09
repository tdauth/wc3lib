find_package(Boost COMPONENTS unit_test_framework chrono REQUIRED)

add_executable(basetypestest basetypes.cpp)
target_link_libraries(basetypestest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME BaseTypesTest COMMAND basetypestest)

add_executable(commentstest comments.cpp)
target_link_libraries(commentstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CommentsTest COMMAND commentstest)

add_executable(commontest common.cpp)
target_link_libraries(commontest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CommonTest COMMAND commontest)

add_executable(expressionstest expressions.cpp)
target_link_libraries(expressionstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ExpressionsTest COMMAND expressionstest)

add_executable(functionstest functions.cpp)
target_link_libraries(functionstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME FunctionsTest COMMAND functionstest)

add_executable(stresstest stresstest.cpp)
target_link_libraries(stresstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME StressTest COMMAND stresstest)

add_executable(typestest types.cpp)
target_link_libraries(typestest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TypesTest COMMAND typestest)

add_executable(globalstest globals.cpp)
target_link_libraries(globalstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME GlobalsTest COMMAND globalstest)

configure_file(basetypes.j basetypes.j COPYONLY)
configure_file(comments.j comments.j COPYONLY)
configure_file(common.j common.j COPYONLY)
configure_file(expressions_array_references.j expressions_array_references.j COPYONLY)
configure_file(expressions_binary_operations.j expressions_binary_operations.j COPYONLY)
configure_file(expressions_binary_operators.j expressions_binary_operators.j COPYONLY)
configure_file(expressions_function_calls.j expressions_function_calls.j COPYONLY)
configure_file(expressions_literals.j expressions_literals.j COPYONLY)
configure_file(expressions_unary_operations.j expressions_unary_operations.j COPYONLY)
configure_file(expressions_unary_operators.j expressions_unary_operators.j COPYONLY)
configure_file(functions.j functions.j COPYONLY)
configure_file(stresstest.j stresstest.j COPYONLY)
configure_file(types.j types.j COPYONLY)
configure_file(globals.j globals.j COPYONLY)