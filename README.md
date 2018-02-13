# wc3lib

The Warcraft III Library is a collection of libraries for supporting the file formats of Blizzard's games Warcraft III: Reign of Chaos and Warcraft III: The Frozen Throne.
Additionally, the project provides some useful applications for modifying the game.
Please consider that this version is NEITHER stable NOR usable yet!

## Dependencies
* Boost Libraries: system filesystem serialization program_options iostreams unit_test_framework timer
* libjpeg9-dev (option `BLP`)
* bzip2 (option `MPQ`)
* zlib (option `MPQ`)
* Doxygen (option `DOC`)

Options `EDITOR` and `USE_KIOSLAVE`, `USE_QBLP` and `USE_OGREBLP`:
* qt5-qtbase-devel
* qt5-qtgui-devel
* qt5-qtmultimedia-devel
* kf5-kio-devel
* OGRE 1.9.0

## Automatic Build with TravisCI
[![Build Status](https://travis-ci.org/tdauth/wc3lib.svg?branch=master)](https://travis-ci.org/tdauth/wc3lib)

## Manual Build
For all available compile flags/options configure via cmake please and you'll get all options with their corresponding descriptions.
To build the wc3lib change into the wc3lib directory and run:
```
mkdir build
cd build
cmake ../
make -j4
```

Run `make -j4 install` if you want to install the program.

Specify the options in CMake to reduce the dependencies and modules which are built.
For example, you can build the core module only without any GUI.
Therefore, the build won't require Qt5 anymore.

Don't forget to place the files `ogre.cfg` and `plugins.cfg` files into working directory of editor application or to configure the installed files in `/etc/wc3lib`.
The files contain graphics driver information required by OGRE.
Otherwise, the 3D display won't work.

### Windows
* CMAKE_BUILD_TYPE = Release, to prevent too big files as well.
* Don't use MDLX (32 bit error too big files) and don't use JASS. Use 64 bit libraries of Qt5 and the cygwin 64 bit GCC compiler.
* Compile Boost libraries using gcc which uses the Cygwin compiler.
* Use Qt libraries from Cygwin as well. Qt5Core_DIR E:/cygwin/lib/cmake/Qt5Core
* Either use "C:/Qt/Tools/mingw530_32/bin/mingw32-make.exe" as Make program or use the one of Cygwin but then you have to use Bash as shell which cannot be set for CMake.
* Symbolic link E:\cygwin\usr\lib to E:\cygwin\lib, otherwise CMake does not find the files.
* For MinGW: http://www.boost.org/doc/libs/1_61_0/more/getting_started/windows.html
* In the cygwin Bash: export PATH="/cygdrive/c/Qt/Tools/mingw530_32/bin/:$PATH"
* Make sure that the correct GCC is chosen: which gcc.exe
* Correct: ./b2.exe -j4 toolset=gcc --with-iostreams -s BZIP2_SOURCE=E:/Projekte/wc3lib/windows/bzip2-1.0.6 -s ZLIB_SOURCE=E:/Projekte/wc3lib/windows/zlib-1.2.8 stage
* Then: ./b2.exe -j4 toolset=gcc stage
* Then replace cyg by lib?
* ./b2.exe -j4 toolset=gcc stage --with-iostreams -s BZIP2_SOURCE=E:\Projekte\wc3lib\windows\bzip2-1.0.6 -s ZLIB_SOURCE=E:\Projekte\wc3lib\windows\zlib-1.2.8
* ./b2.exe -j4 toolset=gcc stage --with-iostreams -s BZIP2_BINARY=libbz2 -s BZIP2_INCLUDE=/cygdrive/e/Projekte/wc3lib/windows/include -s BZIP2_LIBPATH=/cygdrive/e/Projekte/wc3lib/windows/lib -s ZLIB_BINARY=zlib -s ZLIB_INCLUDE=/cygdrive/e/Projekte/wc3lib/windows/include -s ZLIB_LIBPATH=/cygdrive/e/Projekte/wc3lib/windows/lib (--build-type=complete --layout=tagged) The cyg prefix has to be removed afterwards.
* Use "E:\Projekte\boost_1_61_0\stage\lib" as Boost_LIBRARYDIR

### Windows Builds on Fedora (cross compilation)
Fedora offers prebuilt mingw packages which can be used. On Jenkins the following CMake options are passed:
```
cmake -DBLP=${BLP} -DMAP=${MAP} -DMPQ=${MPQ} -DMDLX=${MDLX} -DEDITOR=${EDITOR} -DAPP=${APP} -DDOC=${DOC} -DCMAKE_TOOLCHAIN_FILE=Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ../windows
```

The "windows" folder contains a custom CMakeLists.txt file for the Windows build project. Use the toolchain file to
specify the mingw compiler from Fedora.

## Architecture
The project consists of several libraries.
Each library has its own directory in [src](./src).

## Unit Tests
Define environment variable `WC3_DIR` to the Warcraft III directory to make some Unit Tests work.

## Code Style
Rule T009 is too strict since we allow line breaks before commas in the class member initialization list.
In Jenkins I use the following rules for vera++:
```
vera++ --root /usr/lib/vera++/ -c report.xml -w -R F001 -R L001 -R L003 -R T001 -R T002 -R T003 -R T004 -R T005 -R T006 -R T007 -R T008 -R T009 -R T010 -R T011 -R T013 -R T015 -R T016 -R T017 -R T018 -R T019 $(find $WORKSPACE/src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c")
```

## Bugs
If OGRE is installed with the useflag "threads" the following bug may occur:
http://www.ogre3d.org/forums/viewtopic.php?f=2&t=70021

On Fedora this problem might occur:
http://www.ogre3d.org/forums/viewtopic.php?f=2&t=91620

The JASS and the MDLX modules are unfinished at the moment.
Just disable them in the CMake options.

## Credits
wc3lib maintainer:
Tamino Dauth

huffman:
pklib:
wave:
(listfile) files:
Ladislav Zezula

md5:
RSA Data Security, Inc.
Mordechai T. Abzug

People I want to thank for various services:

MPQ specification and basic algorithms:
Justin Olbrantz(Quantam)

StormLib source code:
Ladislav Zezula

BLP specification:
Magnus Ostberg

MDLX specification:
Jimmy "Nub" Campbell
Phil Laing (aka KMK)

War3ModelEditor source code:
Magnus Ostberg

W3M, W3X, W3N, W3V, W3Z, W3G specifications:
Zépir and other people from wc3c.net

and finally all contributors of the Boost C++ Libraries, Qt and KDE SC (especially the KDevPlatform/KDevelop) projects!
