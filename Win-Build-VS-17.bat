@echo off
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A Win32 -DCMAKE_BUILD_TYPE=Release ../
mkdir Debug
mkdir Release

msbuild CMakeGL.vcxproj /p:Configuration=Release

..\bin\CMakeGL\Release\CMakeGL.exe

