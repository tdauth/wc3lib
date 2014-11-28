add_executable(mdlreadtestsimple mdlreadtest.cpp)
target_link_libraries(mdlreadtestsimple wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(MdlReadTestSimple mdlreadtestsimple)

configure_file(bounds.mdl bounds.mdl COPYONLY)
configure_file(version.mdl version.mdl COPYONLY)
configure_file(model.mdl model.mdl COPYONLY)
configure_file(faces.mdl faces.mdl COPYONLY) # From first Geoset in "Orc_Exp.mdl"
configure_file(groups.mdl groups.mdl COPYONLY) # From first Geoset in "Orc_Exp.mdl"