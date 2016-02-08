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

message(STATUS "Include dir: ${CMAKE_BINARY_DIR}nasm-prefix/src/nasm-build")

ExternalProject_Add(nasm DOWNLOAD_NAME nasm.tar.bz2 DOWNLOAD_DIR download URL http://downloads.sourceforge.net/project/nasm/nasm%20sources/2.07/nasm-2.07.tar.bz2 CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/nasm-prefix/src/nasm/configure --includedir=${CMAKE_BINARY_DIR}nasm-prefix/src/nasm-build CC=${MINGW_C_COMPILER} CXX=${MINGW_CXX_COMPILER} CFLAGS=-I${CMAKE_BINARY_DIR}nasm-prefix/src/nasm-build BUILD_COMMAND make
  BUILD_IN_SOURCE 1 INSTALL_COMMAND "")
# CFLAGS="-I${CMAKE_BINARY_DIR}/nasm-prefix/src/nasm-build" CPPFLAGS="$CPPFLAGS -I${CMAKE_BINARY_DIR}/nasm-prefix/src/nasm-build"

# TODO move jpeglib into mingw dir
ExternalProject_Add(jpeg DEPENDS nasm DOWNLOAD_NAME jpegsrc.tar.gz DOWNLOAD_DIR download URL http://downloads.sourceforge.net/project/libjpeg-turbo/1.4.2/libjpeg-turbo-1.4.2.tar.gz CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/jpeg-prefix/src/jpeg/ -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install BUILD_COMMAND make
  BUILD_IN_SOURCE 1 INSTALL_COMMAND "")

set(CMAKE_OPTIONS -DEDITOR=0 -DPLUGINS=0)
set(DEPENDENCIES jpeg)

option(EDITOR "Editor" 0)
option(PLUGINS "Plugins" 0)

if (EDITOR OR PLUGINS)
	set(DEPENDENCIES ${DEPENDENCIES} ogre)
	ExternalProject_Add(ogre HG_REPOSITORY https://bitbucket.org/sinbad/ogre CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/ogre-prefix/src/ogre/ -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install BUILD_COMMAND make BUILD_IN_SOURCE 1 INSTALL_COMMAND "")
endif ()

ExternalProject_Add(wc3lib DEPENDS ${DEPENDENCIES} GIT_REPOSITORY git://wc3lib.org/wc3lib.git CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/wc3lib-prefix/src/wc3lib/ -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install ${CMAKE_OPTIONS} BUILD_COMMAND make BUILD_IN_SOURCE 1 INSTALL_COMMAND "")