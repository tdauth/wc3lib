#smpq -SM 1 -c testattributes.mpq test.txt
#add_executable(slavetest slavetest.cpp)
kde4_add_unit_test(slavetest TESTNAME SlaveTest slavetest.cpp)
target_link_libraries(slavetest wc3libmpq wc3libcore kio_mpq ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} ${KDE4_KIO_LIBS})
#add_test(NAME AttributesTest COMMAND attributestest)

configure_file(testattributes.mpq testattributes.mpq COPYONLY) # MPQ archive with extended attributes