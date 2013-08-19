if (MAP)
	WC3LIB_ENABLE_EXCEPTIONS()

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
		metadata.hpp
		minimap.hpp
		object.hpp
		pathmap.hpp
		platform.hpp
		playable.hpp
		rect.hpp
		rects.hpp
		shadow.hpp
		sound.hpp
		sounds.hpp
		string.hpp
		strings.hpp
		tilepoint.hpp
		tree.hpp
		trees.hpp
		trigger.hpp
		triggercategory.hpp
		triggerdata.hpp
		triggerfunction.hpp
		triggerfunctionparameter.hpp
		triggers.hpp
		triggerstrings.hpp
		txt.hpp
		value.hpp
		variable.hpp
		w3g.hpp
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
		object.cpp
		pathmap.cpp
		rect.cpp
		rects.cpp
		shadow.cpp
		sound.cpp
		sounds.cpp
		strings.cpp
		tilepoint.cpp
		tree.cpp
		trees.cpp
		trigger.cpp
		triggercategory.cpp
		triggerdata.cpp
		triggerfunction.cpp
		triggerfunctionparameter.cpp
		triggers.cpp
		triggerstrings.cpp
		txt.cpp
		value.cpp
		variable.cpp
		w3g.cpp
		w3m.cpp
	)

	add_library(wc3libmap SHARED ${wc3lib_MAP_SRC})
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
	install(TARGETS wc3libmap DESTINATION lib/wc3lib/)
	install(FILES ${wc3lib_MAP_H} PUBLIC_HEADER DESTINATION include/wc3lib/map)
endif ()