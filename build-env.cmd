@ECHO off
cls
@rem Make sure these paths are correct.
set MAKE=mingw32-make.exe 
set MINGWPATH=C:\dev\Qt\Tools\mingw48_32
set MSYSPATH=C:\dev\mingw\msysbasic
set SUPPORTPATH=C:\dev\supportlibs
set PATH=%PATH%;%MINGWPATH%\bin\;%MSYSPATH%\bin;%SUPPORTPATH%\bin\

set LDFLAGS=-L"%SUPPORTPATH%/lib" 
set CPPFLAGS=-I"%SUPPORTPATH%/include" -I"%MINGWPATH%/include" 

set BUILDDEBUG=FALSE


cmd /k echo Meg Build Environment. Just run 'make'



