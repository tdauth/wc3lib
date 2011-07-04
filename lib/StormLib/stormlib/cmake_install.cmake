# Install script for directory: /home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib/libStormLib.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib/libStormLib.so")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib/libStormLib.so"
         RPATH "/usr/local/lib:/usr/kde/4.6/lib")
  ENDIF()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib" TYPE SHARED_LIBRARY FILES "/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/libStormLib.so")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib/libStormLib.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib/libStormLib.so")
    FILE(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib/libStormLib.so"
         OLD_RPATH "/usr/kde/4.6/lib:/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/extern/OGRE/ogre_src_v1-7-2/lib:"
         NEW_RPATH "/usr/local/lib:/usr/kde/4.6/lib")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/wc3lib/lib/libStormLib.so")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/wc3lib/lib/StormLib" TYPE FILE FILES
    "/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/StormPort.h"
    "/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/StormLib.h"
    "/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/SCommon.h"
    "/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/StormDll.h"
    "/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/PUBLIC_HEADER"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/huffman/cmake_install.cmake")
  INCLUDE("/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/misc/cmake_install.cmake")
  INCLUDE("/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/pklib/cmake_install.cmake")
  INCLUDE("/home/tamino/Dokumente/Projekte/Informatik/C++/wc3sdk/wc3lib/lib/StormLib/stormlib/wave/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

