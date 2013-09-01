set(wc3lib_OBJECTMANAGER_H
	objectmanager.hpp
)
set(wc3lib_OBJECTMANAGER_SRC
	objectmanager.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
kde4_add_ui_files(wc3lib_OBJECTMANAGER_UI_H ${wc3lib_OBJECTMANAGER_UI})
kde4_add_library(wc3libobjectmanager SHARED ${wc3lib_OBJECTMANAGER_SRC} ${wc3lib_OBJECTMANAGER_UI_H})
target_link_libraries(wc3libobjectmanager ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} wc3libeditor)

# install
install(TARGETS wc3libobjectmanager DESTINATION lib)
install(FILES ${wc3lib_OBJECTMANAGER_H} ${wc3lib_OBJECTMANAGER_UI_H} DESTINATION include/wc3lib/editor/objectmanager)