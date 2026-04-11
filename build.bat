@echo off

set DEBUG_DIR=out\build\x64-debug
set RELEASE_DIR=out\build\x64-release

:menu
echo 1. Build x64-release 
echo 2. Build x64-debug
echo 3. Clean build 
echo 4. Exit
set /p choice="Enter your choice: "

if "%choice%"=="1" goto release64
if "%choice%"=="2" goto debug64
if "%choice%"=="3" goto clean
if "%choice%"=="4" goto exit

:release64
    cmake -S . -B %RELEASE_DIR% -A x64
    cmake --build %RELEASE_DIR% --config Release
    pause
    Exit

:debug64 
    cmake -S . -B %DEBUG_DIR% -A x64
    cmake --build %DEBUG_DIR% --config Debug
    pause
    Exit

:clean
    if exist out rd /s /q out
    Exit

:exit
    Exit