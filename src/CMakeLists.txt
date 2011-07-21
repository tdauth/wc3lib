WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_CORE_H)
WC3LIB_GET_SRC(wc3lib_CORE_SRC)

if (FLAG_EDITOR_USE)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/editor) # include binary dir since the KDE configuration class will be generated there and has to be included (also include for moc cxx files)
endif ()

# message(STATUS "These are the source files of core: ${wc3lib_CORE_SRC}")
add_library(wc3libcore ${wc3lib_CREATION_MODE} ${wc3lib_CORE_SRC})
if (NOT FLAG_STATIC_USE)
	# TODO: dl is required for using dlopen etc. (cross platform?)
	target_link_libraries(wc3libcore dl)
endif ()
target_link_libraries(wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

add_subdirectory(blp)
#add_subdirectory(lang)
add_subdirectory(mdlx)
add_subdirectory(mpq)
#add_subdirectory(slk)
#add_subdirectory(vm)
add_subdirectory(map)

# libraries without editor
set(wc3lib_CORE_LIBRARIES wc3libcore wc3libblp wc3libmdlx wc3libmpq wc3libmap)
#wc3libslk, wc3libvm, wc3liblang

if (FLAG_EDITOR_USE)
	add_subdirectory(editor) # last one
endif ()

# all available libraries
set(wc3lib_LIBRARIES ${wc3lib_CORE_LIBRARIES})
set(wc3lib_SRC ${wc3lib_CORE_SRC} ${wc3lib_BLP_SRC} ${wc3lib_MDLX_SRC} ${wc3lib_MPQ_SRC} ${wc3lib_MAP_SRC})
# ${wc3lib_LANG_SRC}, ${wc3lib_SLK_SRC}, ${wc3lib_VM_SRC}
if (FLAG_EDITOR_USE)
	set(wc3lib_LIBRARIES ${wc3lib_LIBRARIES} wc3libeditor)
	set(wc3lib_SRC ${wc3lib_SRC} ${wc3lib_EDITOR_SRC})
endif ()

#add_library(wc3lib SHARED ${wc3lib_SRC})
#if (NOT FLAG_STATIC_USE)
	# TODO: dl is required for using dlopen etc. (cross platform?)
#	target_link_libraries(wc3lib dl)
#endif ()
#target_link_libraries(wc3lib ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
#if (FLAG_EDITOR_USE)
#	target_link_libraries(wc3lib ${OGRE_LIBRARIES} ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS})
#endif ()

add_subdirectory(app)

# install
install(TARGETS wc3libcore DESTINATION lib/wc3lib)
#install(TARGETS wc3lib DESTINATION lib/wc3lib)
install(FILES ${wc3lib_CORE_H} DESTINATION include/wc3lib)