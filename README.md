# C++ Project Template (VS Code + CMake)

This is a simple and extensible C++ project template for Windows using VS Code and CMake. It supports automatic fetching and building of dependencies and provides full IntelliSense support in VS Code.

The example in the template includes raylib and is fully executable within VS Code, e.g. `RUN AND DEBUG -> Launch MyProject`, or by hitting F5.

---

## ✅ Features

- Single `src/` directory
- Automatically includes all `.cpp`, `.h`, and `.hpp` files
- Dependencies fetched via `FetchContent` (e.g., raylib, imgui, eigen)
- Full IntelliSense support with VS Code CMake Tools
- No need to touch `CMakeLists.txt` except to add new dependencies

---

## 🧰 Required Software

Install the following:

### 1. [Visual Studio Code](https://code.visualstudio.com/)
- With extensions:
  - **CMake Tools**
  - **C/C++ (by Microsoft)**

### 2. [CMake](https://cmake.org/download/)
- Version 3.20 or newer

### 3. [Ninja](https://github.com/ninja-build/ninja/releases)
- Improves build performance
- Can be installed with `winget install Ninja-build.Ninja`
- Add to `PATH` after download if you choose to not use winget

### 4. Visual Studio
- Install **Visual Studio 2022 or later**
- Include the **Desktop development with C++** workload
