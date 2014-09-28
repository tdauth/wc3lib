find_package(Boost COMPONENTS unit_test_framework system REQUIRED)

add_executable(colors colors.cpp)
target_link_libraries(colors ${Boost_LIBRARIES} ${QT_LIBRARIES})
add_test(NAME ColorsTest COMMAND colors)

add_definitions(-DTEST_MPQ_URL="mpq:/${CMAKE_CURRENT_BINARY_DIR}/test.mpq")

add_executable(shared shared.cpp)
target_link_libraries(shared wc3libeditor ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KDECORE_LIBS})
add_test(NAME SharedTest COMMAND shared)

configure_file(test.mpq test.mpq COPYONLY)