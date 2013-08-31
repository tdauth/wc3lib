WC3LIB_ENABLE_EXCEPTIONS()

set(wc3lib_LIB_HUFFMAN_H
	huffman.h
)
set(wc3lib_LIB_HUFFMAN_SRC
	huffman.cpp
)

add_library(huffman SHARED ${wc3lib_LIB_HUFFMAN_SRC})

# install

install(TARGETS huffman DESTINATION lib/wc3lib/lib/)
install(FILES ${wc3lib_LIB_HUFFMAN_H} DESTINATION include/wc3lib/lib/huffman)