find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(mdlreadtest mdlreadtest.cpp)
target_link_libraries(mdlreadtest wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(MdlReadTest mdlreadtest)

add_executable(mdxreadtest mdxreadtest.cpp)
target_link_libraries(mdxreadtest wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(MdxReadTest mdxreadtest)

configure_file(Alliance_Exp.mdl Alliance_Exp.mdl COPYONLY)
configure_file(Alliance_Exp.mdx Alliance_Exp.mdx COPYONLY)
configure_file(NightElf_Exp.mdl NightElf_Exp.mdl COPYONLY)
configure_file(NightElf_Exp.mdx NightElf_Exp.mdx COPYONLY)
configure_file(Orc_Exp.mdl Orc_Exp.mdl COPYONLY)
configure_file(Orc_Exp.mdx Orc_Exp.mdx COPYONLY)
configure_file(Undead3D_Exp.mdl Undead3D_Exp.mdl COPYONLY)
configure_file(Undead3D_Exp.mdx Undead3D_Exp.mdx COPYONLY)

add_subdirectory(mdl)