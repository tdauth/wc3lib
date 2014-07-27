add_executable(interpolationtest interpolationtest.cpp)
target_link_libraries(interpolationtest wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(InterpolationTest interpolationtest)

configure_file(geosetanim.mdl geosetanim.mdl COPYONLY)