if (MAP)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/map")

	set (wc3lib_MAP_H
		camera.hpp
		cameras.hpp
		campaign.hpp
		campaigninfo.hpp
		customobjects.hpp
		customobjectscollection.hpp
		customtexttriggers.hpp
		customunits.hpp
		environment.hpp
		importedfiles.hpp
		info.hpp
		itemset.hpp
		mapstrings.hpp
		menuminimap.hpp
		minimap.hpp
		pathmap.hpp
		platform.hpp
		playable.hpp
		rect.hpp
		rects.hpp
		shadow.hpp
		slk.hpp
		sound.hpp
		sounds.hpp
		tilepoint.hpp
		tree.hpp
		trees.hpp
		trigger.hpp
		triggercategory.hpp
		triggerdata.hpp
		triggerdataex.hpp
		triggerfunction.hpp
		triggerfunctionparameter.hpp
		triggers.hpp
		triggerstrings.hpp
		txt.hpp
		unitanditem.hpp
		unitsanditems.hpp
		value.hpp
		variable.hpp
		w3m.hpp
		w3x.hpp
	)
	set (wc3lib_MAP_SRC
		camera.cpp
		cameras.cpp
		campaign.cpp
		campaigninfo.cpp
		customobjects.cpp
		customobjectscollection.cpp
		customtexttriggers.cpp
		customunits.cpp
		environment.cpp
		importedfiles.cpp
		info.cpp
		mapstrings.cpp
		menuminimap.cpp
		minimap.cpp
		pathmap.cpp
		rect.cpp
		rects.cpp
		shadow.cpp
		slk.cpp
		sound.cpp
		sounds.cpp
		tilepoint.cpp
		tree.cpp
		trees.cpp
		trigger.cpp
		triggercategory.cpp
		triggerdata.cpp
		triggerdataex.cpp
		triggerfunction.cpp
		triggerfunctionparameter.cpp
		triggers.cpp
		triggerstrings.cpp
		txt.cpp
		unitanditem.cpp
		unitsanditems.cpp
		value.cpp
		variable.cpp
		w3m.cpp
	)

	add_library(wc3libmap ${wc3lib_MAP_SRC})
	# Fixes Assembler file too big message on Windows
	if (WIN32)
	    target_compile_options(wc3libmap BEFORE PUBLIC "-Wa,-mbig-obj")
	endif ()
	target_link_libraries(wc3libmap wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

	if (MPQ)
		target_link_libraries(wc3libmap wc3libmpq)
	endif ()

	if (BLP)
		target_link_libraries(wc3libmap wc3libblp)
	endif ()

	if (DEBUG)
		add_subdirectory(test)
	endif ()

	# install
	install(TARGETS wc3libmap DESTINATION lib)
	install(FILES ${wc3lib_MAP_H} DESTINATION include/wc3lib/map)
endif ()
