WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_EDITOR_PLUGIN_BLPCODEC_H)
WC3LIB_GET_SRC(wc3lib_EDITOR_PLUGIN_BLPCODEC_SRC)
set(wc3lib_EDITOR_PLUGIN_BLPCODEC_SRC ${wc3lib_EDITOR_PLUGIN_BLPCODEC_SRC} PARENT_SCOPE)

#include_directories(${OGRE_INCLUDE_DIRS})

# BLP OGRE plugin
# allows you to read BLP image files by using class Ogre::Image
add_library(Plugin_BlpCodec SHARED ${wc3lib_EDITOR_PLUGIN_BLPCODEC_SRC})
target_link_libraries(Plugin_BlpCodec wc3libblp ${Boost_LIBRARIES} OgreMain ${GETTEXT_LIBRARIES}) # ${OGRE_LIBRARIES}
# TODO drop ${Boost_LIBRARIES}
# TODO workaround
if (UNIX)
	target_link_libraries(Plugin_BlpCodec pthread)
endif ()

# install
install(TARGETS Plugin_BlpCodec DESTINATION lib/OGRE/)
install(FILES ${wc3lib_EDITOR_PLUGIN_BLPCODEC_H} PUBLIC_HEADER DESTINATION include/wc3lib/editor/Plugin_BlpCodec)