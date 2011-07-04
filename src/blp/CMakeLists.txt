WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_BLP_H)
WC3LIB_GET_SRC(wc3lib_BLP_SRC)

add_library(wc3libblp SHARED ${wc3lib_BLP_SRC})
target_link_libraries(wc3libblp wc3libcore ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

# install

install(TARGETS wc3libblp DESTINATION lib/wc3lib/)
install(FILES ${wc3lib_BLP_H} PUBLIC_HEADER DESTINATION include/wc3lib/blp)
