if (MPQ)

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