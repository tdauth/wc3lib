set(wc3lib_OBJECTEDITOR_H
	objectiddialog.hpp
	itemdata.hpp
	itemeditor.hpp
	itemtreemodel.hpp
	misceditor.hpp
	miscdata.hpp
	misctreemodel.hpp
	objecteditor.hpp
	objecteditortab.hpp
	objectdata.hpp
	objectvaluedialog.hpp
	objecttablemodel.hpp
	objecttableview.hpp
	objecttreeview.hpp
	objecttreeitem.hpp
	objecttreemodel.hpp
	objectlistwidget.hpp
	uniteditor.hpp
	unitdata.hpp
	unitselectiondialog.hpp
	unittreemodel.hpp
	weatherdata.hpp
	weathereditor.hpp
	weathertreemodel.hpp
)
set(wc3lib_OBJECTEDITOR_SRC
	objectiddialog.cpp
	itemdata.cpp
	itemeditor.cpp
	itemtreemodel.cpp
	misceditor.cpp
	miscdata.cpp
	misctreemodel.cpp
	objecteditor.cpp
	objecteditortab.cpp
	objectdata.cpp
	objectvaluedialog.cpp
	objecttablemodel.cpp
	objecttableview.cpp
	objecttreeview.cpp
	objecttreeitem.cpp
	objecttreemodel.cpp
	objectlistwidget.cpp
	uniteditor.cpp
	unitdata.cpp
	unitselectiondialog.cpp
	unittreemodel.cpp
	weatherdata.cpp
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