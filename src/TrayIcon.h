#pragma once

#include <windows.h>
#include <string>

class TrayIcon {
public:
    TrayIcon();
    ~TrayIcon();

    bool Initialize(HWND hWnd, UINT message);
    void Show();
    void Hide();
    void UpdateMenu(bool autoRefreshEnabled, bool startupEnabled);
    HMENU GetMenu() const { return m_hMenu; }

    static const UINT WM_TRAY_MESSAGE = WM_USER + 1;
    static const UINT ID_EXIT = 1001;
    static const UINT ID_TOGGLE_STARTUP = 1002;
    static const UINT ID_TOGGLE_AUTO_REFRESH = 1003;
    static const UINT ID_SET_REFRESH_INTERVAL = 1004;
    static const UINT ID_NEXT = 1005;

private:
    NOTIFYICONDATA m_nid;
    HMENU m_hMenu;
    bool m_initialized;
};