@echo off
echo Initializing MSVC environment...
call "D:\Develop\window\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo Clean build directory...
if exist build rmdir /s /q build

echo Running CMake...
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building project...
cmake --build build --config Release
if %errorlevel% neq 0 exit /b %errorlevel%

echo Build completed.
pause