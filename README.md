# C 2D Simulation Environment (No CMake)

This project is set up for **C + SDL3** with a very small build flow.

## Why I chose SDL3 instead of SDL2

- SDL3 is the current generation of SDL and is where active API evolution is happening.
- You are starting a new project (boids, Conway, particle-life), so there is no legacy lock-in.
- For pure 2D simulation work, SDL3 provides everything needed now and keeps the project future-facing.

SDL2 is still good and stable, but for a fresh codebase SDL3 is the better long-term default.

## Project layout

- `src/main.c` minimal SDL app and render loop
- `build.bat` compile script
- `run.bat` build then run
- `bin/` compiled output

## 1) Install toolchain and SDL3 (Windows, MSYS2)

1. Install MSYS2 from https://www.msys2.org/
2. Open **MSYS2 UCRT64** shell and run:

```bash
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-sdl3
```

3. Either:
- Keep using the UCRT64 shell, or
- Add `C:\msys64\ucrt64\bin` to your Windows PATH so `gcc` and `pkg-config` are available in PowerShell/CMD.

## 2) Build

From project root:

```bat
build.bat
```

## 3) Run

```bat
run.bat
```

## What to add later to keep things working

### Add more C files

- Put new `.c` files inside `src/`.
- `build.bat` already compiles `src\*.c`, so no script changes are needed.

### Add headers

- Put your headers in `src/` or create an `include/` folder.
- If you create `include/`, add `-Iinclude` to the `gcc` line in `build.bat`.

### Add another library

- If the library supports `pkg-config`, extend this line in `build.bat`:

```bat
pkg-config --cflags --libs sdl3
```

Example:

```bat
pkg-config --cflags --libs sdl3 some_other_lib
```

- If a library does not use `pkg-config`, add its include/lib flags manually to the `gcc` command.

## Notes

- This setup intentionally avoids CMake and other build systems.
- Keep `build.bat` as the single source of build options.
