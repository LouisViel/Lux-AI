@echo off


REM PREVIOUS WAYS OF BUILDING PROJECT
REM g++ main.cpp -O3 -std=c++11 -I. -o main.out
REM g++ $(find project modules -name "*.cpp") main.cpp -O3 -std=c++11 -I. -o main.out
REM make clean // Clean project before building
REM mingw32-make // Alternative for windows without linux
REM pause


REM BUILD PROJECT USING Make
set MSYS2_BASH=C:\msys64\usr\bin\bash.exe
set CURDIR=%CD%
set CURDIR_UNIX=%CURDIR:\=/%
set DRIVE=%CURDIR_UNIX:~0,1%
set PATH_UNIX=/%DRIVE%%CURDIR_UNIX:~2%

"%MSYS2_BASH%" --login -i -c "export PATH=/mingw64/bin:$PATH && cd %PATH_UNIX% && make clean"
REM pause
