set(wc3lib_LIB_HUFFMAN_H
	huffman.h
)
set(wc3lib_LIB_HUFFMAN_SRC
	huffman.cpp
)

add_library(wc3libhuffman ${wc3lib_LIB_HUFFMAN_SRC})

# install

install(TARGETS wc3libhuffman DESTINATION lib)
install(FILES ${wc3lib_LIB_HUFFMAN_H} DESTINATION include/wc3lib/lib/huffman)