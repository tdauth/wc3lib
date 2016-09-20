#smpq -SM 1 -c testattributes.mpq test.txt
#add_executable(slavetest slavetest.cpp)
add_executable(slavetest slavetest.cpp)
target_link_libraries(slavetest wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} ${KDE4_KIO_LIBS})
add_dependencies(slavetest kio_mpq)
add_test(NAME SlaveTest COMMAND slavetest)
#add_test(NAME AttributesTest COMMAND attributestest)

# get build path of plugin "kio_mpq" and copy file to current build directory to load the plugin at runtime
message(STATUS "Path of kio_mpq: $<TARGET_FILE:kio_mpq>")
add_custom_command(TARGET slavetest
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:kio_mpq>" ${CMAKE_CURRENT_BINARY_DIR}/.
)

# add C++ definition of absolute path
add_definitions(-DKIO_NAME ${KIO_NAME})
configure_file(kio_mpqtest.h.in ${CMAKE_CURRENT_BINARY_DIR}/kiotestConfig.h)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

configure_file(testattributes.mpq testattributes.mpq COPYONLY) # MPQ archive with extended attributes