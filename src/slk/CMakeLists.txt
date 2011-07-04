WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_SLK_H)
WC3LIB_GET_SRC(wc3lib_SLK_SRC)

add_library(wc3libslk SHARED ${wc3lib_SLK_SRC})
target_link_libraries(wc3libslk ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})

# install

install(TARGETS wc3libslk DESTINATION lib/wc3lib/)
install(FILES ${wc3lib_SLK_H} PUBLIC_HEADER DESTINATION include/wc3lib/slk)
