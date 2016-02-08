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

ExternalProject_Add(jpeg DOWNLOAD_NAME jpegsrc.tar.gz DOWNLOAD_DIR download URL http://downloads.sourceforge.net/project/libjpeg-turbo/1.4.2/libjpeg-turbo-1.4.2.tar.gz CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/jpeg-prefix/src/jpeg/ -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH})


#ExternalProject_Add(wc3lib DOWNLOAD_NAME wc3lib.rpm DOWNLOAD_DIR download URL http://127.0.0.1:8080/job/wc3lib_release_linux/lastSuccessfulBuild/artifact/pkg/wc3lib-0.1.0-Linux.rpm)