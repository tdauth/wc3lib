set(wc3lib_MAPSETTINGSEDITOR_H
	minimapcreator.hpp
)
set(wc3lib_MAPSETTINGSEDITOR_SRC
	minimapcreator.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files

kde4_add_library(wc3libmapsettingseditor SHARED ${wc3lib_MAPSETTINGSEDITOR_SRC})
target_link_libraries(wc3libmapsettingseditor ${wc3lib_CORE_LIBRARIES} ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS} wc3libeditor)

# install
install(TARGETS wc3libmapsettingseditor DESTINATION lib)
install(FILES ${wc3lib_MAPSETTINGSEDITOR_H} DESTINATION include/wc3lib/editor/campaigneditor)