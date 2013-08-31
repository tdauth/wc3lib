WC3LIB_ENABLE_EXCEPTIONS()

set(wc3lib_LIB_WAVE_H
	wave.h
)
set(wc3lib_LIB_WAVE_SRC
	wave.cpp
)

add_library(wave SHARED ${wc3lib_LIB_WAVE_SRC})

# install

install(TARGETS wave DESTINATION lib/wc3lib/lib/)
install(FILES ${wc3lib_LIB_WAVE_H} DESTINATION include/wc3lib/lib/wave)
