WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_LIB_HUFFMAN_H)
WC3LIB_GET_SRC(wc3lib_LIB_HUFFMAN_SRC)

add_library(huffman SHARED ${wc3lib_LIB_HUFFMAN_SRC})
target_link_libraries(huffman  ${Boost_LIBRARIES})

# install

install(TARGETS huffman DESTINATION lib/wc3lib/lib/)
install(FILES ${wc3lib_LIB_PKLIB_H} PUBLIC_HEADER DESTINATION include/wc3lib/lib/huffman)