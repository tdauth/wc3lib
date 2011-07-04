#add_executable(converter converter.cpp)
#target_link_libraries(converter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
message("These are the Boost libraries: ${Boost_LIBRARIES}.")

if (FLAG_EDITOR_USE)
	kde4_add_executable(editor editor.cpp)
	target_link_libraries(editor ${wc3lib_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS})
endif ()


add_executable(converter converter.cpp)
target_link_libraries(converter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

#add_executable(jassc jassc.cpp)
#target_link_libraries(jassc ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

#add_executable(tilesetter tilesetter.cpp)
#target_link_libraries(tilesetter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

add_executable(jasstrans jasstrans.cpp)
target_link_libraries(jasstrans ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

add_executable(mpq mpq.cpp)
target_link_libraries(mpq ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
if (FLAG_DEBUG_USE)
	target_link_libraries(mpq StormLib)
endif ()

#install

#install(TARGETS converter jassc tilesetter mpq DESTINATION bin/wc3lib)
