#include "pch.h"
#include "SettingsDialog.h"
#include "Logger.h"

static int* g_pInterval = nullptr;
static INT_PTR g_result = IDCANCEL;

static LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        CreateWindowEx(0, L"STATIC", L"Refresh interval (minutes):",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 15, 130, 14, hWnd, (HMENU)2001, nullptr, nullptr);

        HWND hEdit = CreateWindowEx(0, L"EDIT", nullptr,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | WS_TABSTOP | ES_AUTOHSCROLL,
            145, 12, 50, 22, hWnd, (HMENU)2002, nullptr, nullptr);

        CreateWindowEx(0, L"STATIC", L"Min 5 min",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            200, 15, 60, 14, hWnd, (HMENU)2003, nullptr, nullptr);

        CreateWindowEx(0, L"BUTTON", L"OK",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            70, 50, 55, 25, hWnd, (HMENU)IDOK, nullptr, nullptr);

        CreateWindowEx(0, L"BUTTON", L"Cancel",
            WS_CHILD | WS_VISIBLE,
            135, 50, 55, 25, hWnd, (HMENU)IDCANCEL, nullptr, nullptr);

        if (g_pInterval) {
            WCHAR buffer[32];
            swprintf_s(buffer, L"%d", *g_pInterval / 60);
            SetWindowText(hEdit, buffer);
        }
        SetFocus(hEdit);
        SendMessage(hEdit, EM_SETSEL, 0, -1);
        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            if (g_pInterval) {
                WCHAR buffer[32];
                GetWindowText(GetDlgItem(hWnd, 2002), buffer, sizeof(buffer) / sizeof(WCHAR));
                int minutes = _wtoi(buffer);
                int seconds = minutes * 60;
                if (seconds < SettingsDialog::MIN_INTERVAL) seconds = SettingsDialog::MIN_INTERVAL;
                if (seconds > SettingsDialog::MAX_INTERVAL) seconds = SettingsDialog::MAX_INTERVAL;
                *g_pInterval = seconds;
                Logger::Log(L"Set refresh interval to " + std::to_wstring(seconds) + L" seconds");
            }
            g_result = IDOK;
            DestroyWindow(hWnd);
            return 0;
        }
        case IDCANCEL:
            g_result = IDCANCEL;
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_CLOSE:
        g_result = IDCANCEL;
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

INT_PTR SettingsDialog::Show(HWND hParent, int* pInterval) {
    g_pInterval = pInterval;
    g_result = IDCANCEL;

    const wchar_t CLASS_NAME[] = L"SpotlightWallpaperSettingsDlg";
    WNDCLASS wc = {};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClass(&wc);

    int width = 280;
    int height = 110;

    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    int x = rcParent.left + (rcParent.right - rcParent.left - width) / 2;
    int y = rcParent.top + (rcParent.bottom - rcParent.top - height) / 2;

    HWND hDlg = CreateWindowEx(
        WS_EX_DLGMODALFRAME,
        CLASS_NAME,
        L"Settings",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y, width, height,
        hParent, nullptr, GetModuleHandle(nullptr), nullptr
    );

    if (!hDlg) {
        Logger::LogError(L"Failed to create settings dialog");
        return IDCANCEL;
    }

    EnableWindow(hParent, FALSE);
    ShowWindow(hDlg, SW_SHOW);
    UpdateWindow(hDlg);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    EnableWindow(hParent, TRUE);
    SetForegroundWindow(hParent);

    UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));
    return g_result;
}