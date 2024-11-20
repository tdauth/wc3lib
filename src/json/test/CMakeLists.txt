if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif ()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(mdlxfilejsontest mdlxfiletest.cpp)
target_link_libraries(mdlxfilejsontest wc3libjson wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(MdlxFileJsonTest mdlxfilejsontest)

# MDL files are converted by MdlxConv
configure_file(Orc_Exp.mdl Orc_Exp.mdl COPYONLY)
configure_file(Orc_Exp.mdx Orc_Exp.mdx COPYONLY)