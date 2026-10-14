#include "App.h"

App::App() 
    : m_autoRefreshEnabled(false), 
      m_startupEnabled(false), 
      m_refreshInterval(3600) {
}

App::~App() {
}

void App::RefreshWallpaper() {
    LOG_INFO("Refreshing wallpaper...");
    std::wstring wallpaperPath = m_spotlight.GetNextWallpaper();
    if (!wallpaperPath.empty()) {
        m_renderer.LoadImage(wallpaperPath);
        m_window.Invalidate();
        LOG_INFO("Wallpaper refreshed: {}", Logger::WstrToStr(wallpaperPath));
    }
}

void App::OnTrayCommand(UINT cmd) {
    switch (cmd) {
    case TrayIcon::ID_EXIT:
        LOG_INFO("Exit from tray");
        m_config.Save();
        m_window.Close();
        break;

    case TrayIcon::ID_NEXT:
        RefreshWallpaper();
        break;

    case TrayIcon::ID_TOGGLE_STARTUP:
        if (m_startupEnabled) {
            StartupManager::DisableStartup();
            m_startupEnabled = false;
        } else {
            StartupManager::EnableStartup();
            m_startupEnabled = true;
        }
        m_tray.UpdateMenu(m_autoRefreshEnabled, m_startupEnabled);
        break;

    case TrayIcon::ID_TOGGLE_AUTO_REFRESH:
        m_autoRefreshEnabled = !m_autoRefreshEnabled;
        m_config.SetAutoRefreshEnabled(m_autoRefreshEnabled);
        m_config.Save();
        if (m_autoRefreshEnabled) {
            m_window.StartTimer(m_refreshInterval * 1000);
            LOG_INFO("Auto refresh enabled, interval: {}s", m_refreshInterval);
        } else {
            m_window.StopTimer();
            LOG_INFO("Auto refresh disabled");
        }
        m_tray.UpdateMenu(m_autoRefreshEnabled, m_startupEnabled);
        break;

    case TrayIcon::ID_SET_REFRESH_INTERVAL: {
        int interval = m_config.GetRefreshInterval();
        if (SettingsDialog::Show(m_window.GetHandle(), &interval) == IDOK) {
            m_config.SetRefreshInterval(interval);
            m_config.Save();
            m_refreshInterval = interval;
            if (m_autoRefreshEnabled) {
                m_window.StartTimer(m_refreshInterval * 1000);
            }
        }
        break;
    }
    }
}

int App::Run(HINSTANCE hInstance, int nCmdShow) {
    Logger::GetInstance().Init("./", "app.log");
    LOG_INFO("=== SpotlightWallpaper started ===");

    m_config.Load();
    m_autoRefreshEnabled = m_config.IsAutoRefreshEnabled();
    m_refreshInterval = m_config.GetRefreshInterval();
    m_startupEnabled = StartupManager::IsStartupEnabled();

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    LOG_INFO("Screen: {}x{}", width, height);

    LOG_INFO("Scanning Spotlight wallpapers...");
    if (!m_spotlight.Scan()) {
        LOG_ERROR("No Spotlight wallpapers found");
        return 1;
    }

    std::wstring wallpaperPath = m_spotlight.GetNextWallpaper();
    LOG_INFO("Latest wallpaper: {}", Logger::WstrToStr(wallpaperPath));

    LOG_INFO("Creating window...");
    if (!m_window.Initialize(hInstance, width, height)) {
        LOG_ERROR("Window failed");
        return 1;
    }
    LOG_INFO("Window OK");

    LOG_INFO("Initializing D2D...");
    if (!m_renderer.Initialize(m_window.GetHandle())) {
        LOG_ERROR("D2D failed");
        return 1;
    }
    LOG_INFO("D2D OK");

    LOG_INFO("Loading wallpaper...");
    if (!wallpaperPath.empty() &&!m_renderer.LoadImage(wallpaperPath)) {
        LOG_ERROR("Image failed");
        return 1;
    }
    m_window.Invalidate();
    LOG_INFO("Image OK");

    LOG_INFO("Initializing tray icon...");
    if (!m_tray.Initialize(m_window.GetHandle(), TrayIcon::WM_TRAY_MESSAGE)) {
        LOG_ERROR("Tray icon failed");
    }
    m_tray.UpdateMenu(m_autoRefreshEnabled, m_startupEnabled);
    
    m_window.SetTrayIcon(&m_tray);
    
    // 使用 Lambda 表达式捕获 this 指针，以调用类的成员函数
    // 注意：这要求 Window 类的 SetCallback 等方法接受 std::function
    m_window.SetCallback([this](UINT cmd) {
        this->OnTrayCommand(cmd);
    });
    
    m_window.SetRenderCallback([this]() {
        this->m_renderer.Render();
    });
    
    m_window.SetTimerCallback([this]() {
        this->RefreshWallpaper();
    });
    LOG_INFO("Tray icon OK");

    LOG_INFO("Inserting window behind desktop icons...");
    m_desktopUtil.Initialize(m_window.GetHandle());

    if (m_autoRefreshEnabled) {
        m_window.StartTimer(m_refreshInterval * 1000);
    }
    
    LOG_INFO("Main loop...");
    while (m_window.ProcessMessages()) {
    }

    m_config.Save();
    LOG_INFO("=== SpotlightWallpaper exited ===");
    return 0;
}