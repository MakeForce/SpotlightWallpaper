# env-prepare.md

## 必需安装
- Visual Studio 2022
  - 使用 C++ 的桌面开发
  - Windows 10 SDK (10.0.19041 或更高)
  - MSVC v143 x64
- CMake（VS自带即可）

## 项目依赖
- Win32 API (user32.lib, gdi32.lib)
- DirectX11 (d3d11.lib, dxgi.lib, d3dcompiler.lib)
- 无第三方库
- 无额外 DLL

## 编译命令（x64）
& 'D:\Develop\window\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config Release

## 运行
直接运行生成的 exe
阶段3、阶段4 建议**管理员权限**以确保窗口层级生效