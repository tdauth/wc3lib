cmake_minimum_required(VERSION 3.0)

project(wc3lib_windows)

include(ExternalProject)

# builds KDE for Windows on Fedora


set(KDE_PREFIX    /windows/kde4)
set(KDE4_SRC_DIR  /home/kdeuser/kde/src)

# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

set(MINGW_ROOT_DIR /usr/x86_64-w64-mingw32/sys-root/mingw)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  ${MINGW_ROOT_DIR})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# FindQt4.cmake querys qmake to get information, this doesn't work when crosscompiling
set(QT_BINARY_DIR   ${MINGW_ROOT_DIR}/bin)
set(QT_LIBRARY_DIR   ${MINGW_ROOT_DIR}/lib)
set(QT_QTCORE_LIBRARY    ${MINGW_ROOT_DIR}/lib/libQtCore4.dll.a)
set(QT_QTCORE_INCLUDE_DIR  ${MINGW_ROOT_DIR}/include/QtCore)
set(QT_MKSPECS_DIR  ${MINGW_ROOT_DIR}/mkspecs)
set(QT_MOC_EXECUTABLE  ${QT_BINARY_DIR}/moc)
set(QT_QMAKE_EXECUTABLE  ${QT_BINARY_DIR}/qmake)
set(QT_UIC_EXECUTABLE  ${QT_BINARY_DIR}/uic)

#ExternalProject_Add(kdelibs4 DOWNLOAD_NAME kdelibs.tar.xz DOWNLOAD_DIR download URL http://download.kde.org/stable/4.14.3/src/kdelibs-4.14.3.tar.xz)
# requires libtool!
#add_custom_target(copylibtool COMMAND echo $$\(which libtool\) && mkdir -p ${CMAKE_BINARY_DIR}/jpeg && cp $$\(which libtool\) ${CMAKE_BINARY_DIR}/jpeg)
#ExternalProject_Add(jpeg DEPENDS copylibtool DOWNLOAD_NAME jpegsrc.tar.gz DOWNLOAD_DIR download URL http://downloads.sourceforge.net/project/libjpeg/libjpeg/6b/jpegsrc.v6b.tar.gz SOURCE_DIR "jpeg" CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/jpeg/configure" --enable-shared CC='${CMAKE_C_COMPILER}' BUILD_COMMAND make BUILD_IN_SOURCE 1)

ExternalProject_Add(jpeg DOWNLOAD_NAME jpegsrc.tar.gz DOWNLOAD_DIR download URL http://downloads.sourceforge.net/project/libjpeg-turbo/1.4.2/libjpeg-turbo-1.4.2.tar.gz CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/jpeg-prefix/src/jpeg/ -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH})


#ExternalProject_Add(wc3lib DOWNLOAD_NAME wc3lib.rpm DOWNLOAD_DIR download URL http://127.0.0.1:8080/job/wc3lib_release_linux/lastSuccessfulBuild/artifact/pkg/wc3lib-0.1.0-Linux.rpm)