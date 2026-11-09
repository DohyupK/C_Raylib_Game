@echo off
setlocal
set GCC=C:\Users\user\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\gcc.exe
set RAYLIB=third_party\raylib-6.2_win64_mingw-w64
set CFLAGS=-std=c11 -O2 -Wall -finput-charset=UTF-8 -fexec-charset=UTF-8 -I%RAYLIB%\include -Isrc
set LDFLAGS=-L%RAYLIB%\lib -lraylib -lopengl32 -lgdi32 -lwinmm

"%GCC%" %CFLAGS% src\main.c -o ArenaBreach.exe %LDFLAGS%

if errorlevel 1 exit /b 1
copy /Y %RAYLIB%\lib\raylib.dll . >nul
echo Build OK: ArenaBreach.exe
endlocal
