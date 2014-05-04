find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(teamcolors teamcolors.cpp)
target_link_libraries(teamcolors wc3libblp wc3libcore ${Boost_LIBRARIES})
add_test(NAME TeamColorsTest COMMAND teamcolors)

configure_file(TEAMCOLOR/TeamColor00.blp TeamColor00.blp COPYONLY) # Warcraft III: Reign of Chaos