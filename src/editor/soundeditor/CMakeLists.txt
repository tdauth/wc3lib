set(wc3lib_SOUNDEDITOR_H
	soundeditor.hpp
)
set(wc3lib_SOUNDEDITOR_SRC
	soundeditor.cpp
)

add_subdirectory(ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
kde4_add_ui_files(wc3lib_SOUNDEDITOR_UI_H ${wc3lib_SOUNDEDITOR_UI})
kde4_add_library(wc3libsoundeditor SHARED ${wc3lib_SOUNDEDITOR_SRC} ${wc3lib_SOUNDEDITOR_UI_H})
target_link_libraries(wc3libsoundeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} wc3libeditor)

# install
install(FILES ${wc3lib_SOUNDEDITOR_H} ${wc3lib_SOUNDEDITOR_UI_H} DESTINATION include/wc3lib/editor/soundeditor)