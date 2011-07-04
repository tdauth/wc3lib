WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_LIB_READBLEND_H)
WC3LIB_GET_SRC(wc3lib_LIB_READBLEND_SRC)

add_library(readblend SHARED ${wc3lib_LIB_READBLEND_SRC}) 

install(TARGETS readblend DESTINATION lib/wc3lib/lib/)
install(FILES ${wc3lib_LIB_READBLEND_H} PUBLIC_HEADER DESTINATION include/wc3lib/lib/readblend)