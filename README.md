# 🌌 Boids & Conway's Game of Life Simulation

A minimal, high-performance 2D simulation environment written in pure C using **SDL3**. This project features implementations of Craig Reynolds' **Boids flocking algorithm** and John Conway's **Game of Life**.

This project serves as a personal archive for learning purposes. **I am actively learning C and am not an expert.** AI was not used to write any of the source code. Instead, AI acts as a dedicated teacher to help me break through coding roadblocks and conceptually understand complex topics.

This repository is public for educational use, but the repository is strictly **read-only**.

---

## 🛠️ Tech Stack & Architecture

*   **Language:** Pure C (optimized for raw speed and low memory footprint).
*   **Graphics:** [SDL3](https://github.com/libsdl-org/SDL) (chosen over SDL2 to ensure modern API usage and long-term future-proofing).
*   **Build System:** Single-file batch scripts (`.bat`). **No CMake** or complex meta-build engines required.

### Project Layout
```text
├── bin/          # Compiled executable outputs
├── src/
│   └── main.c    # Application entry point & core SDL3 render loop
├── include/      # Application Header files
├── build.bat     # Windows compilation script
└── run.bat       # Wrapper script to build and execute sequentially
```

---

## 🚀 Getting Started (Windows)

Follow these steps to set up your toolchain, link SDL3, and run the simulation.

### 1. Install Toolchain and SDL3
1. Download and run the setup installer from the [MSYS2 Official Website](https://www.msys2.org/).
2. Open the **MSYS2 UCRT64** terminal environment.
3. Update packages and install GCC, pkg-config, and SDL3 by running:
   ```bash
   pacman -Syu
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-sdl3
   ```
4. *(Optional)* Add `C:\msys64\ucrt64\bin` to your Windows system environment **PATH** variable if you wish to run `gcc` and `pkg-config` directly from standard PowerShell or CMD windows.

### 2. Build the Project
Open your terminal at the project root directory and run the compilation script:
```bat
build.bat
```

### 3. Run the Simulation
Execute the runtime wrapper script to launch the application:
```bat
run.bat
```

---

## 🔧 Project Maintenance & Extensions

Because this build environment relies entirely on `build.bat` as its single source of truth, upgrading the simulation is highly straightforward:

*   **Adding Source Files:** Place any new `.c` files directly inside the `src/` directory. The script compiles `src\*.c` automatically without any script alterations.
*   **Adding Header Directories:** If you decide to decouple your headers into a standalone `include/` directory, append the `-Iinclude` compiler flag to your `gcc` invocation string inside `build.bat`.
*   **Linking Extra Libraries:** For external tools supporting `pkg-config`, append them to the existing configuration flag line:
    ```bat
    pkg-config --cflags --libs sdl3 your_additional_library_here
    ```

---

## 🔒 Repository & AI Notice

*   **AI Usage**: AI assistance was utilized exclusively to clean up and format this Markdown README, as well as serve as an interactive programming tutor to navigate conceptual logic hurdles. **100% of the simulation source code is handwritten** as part of my C learning journey.
*   **Contributions:** Pull Requests and Issues are permanently disabled. This project serves purely as a personal experiment archive.
*   **Security:** This repository is confirmed safe. It contains no API credentials, private infrastructure tokens, or sensitive user environment keys.
