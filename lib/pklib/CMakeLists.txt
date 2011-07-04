WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_LIB_PKLIB_H)
WC3LIB_GET_SRC(wc3lib_LIB_PKLIB_SRC)

add_library(pklib SHARED ${wc3lib_LIB_PKLIB_SRC})
target_link_libraries(pklib  ${Boost_LIBRARIES})

# install

install(TARGETS pklib DESTINATION lib/wc3lib/lib/)
install(FILES ${wc3lib_LIB_PKLIB_H} PUBLIC_HEADER DESTINATION include/wc3lib/lib/pklib)