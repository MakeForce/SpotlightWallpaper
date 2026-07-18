@echo off
echo Initializing MSVC environment...
call "D:\Develop\window\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo Clean build directory...
if exist build rmdir /s /q build

echo Running CMake...
cmake -S . -B build -G "Visual Studio 18 2026" -A x64

echo Building project...
cmake --build build --config Debug

echo Build completed.
pause