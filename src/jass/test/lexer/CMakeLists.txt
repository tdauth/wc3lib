#add_executable(lexertest lexer.cpp)
#target_link_libraries(lexertest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
#add_test(NAME LexerTest COMMAND lexertest)

configure_file(lexer.j lexer.j COPYONLY)