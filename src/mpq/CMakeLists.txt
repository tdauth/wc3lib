if (MPQ)
	# for proper "config.h" detection we use relative paths of modules as well
	include_directories("${PROJECT_BINARY_DIR}/src/mpq")

	set(wc3lib_MPQ_H
		algorithm.hpp
		attributes.hpp
		block.hpp
		hash.hpp
		listfile.hpp
		mpq.hpp
		mpqfile.hpp
		platform.hpp
		sector.hpp
		signature.hpp
	)
	set(wc3lib_MPQ_SRC
		algorithm.cpp
		attributes.cpp
		block.cpp
		hash.cpp
		listfile.cpp
		mpq.cpp
		mpqfile.cpp
		sector.cpp
		signature.cpp
	)

	if (DEBUG)
		set(Boost_DEBUG ON)
		add_definitions(-DBOOST_TEST_DYN_LINK=1)
	endif ()

	# regex is required for listfile only
	find_package(Boost COMPONENTS system filesystem thread serialization program_options iostreams regex REQUIRED)

	# Both compression libraries are required by Boost "iostreams" component which is used for the "mpq" module.
	# Crypto++ library is required for RSA encryption and SHA1 digest and MD5 checksum calculation (Boost does only support CRC).
	find_package(BZip2 REQUIRED)
	include_directories(${BZIP2_INCLUDE_DIR})
	find_package(ZLIB REQUIRED)
	include_directories(${ZLIB_INCLUDE_DIRS})

	if (USE_ENCRYPTION)
		set(CRYPTOPP_INCLUDE_DIRS "${CMAKE_SYSTEM_INCLUDE_PATH}")
		set(CRYPTOPP_LIB_DIR "${CMAKE_SYSTEM_LIBRARY_PATH}")

		find_path(tmp "rsa.h" "${CRYPTOPP_INCLUDE_DIRS}/crypto++")

		if (NOT (${tmp} EQUAL "tmp-NOTFOUND"))
			include_directories(${CRYPTOPP_INCLUDE_DIRS})
			link_directories(${CRYPTOPP_LIB_DIR})
			find_library(CRYPTOPP_LIB NAMES crypto++ REQUIRED "${CRYPTOPP_LIB_DIR}")

			if (NOT (CRYPTOPP_LIB EQUAL "CRYPTOPP_LIB-NOTFOUND"))
				set(CRYPTOPP_LIBRARIES ${CRYPTOPP_LIB})
				message(STATUS "Crypto++ libraries: ${CRYPTOPP_LIBRARIES}")
				set(CRYPTOPP_FOUND 1)
			else ()
				set(CRYPTOPP_FOUND 0)
			endif ()
		else ()
			message(FATAL_ERROR "\"rsa.h\" could not be found in paths: ${CRYPTOPP_INCLUDE_DIRS}.")
		endif ()
	else ()
		find_package(OpenSSL REQUIRED)
		include_directories(${OPENSSL_INCLUDE_DIR})
	endif ()

	add_library(wc3libmpq SHARED ${wc3lib_MPQ_SRC})
	target_link_libraries(wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES} wc3libhuffman wc3libwave wc3libpklib)
	if (USE_ENCRYPTION)
		target_link_libraries(wc3libmpq ${CRYPTOPP_LIBRARIES})
	else ()
		target_link_libraries(wc3libmpq ${OPENSSL_LIBRARIES})
	endif ()

	if (DEBUG)
		add_subdirectory(test)
	endif ()

	# install

	install(TARGETS wc3libmpq DESTINATION lib)
	install(FILES ${wc3lib_MPQ_H} DESTINATION include/wc3lib/mpq)
endif ()