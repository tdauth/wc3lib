find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(mdlreadtest mdlreadtest.cpp)
target_link_libraries(mdlreadtest wc3libmdlx wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

add_test(MdlReadTest mdlreadtest)