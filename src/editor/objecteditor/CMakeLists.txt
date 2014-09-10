set(wc3lib_OBJECTEDITOR_H
	objecteditor.hpp
	objecteditortab.hpp
	objectdata.hpp
	objectvaluedialog.hpp
	objecttablewidget.hpp
	objecttablewidgetpair.hpp
	objecttreeview.hpp
	objecttreeitem.hpp
	objecttreemodel.hpp
	uniteditor.hpp
	unitdata.hpp
	unitselectiondialog.hpp
	unittreemodel.hpp
)
set(wc3lib_OBJECTEDITOR_SRC
	objecteditor.cpp
	objecteditortab.cpp
	objectdata.cpp
	objectvaluedialog.cpp
	objecttablewidget.cpp
	objecttablewidgetpair.cpp
	objecttreeview.cpp
	objecttreeitem.cpp
	objecttreemodel.cpp
	uniteditor.cpp
	unitdata.cpp
	unitselectiondialog.cpp
	unittreemodel.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
kde4_add_ui_files(wc3lib_OBJECTEDITOR_UI_H ${wc3lib_OBJECTEDITOR_UI})
kde4_add_library(wc3libobjecteditor SHARED ${wc3lib_OBJECTEDITOR_SRC} ${wc3lib_OBJECTEDITOR_UI_H})
target_link_libraries(wc3libobjecteditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} wc3libeditor)

# install
install(TARGETS wc3libobjecteditor DESTINATION lib)
install(FILES ${wc3lib_OBJECTEDITOR_H} ${wc3lib_OBJECTEDITOR_UI_H} DESTINATION include/wc3lib/editor/objecteditor)