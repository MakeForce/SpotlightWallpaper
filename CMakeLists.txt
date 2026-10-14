cmake_minimum_required(VERSION 3.20)

project(SpotlightWallpaper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# spdlog
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/spdlog)

add_executable(SpotlightWallpaper WIN32
    src/main.cpp
    src/App.cpp
    src/Window.cpp
    src/D2DRenderer.cpp
    src/SpotlightManager.cpp
    src/DesktopUtil.cpp
    src/Logger.cpp
    src/TrayIcon.cpp
    src/StartupManager.cpp
    src/ConfigManager.cpp
    src/SettingsDialog.cpp
    app.manifest
)

# 资源文件
target_sources(${PROJECT_NAME} PRIVATE
    res/resources.rc
    res/lang/zh_CN.rc
    res/lang/en_US.rc
)

target_include_directories(SpotlightWallpaper PRIVATE res)
target_precompile_headers(SpotlightWallpaper PRIVATE src/pch.h)

target_link_libraries(SpotlightWallpaper PRIVATE
    d2d1
    dwrite
    windowscodecs
    uuid
    shlwapi
    gdiplus
    spdlog::spdlog
)

set_target_properties(SpotlightWallpaper PROPERTIES
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
    LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
)

if(__MINGW32__) 
    # MinGW 需 -municode 才能使用 wWinMain 入口点
    target_compile_options(${PROJECT_NAME} PRIVATE -municode)
    target_link_options(${PROJECT_NAME} PRIVATE -municode)
endif()

# 支持 RC 资源编译
enable_language(RC)


# UTF-8
if(MSVC)
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /c65001")
else()
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -c 65001")
endif()