WC3LIB_ENABLE_EXCEPTIONS()

WC3LIB_GET_H(wc3lib_LIB_WAVE_H)
WC3LIB_GET_SRC(wc3lib_LIB_WAVE_SRC)

add_library(wave SHARED ${wc3lib_LIB_WAVE_SRC})
set_property(TARGET wave PROPERTY LINK_INTERFACE_LIBRARIES "")

# install

install(TARGETS wave DESTINATION lib/wc3lib/lib/)
install(FILES ${wc3lib_LIB_WAVE_H} PUBLIC_HEADER DESTINATION include/wc3lib/lib/wave)
