# SpotlightWallpaper

[![windows](https://img.shields.io/badge/windows-10~11-orange)](https://www.microsoft.com/zh-cn/windows/)

中文 | [English](README.md)

**SpotlightWallpaper** 是一款轻量级的 Windows 桌面壁纸应用，它能够将 Windows 聚焦 (Windows Spotlight) 的高质量缓存图片自动设置为桌面背景。程序使用纯 C++ 和 Win32 API 编写，通过 Direct2D 进行高效渲染，并利用标准的 WorkerW 窗口层级技术将壁纸窗口完美嵌入到桌面图标下方。

## ✨ 特性

- **自动获取聚焦壁纸**：直接读取系统本地缓存的 Windows Spotlight 图片，无需联网下载。
- **智能图片过滤**：自动过滤掉竖屏、小尺寸或比例不匹配的图片，只保留适合当前屏幕分辨率的高清壁纸。
- **无缝桌面嵌入**：使用标准的 `Progman -> WorkerW` 消息机制，将渲染窗口插入到桌面图标下方，实现原生壁纸体验。
- **高性能渲染**：基于 Direct2D 硬件加速渲染，CPU 占用极低。
- **轻量级与纯净**：无第三方重型 UI 框架依赖，无后台冗余服务，代码简洁高效。

## 🛠️ 环境要求

- **操作系统**: Windows 10 / 11 (x64)
- **编译器**: MSVC 2022 (Visual Studio 2022)
- **构建工具**: CMake 3.15+

## 🚀 编译与运行

项目提供了便捷的批处理脚本用于快速构建。

### 调试模式构建

```bat
build-debug.bat
```

### 发布模式构建

```bat
build-release.bat
```

构建完成后，可执行文件将生成在 `build` 目录中。直接运行 `SpotlightWallpaper.exe` 即可。

## 📁 项目结构

```text
SpotlightWallpaper/
├── .vscode/          # VS Code 配置
├── assets/           # 静态资源 (如图标等)
├── build/            # 编译输出目录
├── res/              # 资源文件 (如图标、字符串表)
├── src/              # 核心 C++ 源代码
├── third_party/      # 第三方库 (如 spdlog)
├── app.manifest      # 应用程序清单 (启用现代视觉样式和高DPI)
├── CMakeLists.txt    # CMake 构建脚本
└── README_zh.md      # 项目说明 (中文)
```

## 开发阶段说明

本项目采用严格的阶段性开发模式，确保每个阶段独立可验收。

- **阶段 1**: 基础窗口 + Direct2D + 固定图片渲染
- **阶段 2**: 替换图片源为 Windows 聚焦高清壁纸
- **阶段 3**: 窗口层级插入测试（纯色窗口，不渲染图片）
- **阶段 4**: 合并渲染与层级逻辑，清理冗余代码

## ⚠️ 注意事项

1. **Windows 聚焦缓存**：程序依赖于 Windows 系统自动下载的 Spotlight 缓存。如果系统未开启“Windows 聚焦”锁屏功能，程序可能无法找到图片。
2. **管理员权限**：在极少数情况下，插入桌面层级可能需要管理员权限，但通常标准用户权限即可运行。

## 📄 许可证

本项目仅供学习和交流使用。
