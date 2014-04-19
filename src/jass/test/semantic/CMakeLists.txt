add_executable(semanticconstantstest constants.cpp)
target_link_libraries(semanticconstantstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME SemanticConstantsTest COMMAND semanticconstantstest)

add_executable(semanticexpressiontest expressions.cpp)
target_link_libraries(semanticexpressiontest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME SemanticExpressionTest COMMAND semanticexpressiontest)


configure_file(constants_binary_operation.j constants_binary_operation.j COPYONLY)
configure_file(constants_unary_operation.j constants_unary_operation.j COPYONLY)
configure_file(expressions_unary_operation.j expressions_unary_operation.j COPYONLY)