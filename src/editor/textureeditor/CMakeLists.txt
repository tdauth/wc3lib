set(wc3lib_TEXTUREEDITOR_H
	textureeditor.hpp
)
set(wc3lib_TEXTUREEDITOR_SRC
	textureeditor.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
qt5_wrap_ui(wc3lib_TEXTUREEDITOR_UI_H ${wc3lib_TEXTUREEDITOR_UI})
add_library(wc3libtextureeditor SHARED ${wc3lib_TEXTUREEDITOR_SRC} ${wc3lib_TEXTUREEDITOR_UI_H})
target_link_libraries(wc3libtextureeditor wc3libeditor qblp)

# install
install(TARGETS wc3libtextureeditor DESTINATION lib)
install(FILES ${wc3lib_TEXTUREEDITOR_H} ${wc3lib_TEXTUREEDITOR_UI_H} DESTINATION include/wc3lib/editor/wc3libtextureeditor)