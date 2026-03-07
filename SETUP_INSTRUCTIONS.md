# How to Share and Set Up the Ubi Contest API Project

This guide provides clear instructions for setting up, building, and running the "Ubi Contest API" project on both Windows and macOS.

## 1. Project Overview

This is a C++ project built using CMake, designed for the Ubisoft Toronto NEXT Programming Challenge. The project generates a game application.

## 2. Dependencies

### For All Operating Systems:

- **CMake**: A cross-platform build system generator.
  - Download from: [https://cmake.org/download/](https://cmake.org/download/)

### For Windows Users:

- **Visual Studio 2022 Community**: An IDE, Windows build system, and C++ compiler.
  - Download from: [https://visualstudio.microsoft.com/vs/community/](https://visualstudio.microsoft.com/vs/community/)
  - Ensure C++ development tools are selected during installation.

### For macOS Users:

- **Homebrew**: A package manager for macOS.
  - Install from: [https://brew.sh/](https://brew.sh/) (follow instructions on the website)
- **freeglut**: An OpenGL utility library.
  - Install via Homebrew: Open Terminal and run `brew install freeglut`
- **SDL3**: A cross-platform utility library.
  - Install via Homebrew: Open Terminal and run `brew install SDL3`
- **Xcode Command Line Tools**: Contains the macOS build system and compiler.
  - Usually installed with Homebrew, but if not, install via Terminal: `xcode-select --install`

## 3. Building the Project

### For Windows:

1.  **Generate Build System**:
    - Navigate to the project's root directory (`UbiToronto`).
    - Double-click `generate-windows.bat`. This will open a command prompt, run the CMake generation, and create a `build/win64` directory.
2.  **Open in Visual Studio**:
    - Go to `build/win64`.
    - Double-click the `.sln` file (e.g., `UbiTorContestAPI.sln`) to open the project in Visual Studio.
3.  **Build and Run**:
    - In Visual Studio, build the solution.
    - Run the project from Visual Studio.

### For macOS:

1.  **Generate Build System**:
    - Open Terminal and navigate to the project's root directory (`UbiToronto`).
    - Run the script: `./generate-macos.sh` (You might need to grant execute permissions first: `chmod +x generate-macos.sh`). This will create a `build/macos` directory.
2.  **Build the Program**:
    - Navigate into the generated build directory: `cd build/macos`
    - Run: `make all`
3.  **Run the Game**:
    - From the `build/macos` directory, run: `make run`
    - **Note**: To quit the application, use `Cmd + Q`.

## 4. Adding Code and Assets

- **New Code Files**: Place all new C++ source files (e.g., `.cpp`, `.h`) in the `src/Game` subdirectory.
  - After adding new code files, you must re-run either `generate-windows.bat` (Windows) or `./generate-macos.sh` (macOS) to update the build system. Then, rebuild the project.
- **Game Assets**: Place any game assets (images, audio, models, etc.) in the `data` folder. You can use the existing `TestData` folder or create your own subdirectories within `data`.

## 5. Useful Notes

- When the game runs, it will execute from the project's root directory (`UbiToronto`). This means any references to data files should be relative to this root directory (e.g., `data/my_asset.png`).
- For macOS development, if you are using VSCode, you can run `make all` and `make run` commands directly from the integrated terminal.

By following these instructions, anyone should be able to set up, build, and run the project successfully.
