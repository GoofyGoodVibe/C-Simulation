@echo off
setlocal

where gcc >nul 2>&1
if errorlevel 1 (
    echo gcc was not found in PATH.
    echo Use the MSYS2 UCRT64 shell or add C:\msys64\ucrt64\bin to PATH.
    exit /b 1
)

where pkg-config >nul 2>&1
if errorlevel 1 (
    echo pkg-config was not found in PATH.
    echo Install pkgconf in MSYS2 and ensure it is in PATH.
    exit /b 1
)

pkg-config --exists sdl3
if errorlevel 1 (
    echo SDL3 development files not found by pkg-config.
    echo Install: pacman -S --needed mingw-w64-ucrt-x86_64-sdl3
    exit /b 1
)

for /f "usebackq delims=" %%i in (`pkg-config --cflags --libs sdl3`) do set SDL_FLAGS=%%i

if not exist bin mkdir bin

gcc -std=c11 -Wall -Wextra -g -O0 -Iinclude src\*.c -o bin\sim.exe %SDL_FLAGS%
if errorlevel 1 exit /b 1

set SDL3_DLL=
for /f "delims=" %%i in ('where SDL3.dll 2^>nul') do (
    set SDL3_DLL=%%i
    goto :copydll
)

:copydll
if defined SDL3_DLL copy /Y "%SDL3_DLL%" "bin\SDL3.dll" >nul

echo Build complete: bin\sim.exe
exit /b 0
