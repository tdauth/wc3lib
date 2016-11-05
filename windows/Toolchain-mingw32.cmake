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