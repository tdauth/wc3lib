find_package(Boost COMPONENTS unit_test_framework system REQUIRED)

add_executable(colors colors.cpp)
target_link_libraries(colors ${Boost_LIBRARIES} ${QT_LIBRARIES})
add_test(NAME ColorsTest COMMAND colors)

add_definitions(-DTEST_MPQ_URL="mpq:/${CMAKE_CURRENT_BINARY_DIR}/test.mpq")
configure_file(test.mpq test.mpq COPYONLY)
add_definitions(-DTEST_DIR_URL="file:/${CMAKE_CURRENT_BINARY_DIR}")

add_executable(mpqprioritylist mpqprioritylist.cpp)
target_link_libraries(mpqprioritylist wc3libeditor ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KDECORE_LIBS})
add_test(NAME MpqPriorityListTest COMMAND mpqprioritylist)

add_executable(shared shared.cpp)
target_link_libraries(shared wc3libeditor ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KDECORE_LIBS})
add_test(NAME SharedTest COMMAND shared)

configure_file(WorldEditStrings.txt WorldEditStrings.txt COPYONLY) # Warcraft III: Reign of Chaos war3.mpq
configure_file(war3map.wts war3map.wts COPYONLY) # Warcraft III: Reign of Chaos War Chasers
configure_file(UnitMetaData.slk UnitMetaData.slk COPYONLY) # Warcraft III: Reign of Chaos war3.mpq
add_executable(metadata metadata.cpp)
target_link_libraries(metadata wc3libcore wc3libeditor ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KDECORE_LIBS})
add_test(NAME MetaDataTest COMMAND metadata)