
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(txttest txt.cpp)
target_link_libraries(txttest wc3libmap wc3libcore wc3libmpq wc3libblp ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
