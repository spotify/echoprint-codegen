# Codegen windows compilation instructions

Using Visual C++ 2010 Express

## Preliminary steps

1. Install CMake using the Win32 Installer: http://cmake.org/cmake/resources/software.html
2. Download and install 7-Zip: http://www.7-zip.org/
3. Download zlib: http://zlib.net/zlib-1.2.8.tar.gz ( 1.2.5 is no longer available, you will need to change where Visual C++ looks for the zlib files )
4. Download taglib: http://developer.kde.org/~wheeler/files/src/taglib-1.7.tar.gz
5. Download Boost: http://sourceforge.net/projects/boost/files/boost/1.46.1/
6. Uncompress zlib, taglib and boost into the echoprint-codegen directory

## Dependencies

### Zlib

1. Load CMake
2. For Browse Source... and Browse Build... select the zlib directory you uncompressed
3. Click configure
4. Choose Visual Studio 10 as the generator, and use default native compilers
5. Click generate
6. Load the generated zlib.sln
7. Make sure release is selected in the top bar (next to the play button)
8. In the solution explorer, right click on zlib project and select Build

### Taglib

1. Load CMake
2. For Browse Source... and Browse Build... select the taglib directory you uncompressed
3. Press configure
4. Choose Visual Studio 10 as the generator, and use default native compilers
5. There will be an error saying "Could NOT find ZLIB". Click the Advanced checkbox.
6. Change the ZLIB_INCLUDE_DIR variable to the zlib directory, and ZLIB_LIBRARY variable to zlib-1.2.5\Release\zlib.lib
7. Click Generate
8. Load the generated taglib.sln
9. Make sure release is selected in the top bar (next to the play button)
10. In the solution explorer, right click on tag project and select Build

### FFmpeg

1. Download ffmpeg: http://ffmpeg.zeranoe.com/builds/win32/static/ffmpeg-latest-win32-static.7z
2. Uncompress and move somewhere in your path or 
3. Update your path to include the ffmpeg\bin directory

## Compiling codegen

1. Load codegen.sln
2. If you downloaded different versions of the dependencies you will need to change the path to the include and library directories.
3. Make sure release is selected in the top bar (next to the play button)
4. In the solution explorer, right click on tag project and select Build

 
## Running

Copy tag.dll and zlib.dll into the same directory as codegen.exe. Make sure that the ffmpeg bin directory is on your PATH.

## Credits

Zlib and Taglib compilation instructions from Lukáš Lalinský: http://oxygene.sk/lukas/2011/04/windows-binaries-for-taglib/
