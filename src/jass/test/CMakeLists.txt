find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(literalstest literals.cpp)
target_link_libraries(literalstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME LiteralsTest COMMAND literalstest)

add_executable(basetypestest basetypes.cpp)
target_link_libraries(basetypestest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME BaseTypesTest COMMAND basetypestest)

add_executable(commentstest comments.cpp)
target_link_libraries(commentstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CommentsTest COMMAND commentstest)

add_executable(typestest types.cpp)
target_link_libraries(typestest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TypesTest COMMAND typestest)

add_executable(globalstest globals.cpp)
target_link_libraries(globalstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME GlobalsTest COMMAND globalstest)

configure_file(literals.j literals.j COPYONLY)
configure_file(basetypes.j basetypes.j COPYONLY)
configure_file(comments.j comments.j COPYONLY)
configure_file(types.j types.j COPYONLY)
configure_file(globals.j globals.j COPYONLY)