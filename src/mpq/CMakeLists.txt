if (MPQ)
	WC3LIB_ENABLE_EXCEPTIONS()

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

	add_library(wc3libmpq SHARED ${wc3lib_MPQ_SRC})
	target_link_libraries(wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES} huffman wave pklib)
	if (USE_ENCRYPTION)
		target_link_libraries(wc3libmpq ${CRYPTOPP_LIBRARIES})
	else ()
		target_link_libraries(wc3libmpq ${OPENSSL_LIBRARIES})
	endif ()

	# install

	install(TARGETS wc3libmpq DESTINATION lib/wc3lib/)
	install(FILES ${wc3lib_MPQ_H} DESTINATION include/wc3lib/mpq)
endif ()