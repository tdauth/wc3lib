WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_UI(wc3lib_EDITOR_UI)
#message(STATUS "UI files:")
#foreach (bla IN LISTS wc3lib_EDITOR_UI)
#	message(STATUS ${bla})
#endforeach ()
#set(wc3lib_EDITOR_UI ${wc3lib_EDITOR_UI} PARENT_SCOPE)

#kde4_add_ui_files(wc3lib_EDITOR_UI_SRCS ${wc3lib_EDITOR_UI})
# FIXME workaround if files aren't generated
#list(LENGTH "${wc3lib_EDITOR_UI_SRCS}" length)
#if (${length} EQUAL 0)
add_custom_target(UI sh generateui.sh WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
#endif ()
#set(wc3lib_EDITOR_UI_SRCS ${wc3lib_EDITOR_UI_SRCS} PARENT_SCOPE)

# install
install(FILES ${wc3lib_EDITOR_UI} PUBLIC_HEADER DESTINATION include/wc3lib/editor/ui)
install(FILES ${wc3lib_EDITOR_UI_SRCS} PUBLIC_HEADER DESTINATION include/wc3lib/editor/ui)