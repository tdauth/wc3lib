set(wc3lib_OBJECTEDITOR_H
	abilityeditor.hpp
	abilitytreemodel.hpp
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
	objecttablemodel.hpp
	objecttableview.hpp
	objecttreeview.hpp
	objecttreeitem.hpp
	objecttreemodel.hpp
	objectlistdialog.hpp
	uniteditor.hpp
	unitselectiondialog.hpp
	unittreemodel.hpp
	weathereditor.hpp
	weathertreemodel.hpp
)
set(wc3lib_OBJECTEDITOR_SRC
	abilityeditor.cpp
	abilitytreemodel.cpp
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
	objecttablemodel.cpp
	objecttableview.cpp
	objecttreeview.cpp
	objecttreeitem.cpp
	objecttreemodel.cpp
	objectlistdialog.cpp
	uniteditor.cpp
	unitselectiondialog.cpp
	unittreemodel.cpp
	weathereditor.cpp
	weathertreemodel.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
kde4_add_ui_files(wc3lib_OBJECTEDITOR_UI_H ${wc3lib_OBJECTEDITOR_UI})
kde4_add_library(wc3libobjecteditor SHARED ${wc3lib_OBJECTEDITOR_SRC} ${wc3lib_OBJECTEDITOR_UI_H})
target_link_libraries(wc3libobjecteditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} wc3libeditor)

# install
install(TARGETS wc3libobjecteditor DESTINATION lib)
install(FILES ${wc3lib_OBJECTEDITOR_H} ${wc3lib_OBJECTEDITOR_UI_H} DESTINATION include/wc3lib/editor/objecteditor)