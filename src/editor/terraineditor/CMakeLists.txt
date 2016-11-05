set(wc3lib_TERRAINEDITOR_H
	brush.hpp
	terrainbrush.hpp
	terraineditor.hpp
)
set(wc3lib_TERRAINEDITOR_SRC
	brush.cpp
	terrainbrush.cpp
	terraineditor.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
qt5_wrap_ui(wc3lib_TERRAINEDITOR_UI_H ${wc3lib_TERRAINEDITOR_UI})
add_library(wc3libterraineditor ${wc3lib_TERRAINEDITOR_SRC} ${wc3lib_TERRAINEDITOR_UI_H})
target_link_libraries(wc3libterraineditor wc3libeditor)

# install
install(TARGETS wc3libterraineditor DESTINATION lib)
install(FILES ${wc3lib_TERRAINEDITOR_H} ${wc3lib_TERRAINEDITOR_UI_H} DESTINATION include/wc3lib/editor/wc3libterraineditor)