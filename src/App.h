#pragma once

#include <windows.h>
#include <string>
#include <functional>

// 引入项目原有的头文件
#include "Window.h"
#include "D2DRenderer.h"
#include "SpotlightManager.h"
#include "DesktopUtil.h"
#include "Logger.h"
#include "TrayIcon.h"
#include "StartupManager.h"
#include "ConfigManager.h"
#include "SettingsDialog.h"

class App {
public:
    App();
    ~App();

    // 运行应用程序主循环
    int Run(HINSTANCE hInstance, int nCmdShow);

private:
    // 业务逻辑方法
    void RefreshWallpaper();
    void OnTrayCommand(UINT cmd);

    // 组件成员变量 (替代原有的全局变量)
    Window m_window;
    TrayIcon m_tray;
    D2DRenderer m_renderer;
    SpotlightManager m_spotlight;
    ConfigManager m_config;
    DesktopUtil m_desktopUtil;

    // 状态成员变量
    bool m_autoRefreshEnabled;
    bool m_startupEnabled;
    int m_refreshInterval;
};