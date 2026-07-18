#include "pch.h"
#include "resource.h"
#include "TrayIcon.h"
#include "Logger.h"

TrayIcon::TrayIcon() : m_initialized(false) {
    ZeroMemory(&m_nid, sizeof(m_nid));
}

TrayIcon::~TrayIcon() {
    if (m_initialized) {
        Shell_NotifyIcon(NIM_DELETE, &m_nid);
    }
}

bool TrayIcon::Initialize(HWND hWnd, UINT message) {
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = hWnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = message;

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    if (!hIcon) {
        Logger::LogError(L"Failed to load icon, fallback to default");
        hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    }
    m_nid.hIcon = hIcon;

    wcscpy_s(m_nid.szTip, L"SpotlightWallpaper");

    if (!Shell_NotifyIcon(NIM_ADD, &m_nid)) {
        Logger::LogError(L"Failed to add tray icon");
        return false;
    }

    m_hMenu = CreatePopupMenu();
    if (!m_hMenu) {
        Logger::LogError(L"Failed to create menu");
        return false;
    }

    m_initialized = true;
    Logger::Log(L"Tray icon initialized");
    return true;
}

void TrayIcon::Show() {
    if (m_initialized) {
        Shell_NotifyIcon(NIM_ADD, &m_nid);
    }
}

void TrayIcon::Hide() {
    if (m_initialized) {
        Shell_NotifyIcon(NIM_DELETE, &m_nid);
    }
}

void TrayIcon::UpdateMenu(bool autoRefreshEnabled, bool startupEnabled) {
    if (!m_hMenu) return;

    while (DeleteMenu(m_hMenu, 0, MF_BYPOSITION)) {}

    std::wstring refreshText = autoRefreshEnabled ? L"Disable Auto Refresh" : L"Enable Auto Refresh";
    std::wstring startupText = startupEnabled ? L"Disable Startup" : L"Enable Startup";

    AppendMenu(m_hMenu, MF_STRING, ID_TOGGLE_AUTO_REFRESH, refreshText.c_str());
    AppendMenu(m_hMenu, MF_STRING, ID_SET_REFRESH_INTERVAL, L"Set Refresh Interval...");
    AppendMenu(m_hMenu, MF_STRING, ID_NEXT, L"Next Wallpaper");
    AppendMenu(m_hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(m_hMenu, MF_STRING, ID_TOGGLE_STARTUP, startupText.c_str());
    AppendMenu(m_hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(m_hMenu, MF_STRING, ID_EXIT, L"Exit");
}