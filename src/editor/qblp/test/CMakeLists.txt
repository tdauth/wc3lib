if (DEBUG)
	find_package(Qt5Test)
endif ()

set(wc3lib_EDITOR_QBLP_TEST_H blptest.hpp)
set(wc3lib_EDITOR_QBLP_TEST_SRC blptest.cpp)

add_executable(BlpTest ${wc3lib_EDITOR_QBLP_TEST_SRC})
target_link_libraries(BlpTest wc3libcore wc3libblp qblp ${Boost_LIBRARIES} Qt5::Test)
add_test(NAME BlpTests COMMAND ${CMAKE_COMMAND} -E env "QT_DEBUG_PLUGINS=1" ./BlpTest)

# get build path of plugin "qblp" and copy file to current build directory to load the plugin at runtime
add_custom_command(TARGET BlpTest
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:qblp>" ${CMAKE_CURRENT_BINARY_DIR}/.
)
add_dependencies(BlpTest qblp)

# add C++ definition of absolute path
set(QBLP_NAME "qblp")
configure_file(blptestConfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/blptestConfig.h)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# copy all test files
configure_file(DISBTNMagic.blp DISBTNMagic.blp COPYONLY) # JPEG 64x64
configure_file(TEAMCOLOR/TeamColor00.blp TeamColor00.blp COPYONLY) # Warcraft III: Reign of Chaos
configure_file(HumanCampaignCastle.blp HumanCampaignCastle.blp COPYONLY) # Warcraft III: Reign of Chaos