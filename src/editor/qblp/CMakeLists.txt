set(wc3lib_EDITOR_QBLP_H
	blpiohandler.hpp
	blpioplugin.hpp
)
set(wc3lib_EDITOR_QBLP_SRC
	blpiohandler.cpp
	blpioplugin.cpp
)
set(wc3lib_EDITOR_QBLP_SRC ${wc3lib_EDITOR_QBLP_SRC} PARENT_SCOPE)

add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

# BLP Qt plugin
# allows you to read and write BLP image files by using class QImage and image type "BLP"
add_library(qblp SHARED ${wc3lib_EDITOR_QBLP_SRC})
target_link_libraries(qblp wc3libcore wc3libblp ${Boost_LIBRARIES} ${QT_LIBRARIES})

# wc3libcore is necessary because of catching Exception
# TODO drop ${Boost_LIBRARIES}

if (DEBUG)
	add_subdirectory(test)
endif ()

# install
install(TARGETS qblp DESTINATION lib)
install(TARGETS qblp DESTINATION ${QT_PLUGINS_DIR}/imageformats)
install(FILES ${wc3lib_EDITOR_QBLP_H} DESTINATION include/wc3lib/editor/qblp)