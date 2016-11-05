set(wc3lib_MODELEDITOR_H
	modeleditor.hpp
	modeleditorsettings.hpp
	modeleditorview.hpp
)
set(wc3lib_MODELEDITOR_SRC
	modeleditor.cpp
	modeleditorsettings.cpp
	modeleditorview.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
qt5_wrap_ui(wc3lib_MODELEDITOR_UI_H ${wc3lib_MODELEDITOR_UI})
add_library(wc3libmodeleditor ${wc3lib_MODELEDITOR_SRC} ${wc3lib_MODELEDITOR_UI_H})
target_link_libraries(wc3libmodeleditor wc3libeditor)

# install
install(TARGETS wc3libmodeleditor DESTINATION lib)
install(FILES ${wc3lib_MODELEDITOR_H} ${wc3lib_MODELEDITOR_UI_H} DESTINATION include/wc3lib/editor/modeleditor)