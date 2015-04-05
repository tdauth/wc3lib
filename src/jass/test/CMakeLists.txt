find_package(Boost COMPONENTS unit_test_framework chrono system REQUIRED)

add_executable(basetypestest basetypes.cpp)
target_link_libraries(basetypestest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME BaseTypesTest COMMAND basetypestest)

add_executable(blizzardtest blizzard.cpp)
target_link_libraries(blizzardtest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME BlizzardTest COMMAND blizzardtest)

add_executable(commentstest comments.cpp)
target_link_libraries(commentstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CommentsTest COMMAND commentstest)

add_executable(commonaitest commonai.cpp)
target_link_libraries(commonaitest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CommonAiTest COMMAND commonaitest)

add_executable(commontest common.cpp)
target_link_libraries(commontest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CommonTest COMMAND commontest)

add_executable(expressionstest expressions.cpp)
target_link_libraries(expressionstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ExpressionsTest COMMAND expressionstest)

add_executable(functionstest functions.cpp)
target_link_libraries(functionstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME FunctionsTest COMMAND functionstest)

add_executable(locationstest locations.cpp)
target_link_libraries(locationstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME LocationsTest COMMAND locationstest)

# FIXME
#add_executable(stresstest stresstest.cpp ../client.hpp ../analyzer.cpp ../grammar.cpp ../warning.cpp ../error.cpp ../report.cpp)
#wc3libjass
#target_link_libraries(stresstest wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
#add_test(NAME StressTest COMMAND stresstest)

add_executable(typestest types.cpp)
target_link_libraries(typestest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TypesTest COMMAND typestest)

add_executable(globalstest globals.cpp)
target_link_libraries(globalstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME GlobalsTest COMMAND globalstest)

add_executable(localstest locals.cpp)
target_link_libraries(localstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME LocalsTest COMMAND localstest)

add_executable(statementstest statements.cpp)
target_link_libraries(statementstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME StatementsTest COMMAND statementstest)

add_executable(statementserrorstest statements_errors.cpp)
target_link_libraries(statementserrorstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME StatementsErrorsTest COMMAND statementserrorstest)

configure_file(basetypes.j basetypes.j COPYONLY)
configure_file(Blizzard.j Blizzard.j COPYONLY)
configure_file(comments.j comments.j COPYONLY)
configure_file(common.ai common.ai COPYONLY)
configure_file(common.j common.j COPYONLY)
configure_file(expressions_array_references.j expressions_array_references.j COPYONLY)
configure_file(expressions_binary_operations.j expressions_binary_operations.j COPYONLY)
configure_file(expressions_binary_operators.j expressions_binary_operators.j COPYONLY)
configure_file(expressions_function_calls.j expressions_function_calls.j COPYONLY)
configure_file(expressions_function_refs.j expressions_function_refs.j COPYONLY)
configure_file(expressions_literals.j expressions_literals.j COPYONLY)
configure_file(expressions_unary_operations.j expressions_unary_operations.j COPYONLY)
configure_file(expressions_unary_operators.j expressions_unary_operators.j COPYONLY)
configure_file(functions.j functions.j COPYONLY)
configure_file(stresstest.j stresstest.j COPYONLY)
configure_file(types.j types.j COPYONLY)
configure_file(globals.j globals.j COPYONLY)
configure_file(locals.j locals.j COPYONLY)
configure_file(statements_debug.j statements_debug.j COPYONLY)
configure_file(statements_set.j statements_set.j COPYONLY)
configure_file(statements_set_errors.j statements_set_errors.j COPYONLY)
configure_file(statements_call.j statements_call.j COPYONLY)
configure_file(statements_ifthenelse.j statements_ifthenelse.j COPYONLY)
configure_file(statements_loop.j statements_loop.j COPYONLY)
configure_file(statements_exitwhen.j statements_exitwhen.j COPYONLY)
configure_file(statements_return.j statements_return.j COPYONLY)

add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(semantic)