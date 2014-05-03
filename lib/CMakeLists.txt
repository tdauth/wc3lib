if (MPQ)
	add_compile_options(-fms-extensions)

	add_subdirectory(huffman)
	list(APPEND linkDirectories huffman)
	add_subdirectory(pklib)
	list(APPEND linkDirectories pklib)
	add_subdirectory(wave)
	list(APPEND linkDirectories wave)

	link_directories(${linkDirectories})
endif ()