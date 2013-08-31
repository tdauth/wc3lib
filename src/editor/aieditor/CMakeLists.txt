set(wc3lib_AIEDITOR_H
	aieditor.hpp
)
set(wc3lib_AIEDITOR_SRC
	aieditor.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
kde4_add_ui_files(wc3lib_AIEDITOR_UI_H ${wc3lib_AIEDITOR_UI})
kde4_add_library(wc3libaieditor SHARED ${wc3lib_AIEDITOR_SRC} ${wc3lib_AIEDITOR_UI_H})
target_link_libraries(wc3libaieditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} wc3libeditor)

# install
install(FILES ${wc3lib_AIEDITOR_H} ${wc3lib_AIEDITOR_UI_H} DESTINATION include/wc3lib/editor/aieditor)