set(wc3lib_MPQEDITOR_UI
	ui/archiveinfodialog.ui
	ui/fileinfodialog.ui
	ui/mpqeditor.ui
	ui/creationdialog.ui
)

set(wc3lib_MPQEDITOR_H
	archiveinfodialog.hpp
	creationdialog.hpp
	mpqeditor.hpp
	mpqtreeitem.hpp
	mpqtreemodel.hpp
	mpqtreeproxymodel.hpp
	fileinfodialog.hpp
)
set(wc3lib_MPQEDITOR_SRC
	archiveinfodialog.cpp
	creationdialog.cpp
	mpqeditor.cpp
	mpqtreeitem.cpp
	mpqtreemodel.cpp
	mpqtreeproxymodel.cpp
	fileinfodialog.cpp
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories("${CMAKE_CURRENT_BINARY_DIR}") # include binary dir for moc files
qt5_wrap_ui(wc3lib_MPQEDITOR_UI_H ${wc3lib_MPQEDITOR_UI})
add_library(wc3libmpqeditor SHARED ${wc3lib_MPQEDITOR_SRC} ${wc3lib_MPQEDITOR_UI_H})
target_link_libraries(wc3libmpqeditor wc3libeditor)

# install
install(TARGETS wc3libmpqeditor DESTINATION lib)
install(FILES ${wc3lib_MPQEDITOR_H} ${wc3lib_MPQEDITOR_UI_H} DESTINATION include/wc3lib/editor/mpqeditor)