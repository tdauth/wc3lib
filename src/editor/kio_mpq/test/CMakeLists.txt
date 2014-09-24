#smpq -SM 1 -c testattributes.mpq test.txt
#add_executable(slavetest slavetest.cpp)
kde4_add_unit_test(slavetest TESTNAME SlaveTest slavetest.cpp)
target_link_libraries(slavetest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} ${KDE4_KIO_LIBS})
#add_test(NAME AttributesTest COMMAND attributestest)

# get build path of plugin "kio_mpq" and copy file to current build directory to load the plugin at runtime
get_target_property(KIO_NAME kio_mpq LOCATION)
message(STATUS "Path of kio_mpq: ${KIO_NAME}")
add_custom_command(TARGET BlpTest
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${KIO_NAME} ${CMAKE_CURRENT_BINARY_DIR}/.
)

# add C++ definition of absolute path
add_definitions(-DKIO_NAME ${KIO_NAME})
configure_file(kio_mpqtest.h.in ${CMAKE_CURRENT_BINARY_DIR}/kiotestConfig.h)

configure_file(testattributes.mpq testattributes.mpq COPYONLY) # MPQ archive with extended attributes