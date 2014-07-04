add_executable(mdlreadtestsimple mdlreadtest.cpp)
target_link_libraries(mdlreadtestsimple wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(MdlReadTestSimple mdlreadtestsimple)

configure_file(version.mdl version.mdl COPYONLY)