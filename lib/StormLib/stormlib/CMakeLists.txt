WC3LIB_ENABLE_EXCEPTIONS()

add_subdirectory(huffman)
add_subdirectory(misc)
add_subdirectory(pklib)
add_subdirectory(wave)

WC3LIB_GET_H(wc3lib_LIB_STORMLIB_H)
WC3LIB_GET_SRC(wc3lib_LIB_STORMLIB_SRC)

# Removing OS implementation source files.
if (APPLE)
	message(STATUS "StormLib: Is MAC OS X.")
	string(REPLACE "StormPortLinux.cpp" "" wc3lib_LIB_STORMLIB_SRC "${wc3lib_LIB_STORMLIB_SRC}")
elseif (UNIX)
	message(STATUS "StormLib: Is UNIX.")
	string(REPLACE "StormPortMac.cpp" "" wc3lib_LIB_STORMLIB_SRC "${wc3lib_LIB_STORMLIB_SRC}")
elseif (WIN32)
	message(STATUS "StormLib: Is Windows.")
	string(REPLACE "StormPortLinux.cpp" "" wc3lib_LIB_STORMLIB_SRC "${wc3lib_LIB_STORMLIB_SRC}")
	string(REPLACE "StormPortMac.cpp" "" wc3lib_LIB_STORMLIB_SRC "${wc3lib_LIB_STORMLIB_SRC}")
else ()
	message(FATAL_ERROR "StormLib: Unsupported platform. Supported platforms are Linux/UNIX, Mac OS and Windows.")
endif ()

add_library(StormLib SHARED ${wc3lib_LIB_STORMLIB_SRC} ${wc3lib_LIB_STORMLIB_HUFFMAN_SRC} ${wc3lib_LIB_STORMLIB_MISC_SRC} ${wc3lib_LIB_STORMLIB_PKLIB_SRC} ${wc3lib_LIB_STORMLIB_WAVE_SRC})
target_link_libraries(StormLib ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES})

# install
install(TARGETS StormLib DESTINATION lib/wc3lib/lib/)
install(FILES ${wc3lib_LIB_STORMLIB_H} PUBLIC_HEADER DESTINATION include/wc3lib/lib/StormLib)
