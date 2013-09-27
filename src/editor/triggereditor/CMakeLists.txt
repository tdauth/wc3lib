set(wc3lib_TRIGGEREDITOR_H
	mapscriptwidget.hpp
	triggereditor.hpp
	triggerwidget.hpp
	variabledialog.hpp
	variablesdialog.hpp
)
set(wc3lib_TRIGGEREDITOR_SRC
	mapscriptwidget.cpp
	triggereditor.cpp
	triggerfunctiondialog.cpp
	triggerwidget.cpp
	triggertreewidgetitem.cpp
	variabledialog.cpp
	variablesdialog.cpp
)

add_subdirectory(ui)
include_directories(${QT_INCLUDE} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
kde4_add_kcfg_files(wc3lib_TRIGGEREDITOR_SRC triggereditorconfig.kcfgc)
kde4_add_ui_files(wc3lib_TRIGGEREDITOR_UI_H ${wc3lib_TRIGGEREDITOR_UI})
kde4_add_library(wc3libtriggereditor SHARED ${wc3lib_TRIGGEREDITOR_SRC} ${wc3lib_TRIGGEREDITOR_UI_H})
target_link_libraries(wc3libtriggereditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} wc3libeditor)

# install
install(TARGETS wc3libtriggereditor DESTINATION lib)
install(FILES ${wc3lib_TRIGGEREDITOR_H} ${wc3lib_TRIGGEREDITOR_UI_H} DESTINATION include/wc3lib/editor/triggereditor)
install(FILES triggereditorconfig.kcfg DESTINATION ${KCFG_INSTALL_DIR})