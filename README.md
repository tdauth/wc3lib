# wc3lib

The Warcraft III Library is a collection of libraries and tools for supporting the file formats of Blizzard's games Warcraft III: Reign of Chaos and Warcraft III: The Frozen Throne.
The current state is still experimental and under development.
You are welcome to create pull requests with improvements.
The API documentation of the code contains much more information about the project than this overview.
wc3lib does not support CASC archives from Warcraft III: Reforged yet.

The Linux support is much better although wc3lib is cross-platform since the maintainer mainly uses Linux to develop and it is much easier to get the dependencies on Linux.

## Dependencies

wc3lib build requires a C/C++ compiler with C++20 support.

The project requires several external libraries.
To reduce the dependencies just disable certain options.
For example, you can build the project without requiring OGRE and KF5 KIO by disabling the options `MDLX`, `USE_KIOSLAVE` and `USE_OGREBLP`.
These are the libraries required by the core modules:

* Boost Libraries: system filesystem serialization program_options iostreams unit_test_framework timer
* libjpeg9-dev (only when the option `BLP` is enabled)
* bzip2 (only when the option `MPQ` is enabled)
* zlib (only when the option `MPQ` is enabled)
* Doxygen (only when the option `DOC` is enabled)

Options `EDITOR`, `USE_KIOSLAVE`, `USE_QBLP`, `USE_OGREBLP` and `USE_MIME`:

* qt5-qtbase-devel
* qt5-qtgui-devel
* qt5-qtmultimedia-devel
* kf5-kio-devel (only when the options `MPQ` and `USE_KIOSLAVE` are enabled)
* OGRE 1.12.0 (only when the options `BLP` and `USE_OGREBLP` are enabled)

The editor module can be disabled by disabling the options `EDITOR`, `USE_KIOSLAVE`, `USE_QBLP`, `USE_OGREBLP` and `USE_MIME`.
In this case only the core modules and command line tools will be available.

You will need the MPQ archives from Warcraft III: The Frozen Throne if you want to use GUI modules from the editor such as "wc3object" or to run certain unit tests.
You can specify their location in a GUI dialog when starting the binary.

## Automatic Build with TravisCI on Linux

[![Build Status](https://travis-ci.org/tdauth/wc3lib.svg?branch=master)](https://travis-ci.org/tdauth/wc3lib)

## Automatic Windows Build with AppVeyor on Windows

[Latest Build](https://ci.appveyor.com/project/tdauth/wc3lib)

The file [appveyor.yml](./appveyor.yml) configures the automatic build for AppVeyor which uses Windows.

## Manual Build on Linux

Install all dependencies required by wc3lib with the Bash script [ubuntu_install.sh](./ubuntu_install.sh) or manually.
It tries to build wc3lib as well.
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

## Manual Build on Windows

The project has been built on Windows 7 with the following environment:
* Cygwin has been installed into the directory `E:\cygwin`.
* Qt has been installed into the directory `C:\Qt\`.
* Boost has been checked out into the directory `E:\Projekte\boost_1_61_0`.
* wc3lib has been checked out into the directory `E:\Projekte\wc3lib`.

The following steps have been required:

* Use `CMAKE_BUILD_TYPE=Release`, to prevent too big files as well.
* Don't use MDLX (32 bit error too big files) and don't use JASS. Use 64 bit libraries of Qt5 and the cygwin 64 bit GCC compiler.
* Compile Boost libraries using gcc which uses the Cygwin compiler.
* Use Qt libraries from Cygwin as well. Qt5Core_DIR `E:/cygwin/lib/cmake/Qt5Core`
* Either use `C:/Qt/Tools/mingw530_32/bin/mingw32-make.exe` as Make program or use the one of Cygwin but then you have to use Bash as shell which cannot be set for CMake.
* Symbolic link E:\cygwin\usr\lib to E:\cygwin\lib, otherwise CMake does not find the files.
* For MinGW: http://www.boost.org/doc/libs/1_61_0/more/getting_started/windows.html
* In the cygwin Bash: `export PATH="/cygdrive/c/Qt/Tools/mingw530_32/bin/:$PATH"`
* Make sure that the correct GCC is chosen: which gcc.exe
* Correct: `./b2.exe -j4 toolset=gcc --with-iostreams -s BZIP2_SOURCE=E:/Projekte/wc3lib/windows/bzip2-1.0.6 -s ZLIB_SOURCE=E:/Projekte/wc3lib/windows/zlib-1.2.8 stage`
* Then: `./b2.exe -j4 toolset=gcc stage`
* Then replace cyg by lib?
* `./b2.exe -j4 toolset=gcc stage --with-iostreams -s BZIP2_SOURCE=E:\Projekte\wc3lib\windows\bzip2-1.0.6 -s ZLIB_SOURCE=E:\Projekte\wc3lib\windows\zlib-1.2.8`
* `./b2.exe -j4 toolset=gcc stage --with-iostreams -s BZIP2_BINARY=libbz2 -s BZIP2_INCLUDE=/cygdrive/e/Projekte/wc3lib/windows/include -s BZIP2_LIBPATH=/cygdrive/e/Projekte/wc3lib/windows/lib -s ZLIB_BINARY=zlib -s ZLIB_INCLUDE=/cygdrive/e/Projekte/wc3lib/windows/include -s ZLIB_LIBPATH=/cygdrive/e/Projekte/wc3lib/windows/lib (--build-type=complete --layout=tagged)`. The cyg prefix has to be removed afterwards.
* Use `E:\Projekte\boost_1_61_0\stage\lib` as Boost_LIBRARYDIR

## Windows Builds on Fedora (cross compilation)

Fedora offers prebuilt mingw packages which can be used. On Jenkins the following CMake options are passed:
```
cmake -DBLP=${BLP} -DMAP=${MAP} -DMPQ=${MPQ} -DMDLX=${MDLX} -DEDITOR=${EDITOR} -DAPP=${APP} -DDOC=${DOC} -DCMAKE_TOOLCHAIN_FILE=Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ../windows
```

The "windows" folder contains a custom CMakeLists.txt file for the Windows build project. Use the toolchain file to
specify the mingw compiler from Fedora.

## Tools

wc3lib provides several tools which might help to modify Warcraft III.
The source code for the command line tools is placed in the directory [src/app](./src/app).
The source code for the GUI tools is placed in the directory [src/editor/app](./src/editor/app).

TODO Some tools do still not work yet since some modules are unfinished.

### mpq

This tool allows extracting MPQ archives.

The following command extracts all files from the archive `war3.mpq`:
```
mpq -x war3.mpq
```

The following command lists all files from the archive `war3.mpq`:
```
mpq -t war3.mpq
```

### wc3tilesetter

This tool allows changing ground tilesets of a map's environment file `war3map.w3e`.

### wc3baseobjectchanger

This tool allows converting the base object IDs from custom object data into another one.
This might help you to fix object data which does not work in Warcraft III: Reforged anymore.

See [wc3baseobjectchanger.sh](./scripts/wc3baseobjectchanger.sh) as an example.

### wc3objectlevellimiter

This tool allows removing modifications from and adapting the maximum levels of custom object data which are higher than the user-specified passed maximum level.
This might help you to reduce the size of your object data and levels for abilities and researches.

See [wc3abilitylevellimiter.sh](./scripts/wc3abilitylevellimiter.sh) as an example.

### wc3objectcompressor

This tool allows removing modifications from custom object data which are higher than the specific maximum level of the custom objects.

```
./wc3objectcompressor -vF . ~/Downloads/objectdata.w3o > output.txt
```

### wc3objectquery

This tool allows querying modifications from custom object data.

```
./wc3objectquery -v -q "ubdg"=1 -q "upat"=_ ~/Downloads/objectdata.w3o > output.txt
```

### wc3abilityhotkeyfixer

This tools allows fixing hotkeys of abilities.

See [wc3abilityhotkeyfixer.sh](./scripts/wc3abilityhotkeyfixer.sh) as an example.

### wc3converter

This tool allows converting files with Warcraft III formats.

The following command converts the MDL file `MyModel.mdl` into the MDX file `MyModel.mdx`:

```
wc3converter MyModel.mdx MyModel.mdl
```
TODO MDL and MDX parsing and generating might not be fully supported yet.

Converting abilities into JSON:

```
wc3converter abilities.json war3map.w3a
```

### wc3objectdataextractor

This tool allows to extract object data fields from Warcraft III object data into a JASS file.

The following command extracts all fields from the custom units file `war3map.w3u`into the newly generated JASS file `script.j`:

```
wc3objectdataextractor -Fvpg -f umki,usei,urq,urqa,ures,useu,utra,uupt,upgr,ucam -t integerlist,integerlist,integerlist,integerlist,integerlist,integerlist,integerlist,integerlist,integerlist,integer -i war3map.w3u -o script.j
```

### wc3trans

This tool allows to generate map string files (.wts) for translating maps into different languages.

The following command uses the untranslated and translated file versions to create an updated translated file from another map which uses similar strings:
```
wc3trans MyUntranslatedFileA.wts MyTranslatedFileA.wts MyTranslatedFileBWhichNeedsAnUpdate.wts MyOutputUpdatedTranslatedFileB.wts
```

The following command updates the translated file from the same map:
```
wc3trans --update MyUntranslatedFileA.wts MyTranslatedFileA.wts MyTranslatedFileA.wts MyOutputUpdatedTranslatedFileA.wts
```
It drops all string entries which are not used in the untranslated file anymore.

### wc3object

A standalone application similar to the World Editor's Object Editor.

### wc3trigger

A standalone application similar to the World Editor's Trigger Editor.

### wc3mpq

A standalone application which allows the modification of MPQ archives.

### wc3texture

A standalone application which allows the modification of BLP textures.

### wc3terrain

A standalone application which allows the modification of Warcraft III terrain.

### wc3editor

Combines all standalone GUI applications to a single application similar to the World Editor.

## Software Architecture

The project is written in C/C++ since it is a multiparadigm programming language with high performance binaries.
Warcraft III is also written in C/C++ which makes the implementation of similar algorithms much easier even without using original source code.
The compile process might be more difficult especially on platforms like Windows than with programming languages like Java.
However, Java usually produces binaries with worse performance and higher memory usage and requires a running JVM.

The project consists of several modules.
Each module creates one single library which can be used as dependency for any tool.
Your tool does not have to use all libraries from all modules as dependencies which reduces the overhead.
Each module has its own directory in [src](./src).

### BLP module

The BLP module allows to read and write BLP files.

### Map module

The map module allows to read and write TXT, SLK, TriggerData.txt files etc.

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

## Specifications

There is several unoffical specifications for the formats of Warcraft III:

* [MDX Specification](https://www.hiveworkshop.com/threads/mdx-specifications.240487/)
* [BLP Specification](https://www.hiveworkshop.com/threads/blp-specifications-wc3.279306/)
* [Warcraft III Trigger Format Specification](https://www.hiveworkshop.com/threads/warcraft-3-trigger-format-specification-wtg.294491/)
* [vJass Specification](http://www.wc3c.net/vexorian/jasshelpermanual.html)

## Related Projects

* [StormLib](https://github.com/ladislav-zezula/StormLib) - C library for the modification of MPQ archives.
* [inwarcraft collective](https://github.com/inwc3) - Collection of repositories related to Warcraft III modding.
* [JassHelper](http://www.wc3c.net/showthread.php?t=88142) - A compiler which compiles vJass code into JASS code.
* [WarsmashModEngine](https://github.com/Retera/WarsmashModEngine) - Warcraft III engine.

## Credits

* wc3lib: Tamino Dauth
* huffman, pklib, wave, (listfile) files: Ladislav Zezula
* md5: RSA Data Security, Inc. and Mordechai T. Abzug

People I want to thank for various services:

* MPQ specification and basic algorithms:  Justin Olbrantz(Quantam)
* StormLib source code: Ladislav Zezula
* BLP specification: Magnus Ostberg
* MDLX specification: Jimmy "Nub" Campbell, Phil Laing (aka KMK)
* War3ModelEditor source code: Magnus Ostberg
* W3M, W3X, W3N, W3V, W3Z, W3G specifications: Zépir and other people from wc3c.net

and finally all contributors of the Boost C++ Libraries, Qt and KDE SC (especially the KDevPlatform/KDevelop) projects!
