find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(commentstest comments.cpp)
target_link_libraries(commentstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME CommentsTest COMMAND commentstest)

add_executable(typestest types.cpp)
target_link_libraries(typestest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME TypeTest COMMAND typestest)

add_executable(globalstest globals.cpp)
target_link_libraries(globalstest wc3libjass ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME GlobalsTest COMMAND globalstest)

configure_file(comments.j comments.j COPYONLY)
configure_file(types.j types.j COPYONLY)
configure_file(globals.j globals.j COPYONLY)