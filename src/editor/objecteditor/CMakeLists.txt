set(wc3lib_OBJECTEDITOR_H
	abilityeditor.hpp
	abilitytreemodel.hpp
	buffeditor.hpp
	bufftreemodel.hpp
	destructableeditor.hpp
	destructabletreemodel.hpp
	doodadeditor.hpp
	doodadtreemodel.hpp
	objectiddialog.hpp
	itemeditor.hpp
	itemtreemodel.hpp
	misceditor.hpp
	misctreemodel.hpp
	objecteditor.hpp
	objecteditortab.hpp
	objectvaluedialog.hpp
	skineditor.hpp
	skintreemodel.hpp
	objecttablemodel.hpp
	objecttableview.hpp
	objecttreeview.hpp
	objecttreeitem.hpp
	objecttreemodel.hpp
	objecttreesortfilterproxymodel.hpp
	objectlistdialog.hpp
	uniteditor.hpp
	unitselectiondialog.hpp
	unittreemodel.hpp
	upgradeeditor.hpp
	upgradetreemodel.hpp
	watereditor.hpp
	watertreemodel.hpp
	weathereditor.hpp
	weathertreemodel.hpp
)
set(wc3lib_OBJECTEDITOR_SRC
	abilityeditor.cpp
	abilitytreemodel.cpp
	buffeditor.cpp
	bufftreemodel.cpp
	destructableeditor.cpp
	destructabletreemodel.cpp
	doodadeditor.cpp
	doodadtreemodel.cpp
	objectiddialog.cpp
	itemeditor.cpp
	itemtreemodel.cpp
	misceditor.cpp
	misctreemodel.cpp
	objecteditor.cpp
	objecteditortab.cpp
	objectvaluedialog.cpp
	skineditor.cpp
	skintreemodel.cpp
	objecttablemodel.cpp
	objecttableview.cpp
	objecttreeview.cpp
	objecttreeitem.cpp
	objecttreemodel.cpp
	objecttreesortfilterproxymodel.cpp
	objectlistdialog.cpp
	uniteditor.cpp
	unitselectiondialog.cpp
	unittreemodel.cpp
	upgradeeditor.cpp
	upgradetreemodel.cpp
	watereditor.cpp
	watertreemodel.cpp
	weathereditor.cpp
	weathertreemodel.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
qt5_wrap_ui(wc3lib_OBJECTEDITOR_UI_H ${wc3lib_OBJECTEDITOR_UI})
add_library(wc3libobjecteditor SHARED ${wc3lib_OBJECTEDITOR_SRC} ${wc3lib_OBJECTEDITOR_UI_H})
target_link_libraries(wc3libobjecteditor wc3libeditor)

add_executable(wc3objectmerger objectmerger.cpp)
target_link_libraries(wc3objectmerger wc3libeditor)

# install
install(TARGETS wc3libobjecteditor DESTINATION lib)
install(TARGETS wc3objectmerger DESTINATION bin)
install(FILES ${wc3lib_OBJECTEDITOR_H} ${wc3lib_OBJECTEDITOR_UI_H} DESTINATION include/wc3lib/editor/objecteditor)