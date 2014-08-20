
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(listfiletest listfile.cpp)
target_link_libraries(listfiletest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME ListfileTest COMMAND listfiletest)

#smpq -SM 1 -c testattributes.mpq test.txt
add_executable(attributestest attributes.cpp)
target_link_libraries(attributestest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
add_test(NAME AttributesTest COMMAND attributestest)

configure_file(testattributes.mpq testattributes.mpq COPYONLY) # MPQ archive with extended attributes
configure_file(ladik_mpq1_all_extended_attributes.mpq ladik_mpq1_all_extended_attributes.mpq COPYONLY)