cmake_minimum_required(VERSION 3.20)

project(SpotlightWallpaper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(SpotlightWallpaper WIN32
    src/main.cpp
    src/Window.cpp
    src/D2DRenderer.cpp
    src/SpotlightManager.cpp
    src/DesktopLayerManager.cpp
    src/Logger.cpp
    src/TrayIcon.cpp
    src/StartupManager.cpp
    src/ConfigManager.cpp
    src/SettingsDialog.cpp
    src/resources.rc
)

target_precompile_headers(SpotlightWallpaper PRIVATE src/pch.h)

target_link_libraries(SpotlightWallpaper PRIVATE
    d2d1
    dwrite
    windowscodecs
    uuid
    shlwapi
    gdiplus
)

set_target_properties(SpotlightWallpaper PROPERTIES
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
    LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
)