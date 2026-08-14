
# Project Architecture

This is a C-based simulation application utilizing SDL3 and FreeType.

## Directory Structure
- `/include`: Contains all header files (`.h`) defining data structures and function prototypes.
- `/src`: Contains all C source files (`.c`) implementing the simulation logic.
- `/bin`: Contains the compiled executable (`sim.exe`) and required third-party dynamic libraries (`.dll`).
- `/assets`: Contains visual media elements used by the simulation.

## Component Breakdown
- **main.c**: Entry point of the simulation loop, window initialization, and event handling.
- **particle**: Handles particle physics, rendering, structures, and lifecycle state management (`particle.c` / `particle.h`).
- **threadpool**: Manages multi-threaded parallel execution or worker threads for the physics/simulation steps (`threadpool.c` / `threadpool.h`).
- **simtimer**: Manages delta time, frame rate clamping, and timing metrics (`simtimer.c` / `simtimer.h`).
- **assets**: System for loading and managing textures/images like BMP and PNG files (`assets.c` / `assets.h`).
- **general**: Helper functions, global definitions, and shared utilities (`general.c` / `general.h`).

## Coding Standards
- Match implementations in `/src` strictly with their corresponding headers in `/include`.
- Ensure proper memory allocation/freeing practices, especially within particle lifecycles and threadpool destruction.
- Leverage the custom threadpool module for parallel processing tasks rather than spawning raw threads.
- Keep dependencies decoupled; avoid cross-referencing module internals directly outside their public headers.

## Build and Execution
- Use `build.bat` to compile the codebase.
- Use `run.bat` to execute the application with the proper pathing to `/bin`.