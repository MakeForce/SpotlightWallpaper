#include "pch.h"
#include "Window.h"
#include "D2DRenderer.h"
#include "SpotlightManager.h"
#include "DesktopLayerManager.h"
#include "Logger.h"
#include "TrayIcon.h"
#include "StartupManager.h"
#include "ConfigManager.h"
#include "SettingsDialog.h"

Window g_window;
TrayIcon g_tray;
D2DRenderer g_renderer;
SpotlightManager g_spotlight;
ConfigManager g_config;

bool g_autoRefreshEnabled = false;
bool g_startupEnabled = false;
int g_refreshInterval = 3600;
DWORD g_lastRefreshTime = 0;

void RefreshWallpaper() {
    Logger::Log(L"Refreshing wallpaper...");
    
    if (!g_spotlight.Scan()) {
        Logger::LogError(L"No Spotlight wallpapers found during refresh");
        return;
    }
    
    std::wstring wallpaperPath = g_spotlight.GetNextWallpaper();
    if (!wallpaperPath.empty()) {
        g_renderer.LoadImage(wallpaperPath);
        g_window.Invalidate();
        Logger::Log(L"Wallpaper refreshed: " + wallpaperPath);
    }
    
    g_lastRefreshTime = GetTickCount();
}

void OnTrayCommand(UINT cmd) {
    switch (cmd) {
    case TrayIcon::ID_EXIT:
        Logger::Log(L"Exit from tray");
        g_config.Save();
        g_window.Close();
        break;

    case TrayIcon::ID_NEXT:
        RefreshWallpaper();
        break;

    case TrayIcon::ID_TOGGLE_STARTUP:
        if (g_startupEnabled) {
            StartupManager::DisableStartup();
            g_startupEnabled = false;
        } else {
            StartupManager::EnableStartup();
            g_startupEnabled = true;
        }
        g_tray.UpdateMenu(g_autoRefreshEnabled, g_startupEnabled);
        break;

    case TrayIcon::ID_TOGGLE_AUTO_REFRESH:
        g_autoRefreshEnabled = !g_autoRefreshEnabled;
        g_config.SetAutoRefreshEnabled(g_autoRefreshEnabled);
        g_config.Save();
        if (g_autoRefreshEnabled) {
            g_lastRefreshTime = GetTickCount();
            Logger::Log(L"Auto refresh enabled, interval: " + std::to_wstring(g_refreshInterval) + L"s");
        } else {
            Logger::Log(L"Auto refresh disabled");
        }
        g_tray.UpdateMenu(g_autoRefreshEnabled, g_startupEnabled);
        break;

    case TrayIcon::ID_SET_REFRESH_INTERVAL: {
        int interval = g_config.GetRefreshInterval();
        if (SettingsDialog::Show(g_window.GetHandle(), &interval) == IDOK) {
            g_config.SetRefreshInterval(interval);
            g_config.Save();
            g_refreshInterval = interval;
            g_lastRefreshTime = GetTickCount();
        }
        break;
    }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WCHAR exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);
    std::wstring path(exePath);
    size_t lastSlash = path.find_last_of(L"\\/");
    std::wstring logPath = path.substr(0, lastSlash) + L"\\SpotlightWallpaper.log";
    
    Logger::Initialize(logPath);
    Logger::Log(L"=== SpotlightWallpaper started ===");

    g_config.Load();
    g_autoRefreshEnabled = g_config.IsAutoRefreshEnabled();
    g_refreshInterval = g_config.GetRefreshInterval();
    g_startupEnabled = StartupManager::IsStartupEnabled();

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    Logger::Log(L"Screen: " + std::to_wstring(width) + L"x" + std::to_wstring(height));

    Logger::Log(L"Scanning Spotlight wallpapers...");
    if (!g_spotlight.Scan()) {
        Logger::LogError(L"No Spotlight wallpapers found");
        return 1;
    }

    std::wstring wallpaperPath = g_spotlight.GetNextWallpaper();
    Logger::Log(L"Latest wallpaper: " + wallpaperPath);

    Logger::Log(L"Creating window...");
    if (!g_window.Initialize(hInstance ,width, height)) {
        Logger::LogError(L"Window failed");
        return 1;
    }
    Logger::Log(L"Window OK");

    Logger::Log(L"Initializing D2D...");
    if (!g_renderer.Initialize(g_window.GetHandle())) {
        Logger::LogError(L"D2D failed");
        return 1;
    }
    Logger::Log(L"D2D OK");

    Logger::Log(L"Loading wallpaper...");
    if (!g_renderer.LoadImage(wallpaperPath)) {
        Logger::LogError(L"Image failed");
        return 1;
    }
    g_window.Invalidate();
    Logger::Log(L"Image OK");

    Logger::Log(L"Initializing tray icon...");
    if (!g_tray.Initialize(g_window.GetHandle(), TrayIcon::WM_TRAY_MESSAGE)) {
        Logger::LogError(L"Tray icon failed");
    }
    g_tray.UpdateMenu(g_autoRefreshEnabled, g_startupEnabled);
    g_window.SetTrayIcon(&g_tray);
    g_window.SetCallback(OnTrayCommand);
    g_window.SetRenderCallback([]() {
        g_renderer.Render();
    });
    Logger::Log(L"Tray icon OK");

    Logger::Log(L"Inserting window behind desktop icons...");
    if (!DesktopLayerManager::InsertWindowBehindIcons(g_window.GetHandle())) {
        Logger::LogError(L"Failed to insert window behind icons");
    } else {
        Logger::Log(L"Window inserted behind icons successfully");
    }

    g_lastRefreshTime = GetTickCount();
    Logger::Log(L"Main loop...");

    while (g_window.ProcessMessages()) {
        if (g_autoRefreshEnabled) {
            DWORD now = GetTickCount();
            if (now - g_lastRefreshTime >= g_refreshInterval * 1000) {
                RefreshWallpaper();
            }
        }
    }

    g_config.Save();
    Logger::Log(L"=== SpotlightWallpaper exited ===");
    return 0;
}