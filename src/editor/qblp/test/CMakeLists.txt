set(wc3lib_EDITOR_QBLP_TEST_H blptest.hpp)
set(wc3lib_EDITOR_QBLP_TEST_SRC blptest.cpp)

QT4_WRAP_CPP(wc3lib_EDITOR_QBLP_TEST_MOC_SRC ${wc3lib_EDITOR_QBLP_TEST_H})
add_executable(BlpTest ${wc3lib_EDITOR_QBLP_TEST_SRC} ${wc3lib_EDITOR_QBLP_TEST_MOC_SRC})
target_link_libraries(BlpTest wc3libcore wc3libblp qblp ${Boost_LIBRARIES} ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES})
add_test(NAME BlpTests COMMAND BlpTest)

# get build path of plugin "qblp" and copy file to current build directory to load the plugin at runtime
get_target_property(QBLP_NAME qblp LOCATION)
message(STATUS "Path of qblp: ${QBLP_NAME}")
add_custom_command(TARGET BlpTest
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${QBLP_NAME} ${CMAKE_CURRENT_BINARY_DIR}/.
)

# add C++ definition of absolute path
add_definitions(-DQBLP_NAME ${QBLP_NAME})
configure_file(blptestConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/blptestConfig.h)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# copy all test files
configure_file(DISBTNMagic.blp DISBTNMagic.blp COPYONLY) # JPEG 64x64
configure_file(TEAMCOLOR/TeamColor00.blp TeamColor00.blp COPYONLY) # Warcraft III: Reign of Chaos
configure_file(HumanCampaignCastle.blp HumanCampaignCastle.blp COPYONLY) # Warcraft III: Reign of Chaos