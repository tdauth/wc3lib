set(wc3lib_LIB_WAVE_H
	wave.h
)
set(wc3lib_LIB_WAVE_SRC
	wave.cpp
)

add_library(wc3libwave SHARED ${wc3lib_LIB_WAVE_SRC})

# install

install(TARGETS wc3libwave DESTINATION lib)
install(FILES ${wc3lib_LIB_WAVE_H} DESTINATION include/wc3lib/lib/wave)
