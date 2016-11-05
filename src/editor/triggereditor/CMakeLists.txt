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
qt5_wrap_ui(wc3lib_TRIGGEREDITOR_UI_H ${wc3lib_TRIGGEREDITOR_UI})
add_library(wc3libtriggereditor ${wc3lib_TRIGGEREDITOR_SRC} ${wc3lib_TRIGGEREDITOR_UI_H})
target_link_libraries(wc3libtriggereditor wc3libeditor)

# install
install(TARGETS wc3libtriggereditor DESTINATION lib)
install(FILES ${wc3lib_TRIGGEREDITOR_H} ${wc3lib_TRIGGEREDITOR_UI_H} DESTINATION include/wc3lib/editor/triggereditor)