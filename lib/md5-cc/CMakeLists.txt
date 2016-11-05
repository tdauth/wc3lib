set(wc3lib_LIB_MD5_H
	md5.hh
)
set(wc3lib_LIB_MD5_SRC
	md5.cc
)

add_library(wc3libmd5lib ${wc3lib_LIB_MD5_SRC})
set_source_files_properties(${wc3lib_LIB_MD5_H} ${wc3lib_LIB_MD5_SRC} PROPERTIES LANGUAGE CXX)
set_target_properties(wc3libmd5lib PROPERTIES LINKER_LANGUAGE CXX)

# install

install(TARGETS wc3libmd5lib DESTINATION lib)
install(FILES ${wc3lib_LIB_MD5_H} DESTINATION include/wc3lib/lib/md5-cc)