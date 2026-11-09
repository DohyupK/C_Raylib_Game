@echo off
setlocal
set GCC=C:\Users\user\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\gcc.exe
set RAYLIB=third_party\raylib-6.2_win64_mingw-w64
set CFLAGS=-std=c11 -O2 -Wall -finput-charset=UTF-8 -fexec-charset=UTF-8 -I%RAYLIB%\include -IC_Raylib_Game
set LDFLAGS=-L%RAYLIB%\lib -lraylib -lopengl32 -lgdi32 -lwinmm

"%GCC%" %CFLAGS% ^
  C_Raylib_Game\main.c ^
  C_Raylib_Game\util.c ^
  C_Raylib_Game\camera_ctrl.c ^
  C_Raylib_Game\world.c ^
  C_Raylib_Game\player.c ^
  C_Raylib_Game\weapons.c ^
  C_Raylib_Game\inventory.c ^
  C_Raylib_Game\monster_data.c ^
  C_Raylib_Game\monsters.c ^
  C_Raylib_Game\waves.c ^
  C_Raylib_Game\abilities.c ^
  C_Raylib_Game\score.c ^
  C_Raylib_Game\bosses.c ^
  C_Raylib_Game\meta.c ^
  C_Raylib_Game\hud.c ^
  C_Raylib_Game\assets.c ^
  C_Raylib_Game\audio_sfx.c ^
  C_Raylib_Game\ui_font.c ^
  C_Raylib_Game\menu.c ^
  C_Raylib_Game\locale_kr.c ^
  -o ArenaBreach.exe %LDFLAGS%

if errorlevel 1 exit /b 1
copy /Y %RAYLIB%\lib\raylib.dll . >nul
echo Build OK: ArenaBreach.exe
endlocal
