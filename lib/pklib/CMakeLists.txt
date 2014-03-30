set(wc3lib_LIB_PKLIB_H
	pklib.h
)
set(wc3lib_LIB_PKLIB_SRC
	crc32.c
	explode.c
	implode.c
)

add_library(wc3libpklib SHARED ${wc3lib_LIB_PKLIB_SRC})

# install

install(TARGETS wc3libpklib DESTINATION lib)
install(FILES ${wc3lib_LIB_PKLIB_H} DESTINATION include/wc3lib/lib/pklib)