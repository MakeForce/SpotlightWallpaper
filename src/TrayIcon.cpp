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
        LOG_ERROR("Failed to load icon, fallback to default");
        hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    }
    m_nid.hIcon = hIcon;

    LoadString(hInstance, IDS_APP_NAME, m_nid.szTip, sizeof(m_nid.szTip) / sizeof(WCHAR));

    if (!Shell_NotifyIcon(NIM_ADD, &m_nid)) {
        LOG_ERROR("Failed to add tray icon");
        return false;
    }

    m_hMenu = CreatePopupMenu();
    if (!m_hMenu) {
        LOG_ERROR("Failed to create menu");
        return false;
    }

    m_initialized = true;
    LOG_INFO("Tray icon initialized");
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

    HINSTANCE hInst = GetModuleHandle(nullptr);
    WCHAR buf[256];

    LoadString(hInst, autoRefreshEnabled ? IDS_MENU_AUTO_REFRESH_ON : IDS_MENU_AUTO_REFRESH_OFF, buf, 256);
    AppendMenu(m_hMenu, MF_STRING, ID_TOGGLE_AUTO_REFRESH, buf);

    LoadString(hInst, IDS_MENU_SET_INTERVAL, buf, 256);
    AppendMenu(m_hMenu, MF_STRING, ID_SET_REFRESH_INTERVAL, buf);

    LoadString(hInst, IDS_MENU_NEXT, buf, 256);
    AppendMenu(m_hMenu, MF_STRING, ID_NEXT, buf);

    AppendMenu(m_hMenu, MF_SEPARATOR, 0, nullptr);

    LoadString(hInst, startupEnabled ? IDS_MENU_STARTUP_ON : IDS_MENU_STARTUP_OFF, buf, 256);
    AppendMenu(m_hMenu, MF_STRING, ID_TOGGLE_STARTUP, buf);

    AppendMenu(m_hMenu, MF_SEPARATOR, 0, nullptr);

    LoadString(hInst, IDS_MENU_EXIT, buf, 256);
    AppendMenu(m_hMenu, MF_STRING, ID_EXIT, buf);
}
