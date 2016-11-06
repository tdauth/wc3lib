if (MPQ)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/mpq")

	set(wc3lib_MPQ_H
		algorithm.hpp
		archive.hpp
		attributes.hpp
		block.hpp
		file.hpp
		hash.hpp
		listfile.hpp
		platform.hpp
		sector.hpp
		signature.hpp
		test.hpp
	)
	set(wc3lib_MPQ_SRC
		algorithm.cpp
		archive.cpp
		attributes.cpp
		block.cpp
		file.cpp
		hash.cpp
		listfile.cpp
		sector.cpp
		signature.cpp
		test.cpp
	)

	# Both compression libraries are required by Boost "iostreams" component which is used for the "mpq" module.
	# Crypto++ library is required for RSA encryption and SHA1 digest and MD5 checksum calculation (Boost does only support CRC).
	find_package(BZip2 REQUIRED)
	include_directories(${BZIP2_INCLUDE_DIR})
	find_package(ZLIB REQUIRED)
	include_directories(${ZLIB_INCLUDE_DIRS})

	add_library(wc3libmpq ${wc3lib_MPQ_SRC})
        message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
        message(STATUS "BZIP2 libraries: ${BZIP2_LIBRARIES}")
        message(STATUS "ZLIB libraries: ${ZLIB_LIBRARIES}")
	target_link_libraries(wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES} wc3libhuffman wc3libwave wc3libpklib wc3libmd5lib)

	if (DEBUG AND UNIX)
		# FIXME on Windows
		add_subdirectory(test)
	endif ()

	# install

	install(TARGETS wc3libmpq DESTINATION lib)
	install(FILES ${wc3lib_MPQ_H} DESTINATION include/wc3lib/mpq)
endif ()
