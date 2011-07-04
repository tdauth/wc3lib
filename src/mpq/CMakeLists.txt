WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_MPQ_H)
WC3LIB_GET_SRC(wc3lib_MPQ_SRC)

add_library(wc3libmpq SHARED ${wc3lib_MPQ_SRC})
target_link_libraries(wc3libmpq wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES} ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES} huffman wave pklib)

# install

install(TARGETS wc3libmpq DESTINATION lib/wc3lib/)
install(FILES ${wc3lib_MPQ_H} PUBLIC_HEADER DESTINATION include/wc3lib/mpq)