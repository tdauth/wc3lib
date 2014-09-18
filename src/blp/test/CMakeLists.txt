find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(teamcolors teamcolors.cpp)
target_link_libraries(teamcolors wc3libblp wc3libcore ${Boost_LIBRARIES})
add_test(NAME TeamColorsTest COMMAND teamcolors)

add_executable(jpeg jpeg.cpp)
target_link_libraries(jpeg wc3libblp wc3libcore ${Boost_LIBRARIES})
add_test(NAME JpegTest COMMAND jpeg)

add_executable(paletted paletted.cpp)
target_link_libraries(paletted wc3libblp wc3libcore ${Boost_LIBRARIES})
add_test(NAME Paletted COMMAND paletted)

configure_file(TEAMCOLOR/TeamColor00.blp TeamColor00.blp COPYONLY) # Warcraft III: Reign of Chaos
# BLP 1 paletted + alpha
configure_file(HumanCampaignCastle.blp HumanCampaignCastle.blp COPYONLY) # Warcraft III: Reign of Chaos