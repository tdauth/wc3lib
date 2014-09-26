if (MPQ)
	# add MS extensions support to compile properly with Open Source compilers
	# TODO add more generic support
	#CHECK_CXX_COMPILER_FLAG(supportsMsExtensions, "-fms-extensions")

	#if (supportsMsExtensions)
	#	add_compile_options(-fms-extensions)
	#endif ()

	add_subdirectory(huffman)
	list(APPEND linkDirectories huffman)
	add_subdirectory(pklib)
	list(APPEND linkDirectories pklib)
	add_subdirectory(wave)
	list(APPEND linkDirectories wave)
	add_subdirectory(md5-cc)
	list(APPEND linkDirectories md5-cc)

	link_directories(${linkDirectories})
endif ()