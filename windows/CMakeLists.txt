cmake_minimum_required(VERSION 3.0)

project(wc3lib_windows)

include(ExternalProject)

if (${WIN32})
	message(STATUS "Windows build")
endif ()

#ExternalProject_Add(kdelibs4 DOWNLOAD_NAME kdelibs.tar.xz DOWNLOAD_DIR download URL http://download.kde.org/stable/4.14.3/src/kdelibs-4.14.3.tar.xz)
# requires libtool!
#add_custom_target(copylibtool COMMAND echo $$\(which libtool\) && mkdir -p ${CMAKE_BINARY_DIR}/jpeg && cp $$\(which libtool\) ${CMAKE_BINARY_DIR}/jpeg)
#ExternalProject_Add(jpeg DEPENDS copylibtool DOWNLOAD_NAME jpegsrc.tar.gz DOWNLOAD_DIR download URL http://downloads.sourceforge.net/project/libjpeg/libjpeg/6b/jpegsrc.v6b.tar.gz SOURCE_DIR "jpeg" CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/jpeg/configure" --enable-shared CC='${CMAKE_C_COMPILER}' BUILD_COMMAND make BUILD_IN_SOURCE 1)

SET(MINGW_C_COMPILER x86_64-w64-mingw32-gcc)
SET(MINGW_CXX_COMPILER x86_64-w64-mingw32-g++)
set(MINGW_ROOT_DIR /usr/x86_64-w64-mingw32/sys-root/mingw)
set(MAKE_JOBS "-j4")

# Define the program's version here
set(LLVM_URL "http://llvm.org/releases/3.7.1/llvm-3.7.1.src.tar.xz")
set(BINUTILS_URL "ftp://sourceware.org/pub/binutils/snapshots/binutils-2.26.51.tar.bz2")
set(NASM_URL "http://downloads.sourceforge.net/project/nasm/nasm%20sources/2.07/nasm-2.07.tar.bz2")
set(JPEG_URL "http://downloads.sourceforge.net/project/libjpeg-turbo/1.4.2/libjpeg-turbo-1.4.2.tar.gz")
set(OGRE_URL "https://bitbucket.org/sinbad/ogre")
set(wc3lib_URL "git://wc3lib.org/wc3lib.git")

# We need the llvm + clang since there is a "file is too big" error
ExternalProject_Add(llvm DOWNLOAD_NAME llvm.src.tar.xz DOWNLOAD_DIR download URL ${LLVM_URL} CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/llvm-prefix/src/llvm/ -DCMAKE_CROSSCOMPILING=True -DLLVM_TABLEGEN=/usr/bin/llvm-tblgen -DCLANG_TABLEGEN=/usr/bin/clang-tblgen -DLLVM_DEFAULT_TARGET_TRIPLE=amd64-windows-gnueabihf -DLLVM_TARGET_ARCH=amd64 -DLLVM_TARGETS_TO_BUILD=amd64 -DCMAKE_CXX_COMPILER="/usr/bin/clang++" -DCMAKE_C_COMPILER="/usr/bin/clang" -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DLLVM_BUILD_RUNTIME=Off -DLLVM_INCLUDE_TESTS=Off -DLLVM_INCLUDE_EXAMPLES=Off -DLLVM_ENABLE_BACKTRACES=Off BUILD_COMMAND make ${MAKE_JOBS} INSTALL_COMMAND "")

message(STATUS "Include dir: ${CMAKE_BINARY_DIR}nasm-prefix/src/nasm-build")

ExternalProject_Add(nasm DEPENDS llvm DOWNLOAD_NAME nasm.tar.bz2 DOWNLOAD_DIR download URL ${NASM_URL} CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/nasm-prefix/src/nasm/configure --includedir=${CMAKE_BINARY_DIR}nasm-prefix/src/nasm-build CC=${MINGW_C_COMPILER} CXX=${MINGW_CXX_COMPILER} CFLAGS=-I${CMAKE_BINARY_DIR}nasm-prefix/src/nasm-build BUILD_COMMAND make ${MAKE_JOBS}
  BUILD_IN_SOURCE 1 INSTALL_COMMAND "")
# CFLAGS="-I${CMAKE_BINARY_DIR}/nasm-prefix/src/nasm-build" CPPFLAGS="$CPPFLAGS -I${CMAKE_BINARY_DIR}/nasm-prefix/src/nasm-build"

# TODO move jpeglib into mingw dir
ExternalProject_Add(jpeg DEPENDS llvm nasm DOWNLOAD_NAME jpegsrc.tar.gz DOWNLOAD_DIR download URL ${JPEG_URL} CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/jpeg-prefix/src/jpeg/ -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} BUILD_COMMAND make ${MAKE_JOBS}
  BUILD_IN_SOURCE 1 INSTALL_COMMAND "")

#ExternalProject_Add(glibc DOWNLOAD_NAME glibc.tar.bz2 DOWNLOAD_DIR download URL http://ftp.gnu.org/gnu/glibc/glibc-2.22.tar.bz2 CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/glibc-prefix/src/glibc/configure --prefix=${MINGW_ROOT_DIR} --host=x86_64-w64-mingw32 CC=${MINGW_C_COMPILER} CXX=${MINGW_CXX_COMPILER} BUILD_COMMAND make INSTALL_COMMAND "") # BUILD_IN_SOURCE 1

# DEPENDS glibc
# link against a different c library which is used by mingw
# link against all possible libraries since gettext has undefined references
# --enable-shared
#set(gettext_CFLAGS "-O2 ${MINGW_ROOT_DIR}/lib/libmsvcrt.a")
#ExternalProject_Add(gettext DOWNLOAD_NAME gettext.tar.gz DOWNLOAD_DIR download URL http://ftp.gnu.org/pub/gnu/gettext/gettext-latest.tar.gz CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/gettext-prefix/src/gettext/configure --prefix=${MINGW_ROOT_DIR} --build=x86_64-w64-mingw32 --host=x86_64-w64-mingw32 --enable-static CC=${MINGW_C_COMPILER} CXX=${MINGW_CXX_COMPILER} CFLAGS=${gettext_CFLAGS} CPPFLAGS=${gettext_CFLAGS} BUILD_COMMAND make CFLAGS=${gettext_CFLAGS} BUILD_IN_SOURCE 1 INSTALL_COMMAND "")

#ExternalProject_Add(libintl DOWNLOAD_NAME libintl.zip DOWNLOAD_DIR download URL http://downloads.sourceforge.net/project/gnuwin32/libintl/0.14.4/libintl-0.14.4-src.zip CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/libintl-prefix/src/libintl/src/libintl/0.14.4/libintl-0.14.4-src/configure CC=${MINGW_C_COMPILER} CXX=${MINGW_CXX_COMPILER} CFLAGS=-I${CMAKE_BINARY_DIR}libintl-prefix/src/libintl-build BUILD_COMMAND make BUILD_IN_SOURCE 1 INSTALL_COMMAND "")

set(CMAKE_OPTIONS -DEDITOR=0 -DPLUGINS=0)
set(DEPENDENCIES jpeg llvm) # gettext libintl

option(EDITOR "Editor" 0)
option(PLUGINS "Plugins" 0)

if (EDITOR OR PLUGINS)
	# requires binutils for windres command
	ExternalProject_Add(binutils DEPENDS llvm URL ${BINUTILS_URL} CONFIGURE_COMMAND ./configure BUILD_COMMAND make ${MAKE_JOBS} INSTALL_COMMAND "")
	set(DEPENDENCIES ${DEPENDENCIES} ogre)
	ExternalProject_Add(ogre DEPENDS llvm binutils HG_REPOSITORY ${OGRE_URL} CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/ogre-prefix/src/ogre/ -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_FIND_ROOT_PATH="${CMAKE_CURRENT_BINARY_DIR}/binutils-prefix/src/binutils ${CMAKE_FIND_ROOT_PATH}" BUILD_COMMAND make ${MAKE_JOBS} BUILD_IN_SOURCE 1 INSTALL_COMMAND "")
endif ()

# libintl-8.dll
# apparently it does not link automatically with the library intl although the package gettext is found
# Besides link to the locally compiled libjpeg
# -Wa,-mbig-obj solves the "file too big" error
set(wc3lib_LINKFLAGS "-lintl -L${MINGW_ROOT_DIR}/bin/ -L${MINGW_ROOT_DIR}/lib/ ${MINGW_ROOT_DIR}/bin/libintl-8.dll ${MINGW_ROOT_DIR}/lib/libintl.dll.a ${MINGW_ROOT_DIR}/bin/libgettextlib-0-19-4.dll ${CMAKE_CURRENT_BINARY_DIR}/jpeg-prefix/src/jpeg/libjpeg.a -Wa,-mbig-obj")
set(CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_BINARY_DIR}/jpeg-prefix/src/jpeg ${CMAKE_FIND_ROOT_PATH}")

ExternalProject_Add(wc3lib DEPENDS ${DEPENDENCIES} GIT_REPOSITORY ${wc3lib_URL} CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/wc3lib-prefix/src/wc3lib/ -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install ${CMAKE_OPTIONS} -DCMAKE_CXX_FLAGS=${wc3lib_LINKFLAGS} -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH} BUILD_COMMAND make ${MAKE_JOBS} VERBOSE=1 BUILD_IN_SOURCE 1 INSTALL_COMMAND "")