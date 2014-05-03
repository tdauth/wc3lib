# for proper "config.h" detection we use relative paths of modules as well
include_directories("${PROJECT_BINARY_DIR}/src/jass/llvm")

set (wc3lib_JASS_LLVM_H
	llvm.hpp
)
set (wc3lib_JASS_LLVM_SRC
)

if (DEBUG)
	add_subdirectory(test)
endif ()