# SpotlightWallpaper

[![windows](https://img.shields.io/badge/windows-10~11-orange)](https://www.microsoft.com/zh-cn/windows/)

English | [中文](README_ZH.md)

**SpotlightWallpaper** is a lightweight Windows desktop wallpaper application that automatically sets high-quality cached images from Windows Spotlight as the desktop background. Written in pure C++ and Win32 API, it uses Direct2D for efficient rendering and leverages the standard WorkerW window hierarchy technique to seamlessly embed the wallpaper window behind desktop icons.

## ✨ Features

- **Auto-fetch Spotlight Wallpapers**: Reads locally cached Windows Spotlight images directly from the system, no internet download required.
- **Smart Image Filtering**: Automatically filters out portrait, small-sized, or mismatched aspect ratio images, keeping only HD wallpapers suitable for the current screen resolution.
- **Seamless Desktop Embedding**: Uses the standard `Progman -> WorkerW` messaging mechanism to insert the rendering window behind desktop icons for a native wallpaper experience.
- **High-Performance Rendering**: Hardware-accelerated rendering based on Direct2D, resulting in extremely low CPU usage.
- **Lightweight & Clean**: No heavy third-party UI framework dependencies, no redundant background services, clean and efficient code.

## 🛠️ Prerequisites

- **OS**: Windows 10 / 11 (x64)
- **Compiler**: MSVC 2022 (Visual Studio 2022)
- **Build System**: CMake 3.15+

## 🚀 Build & Run

The project provides convenient batch scripts for quick building.

### Debug Build

```bat
build-debug.bat
```

### Release Build

```bat
build-release.bat
```

After building, the executable will be generated in the `build` directory. Simply run `SpotlightWallpaper.exe` to start.

## 📁 Project Structure

```text
SpotlightWallpaper/
├── .vscode/          # VS Code configurations
├── assets/           # Static assets (e.g., icons)
├── build/            # Build output directory
├── res/              # Resource files (e.g., icons, string tables)
├── src/              # Core C++ source code
├── third_party/      # Third-party libraries (e.g., spdlog)
├── app.manifest      # App manifest (enables modern visual styles and high DPI)
├── CMakeLists.txt    # CMake build script
└── README.md         # Project documentation (English)
```

## 📖 Development Phases

This project follows a strict phased development model to ensure each phase is independently verifiable.

- **Phase 1**: Basic Window + Direct2D + Fixed Image Rendering
- **Phase 2**: Replace Image Source with Windows Spotlight HD Wallpapers
- **Phase 3**: Window Hierarchy Insertion Test (Solid Color Window, No Image Rendering)
- **Phase 4**: Merge Rendering and Hierarchy Logic, Clean up Redundant Code

## ⚠️ Notes

1. **Spotlight Cache Dependency**: The application relies on Spotlight caches automatically downloaded by Windows. If "Windows Spotlight" is not enabled on the lock screen, the app may not find any images.
2. **Admin Privileges**: In rare cases, inserting into the desktop hierarchy might require admin privileges, but it typically runs fine with standard user permissions.

## 📄 License

This project is for learning and communication purposes only.
