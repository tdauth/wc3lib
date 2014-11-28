if (MDLX)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/mdlx")

	set(wc3lib_MDLX_H
		attachment.hpp
		bone.hpp
		bounds.hpp
		camera.hpp
		collisionshape.hpp
		event.hpp
		faces.hpp
		geoset.hpp
		geosetanimation.hpp
		interpolator.hpp
		layer.hpp
		light.hpp
		material.hpp
		matrix.hpp
		mdlgenerator.hpp
		mdlgrammar.hpp
		mdlx.hpp
		mdlxanimatedproperties.hpp
		mdlxanimatedproperty.hpp
		mdxgrammarclient.hpp
		model.hpp
		node.hpp
		object.hpp
		particleemitter.hpp
		particleemitter2.hpp
		platform.hpp
		ribbonemitter.hpp
		sequence.hpp
		texture.hpp
		textureanimation.hpp
	)
	set(wc3lib_MDLX_SRC
		attachment.cpp
		bone.cpp
		bounds.cpp
		camera.cpp
		collisionshape.cpp
		event.cpp
		faces.cpp
		geoset.cpp
		geosetanimation.cpp
		layer.cpp
		light.cpp
		material.cpp
		matrix.cpp
		mdlgenerator.cpp
		mdlgrammar.cpp
		mdlx.cpp
		mdlxanimatedproperties.cpp
		mdlxanimatedproperty.cpp
		model.cpp
		node.cpp
		object.cpp
		particleemitter.cpp
		particleemitter2.cpp
		ribbonemitter.cpp
		sequence.cpp
		texture.cpp
		textureanimation.cpp
	)

	add_library(wc3libmdlx SHARED ${wc3lib_MDLX_SRC})
	target_link_libraries(wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

	if (DEBUG)
		add_subdirectory(test)
	endif ()

	# install

	install(TARGETS wc3libmdlx DESTINATION lib)
	install(FILES ${wc3lib_MDLX_H} DESTINATION include/wc3lib/mdlx)
endif ()