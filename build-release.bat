@echo off
:: 1. 先关闭回显，再设置编码，避免打印 "Active code page: 65001"
setlocal enabledelayedexpansion
chcp 65001 >nul

echo ==========================================
echo  Initializing build environment...
echo ==========================================

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "_PRODUCTS=Microsoft.VisualStudio.Product.BuildTools"
set "_REQUIRES=Microsoft.VisualStudio.Component.VC.Tools.x86.x64"

:: 2. 尝试获取 MSVC 路径
call :MSVC_Property installationPath

:: 3. 判断是否成功
if %errorlevel% equ 1 (
    echo [Info] MSVC BuildTools not found. Falling back to MinGW...

    :: 检查基础工具
    where.exe cmake >nul 2>&1
    if %errorlevel% neq 0 (
        echo [Error] CMake not found in PATH.
        pause
        exit /b 1
    )
    where.exe g++ >nul 2>&1
    if %errorlevel% neq 0 (
        echo [Error] MinGW ^(g++^) not found in PATH.
        pause
        exit /b 1
    )

    :: 自动选择 Ninja 或 MinGW Makefiles
    where.exe ninja >nul 2>&1
    if %errorlevel% equ 0 (
        call :BUILD "Ninja"
    ) else (
        call :BUILD "MinGW Makefiles"
    )

) else (
    echo MSVC BuildTools with !RESULT_installationPath!
    goto :MSVC_BUILD
)

:: 4. 主逻辑结束，必须跳转，防止代码“穿透”到下面的子程序！
goto :eof

:: ==========================================
:: 子程序区域
:: ==========================================

:MSVC_Property
set "PROP_NAME=%~1"
set "FOUND=0"

:: 2^>nul 用于静默 vswhere 可能产生的错误输出
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products %_PRODUCTS% -requires %_REQUIRES% -property !PROP_NAME! 2^>nul`) do (
    set "RESULT_!PROP_NAME!=%%i"
    set "FOUND=1"
    echo %%i
)

if "!FOUND!"=="0" (
    exit /b 1
)
exit /b 0

:CLEAN_BUILD
echo Cleaning build directories...
if exist build rmdir /s /q build >nul 2>&1
:: 注意：清理特定库的缓存目录
if exist .cmake\deps\spdlog-subbuild rmdir /s /q .cmake\deps\spdlog-subbuild >nul 2>&1
if exist .cmake\deps\spdlog-build rmdir /s /q .cmake\deps\spdlog-build >nul 2>&1
goto :eof

:BUILD
call :CLEAN_BUILD
echo Running CMake with generator: %1
cmake -S . -B build -G %1 -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [Error] CMake configuration failed!
    exit /b %errorlevel%
)

echo Building project...
cmake --build build --config Release --parallel
if %errorlevel% neq 0 (
    echo [Error] Build failed!
    exit /b %errorlevel%
)

echo ==========================================
echo  Build completed successfully!
echo ==========================================
goto :eof

:MSVC_BUILD
:: 初始化 MSVC 环境
call "!RESULT_installationPath!\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
:: 使用 Ninja 编译 (MSVC 环境下通常默认有 Ninja，如果没有会报错)
call :BUILD "Ninja"
goto :eof