add_executable(wc3converter converter.cpp)
target_link_libraries(wc3converter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

if (EDITOR)
	kde4_add_executable(wc3editor editor.cpp)
	target_link_libraries(wc3editor ${wc3lib_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS})
endif ()

#add_executable(jassc jassc.cpp)
#target_link_libraries(jassc ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

#add_executable(tilesetter tilesetter.cpp)
#target_link_libraries(tilesetter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

add_executable(jasstrans jasstrans.cpp)
target_link_libraries(jasstrans ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

add_executable(mpq mpq.cpp)
target_link_libraries(mpq ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
if (DEBUG)
	target_link_libraries(mpq StormLib)
endif ()

# install
if (EDITOR)
	install(TARGETS wc3editor DESTINATION bin)
endif ()

install(TARGETS wc3converter mpq DESTINATION bin) #jassc, tilesetter
