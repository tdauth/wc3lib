WC3LIB_ENABLE_EXCEPTIONS()

if (BLP AND ${OGRE_FOUND})
	add_subdirectory(Plugin_BlpCodec)
endif ()
if (MPQ AND ${KDE4_FOUND})
	add_subdirectory(kio_mpq)
endif ()
if (BLP AND ${QT4_FOUND})
	add_subdirectory(qblp)
endif ()

if (EDITOR)
	set(wc3lib_EDITOR_H
		colorpalettedialog.hpp
		editor.hpp
		filemetainfo.hpp
		map.hpp
		metadata.hpp
		modelview.hpp
		modelviewsettings.hpp
		module.hpp
		modulemenu.hpp
		moduletoolbar.hpp
		mpqprioritylist.hpp
		newmapdialog.hpp
		ogremdlx.hpp
		ogremdlxentity.hpp
		platform.hpp
		plugin.hpp
		polygonmodecombobox.hpp
		renderstatswidget.hpp
		rendersystemsettings.hpp
		resource.hpp
		settings.hpp
		settingsinterface.hpp
		sourcesdialog.hpp
		splashscreen.hpp
		teamcolordialog.hpp
		texture.hpp
		unitmetadata.hpp
		viewportsettings.hpp
		windowsmenu.hpp
	)
	set(wc3lib_EDITOR_SRC
		colorpalettedialog.cpp
		editor.cpp
		filemetainfo.cpp
		map.cpp
		metadata.cpp
		modelview.cpp
		modelviewsettings.cpp
		module.cpp
		modulemenu.cpp
		moduletoolbar.cpp
		mpqprioritylist.cpp
		newmapdialog.cpp
		ogremdlx.cpp
		ogremdlxentity.cpp
		plugin.cpp
		renderstatswidget.cpp
		rendersystemsettings.cpp
		resource.cpp
		settings.cpp
		settingsinterface.cpp
		sourcesdialog.cpp
		splashscreen.cpp
		teamcolordialog.cpp
		texture.cpp
		unitmetadata.cpp
		viewportsettings.cpp
		windowsmenu.cpp
	)

	add_subdirectory(ui) # wc3lib_EDITOR_UI is set here
	
	include_directories(${QT_INCLUDE} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files
	
	kde4_add_kcfg_files(wc3lib_EDITOR_SRC editorconfig.kcfgc)
	kde4_add_ui_files(wc3lib_EDITOR_UI_H ${wc3lib_EDITOR_UI})
	
	kde4_add_library(wc3libeditor SHARED ${wc3lib_EDITOR_SRC} ${wc3lib_EDITOR_UI_H}  ${wc3lib_EDITOR_KIO_MPQ_SRC}) # MpqProtocol::protocol is required
	#add_dependencies(wc3libeditor UI)
	target_link_libraries(wc3libeditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} Plugin_BlpCodec qblp)
	
	add_subdirectory(aieditor)
	add_subdirectory(campaigneditor)
	add_subdirectory(triggereditor)
	add_subdirectory(modeleditor)
	add_subdirectory(mpqeditor)
	add_subdirectory(objecteditor)
	add_subdirectory(objectmanager)
	add_subdirectory(terraineditor)
	add_subdirectory(textureeditor)
	add_subdirectory(soundeditor)
	add_subdirectory(importmanager)

	# install
	install(TARGETS wc3libeditor DESTINATION lib)
	install(FILES ${wc3lib_EDITOR_H} ${wc3lib_EDITOR_UI_H}  DESTINATION include/wc3lib/editor)
	install(FILES editorconfig.kcfg DESTINATION ${KCFG_INSTALL_DIR})
endif ()