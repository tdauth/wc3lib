set(wc3lib_MAPSETTINGSEDITOR_H
	minimapcreator.hpp
)
set(wc3lib_MAPSETTINGSEDITOR_SRC
	minimapcreator.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}) # include binary dir for moc files

add_library(wc3libmapsettingseditor SHARED ${wc3lib_MAPSETTINGSEDITOR_SRC})
target_link_libraries(wc3libmapsettingseditor wc3libeditor qblp)

# install
install(TARGETS wc3libmapsettingseditor DESTINATION lib)
install(FILES ${wc3lib_MAPSETTINGSEDITOR_H} DESTINATION include/wc3lib/editor/campaigneditor)