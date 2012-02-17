WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_LANG_H)
WC3LIB_GET_SRC(wc3lib_LANG_SRC)

add_subdirectory(jass)
add_subdirectory(jasspp)
add_subdirectory(vjass)
add_subdirectory(zinc)

# library
add_library(wc3liblang ${wc3lib_LANG_SRC} ${wc3lib_LANG_JASS_SRC} ${wc3lib_LANG_JASSPP_SRC} ${wc3lib_LANG_VJASS_SRC} ${wc3lib_LANG_ZINC_SRC})
target_link_libraries(wc3liblang ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
set_property(TARGET wc3liblang PROPERTY LINK_INTERFACE_LIBRARIES "")
# todo add runtime loading
#if (${FLAGS} MATCHES "SQLITE")
#	target_link_libraries(wc3liblang sqlite3) #${SQLITE_LIBRARIES}
#endif (${FLAGS} MATCHES "SQLITE")

# install
install(TARGETS wc3liblang DESTINATION lib/wc3lib/)
install(FILES ${wc3lib_LANG_H} PUBLIC_HEADER DESTINATION include/wc3lib/lang)