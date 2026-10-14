#include "pch.h"
#include "resource.h"
#include "SettingsDialog.h"
#include "Logger.h"

// 控件 ID
#define IDC_EDIT_INTERVAL 2002
#define DIALOG_WIDTH 300
#define DIALOG_HEIGHT 140

// 文件作用域的静态变量，用于在 WndProc 和 Show 之间传递对话框结果
// 因为 Show 是阻塞的模态循环，同一时间只会有一个实例，所以是线程安全的
static INT_PTR s_dialogResult = IDCANCEL;

// 窗口过程 (WndProc)
static LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // 获取通过 lpCreateParams 传入的 pInterval 指针
    int* pInterval = reinterpret_cast<int*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message) {
    case WM_CREATE: {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        // 将传入的指针保存到窗口属性中，以便后续消息使用
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcs->lpCreateParams));
        pInterval = static_cast<int*>(pcs->lpCreateParams);

        HINSTANCE hInst = GetModuleHandle(nullptr);
        WCHAR buf[256];

        // 获取系统现代 UI 字体 (Segoe UI / 微软雅黑)
        NONCLIENTMETRICS ncm = { sizeof(ncm) };
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
        HFONT hFont = CreateFontIndirect(&ncm.lfMessageFont);

        // 1. 标签: 刷新间隔 (分钟)
        LoadString(hInst, IDS_DLG_INTERVAL_LABEL, buf, 256);
        HWND hLabel = CreateWindowEx(0, L"STATIC", buf,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            15, 20, 110, 20, hWnd, nullptr, nullptr, nullptr);
        SendMessage(hLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 2. 输入框
        HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", nullptr,
            WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_TABSTOP | ES_AUTOHSCROLL,
            130, 18, 60, 24, hWnd, (HMENU)IDC_EDIT_INTERVAL, nullptr, nullptr);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 3. 提示文本: 最小 5 分钟
        LoadString(hInst, IDS_DLG_MIN_HINT, buf, 256);
        HWND hHint = CreateWindowEx(0, L"STATIC", buf,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            200, 20, 80, 20, hWnd, nullptr, nullptr, nullptr);
        SendMessage(hHint, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 4. 确定按钮
        LoadString(hInst, IDS_DLG_OK, buf, 256);
        HWND hOk = CreateWindowEx(0, L"BUTTON", buf,
            WS_CHILD | WS_VISIBLE | BS_FLAT,
            20, DIALOG_HEIGHT - 28 - 50, 70, 28, hWnd, (HMENU)IDOK, nullptr, nullptr);
        SendMessage(hOk, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 5. 取消按钮
        LoadString(hInst, IDS_DLG_CANCEL, buf, 256);
        HWND hCancel = CreateWindowEx(0, L"BUTTON", buf,
            WS_CHILD | WS_VISIBLE | BS_FLAT,
            DIALOG_WIDTH - 70 - 20 - 20, DIALOG_HEIGHT - 28 - 50, 70, 28, hWnd, (HMENU)IDCANCEL, nullptr, nullptr);
        SendMessage(hCancel, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 初始化输入框内容 (将秒转换为分钟显示)
        if (pInterval) {
            WCHAR buffer[32];
            swprintf_s(buffer, L"%d", *pInterval / 60);
            SetWindowText(hEdit, buffer);
        }
        
        SetFocus(hEdit);
        SendMessage(hEdit, EM_SETSEL, 0, -1); // 全选文本
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            if (pInterval) {
                WCHAR buffer[32];
                GetWindowText(GetDlgItem(hWnd, IDC_EDIT_INTERVAL), buffer, 32);
                int minutes = _wtoi(buffer);
                int seconds = minutes * 60;
                
                // 边界检查
                if (seconds < SettingsDialog::MIN_INTERVAL) seconds = SettingsDialog::MIN_INTERVAL;
                if (seconds > SettingsDialog::MAX_INTERVAL) seconds = SettingsDialog::MAX_INTERVAL;
                
                *pInterval = seconds;
                LOG_INFO("Set refresh interval to {} seconds", seconds);
            }
            s_dialogResult = IDOK; // 记录结果
            DestroyWindow(hWnd);   // 销毁窗口
            return 0;
        } else if (LOWORD(wParam) == IDCANCEL) {
            s_dialogResult = IDCANCEL;
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_CLOSE:
        s_dialogResult = IDCANCEL;
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0); // 发送退出消息，结束 GetMessage 循环
        return 0;
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);
}

INT_PTR SettingsDialog::Show(HWND hParent, int* pInterval) {
    s_dialogResult = IDCANCEL; // 每次调用重置结果

    const wchar_t CLASS_NAME[] = L"SpotlightWallpaperSettingsDlg";
    
    // 注册窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClass(&wc);

    // 窗口尺寸 (高度增加到 130 以防止按钮被截断)
    int width = DIALOG_WIDTH;
    int height = DIALOG_HEIGHT;

    // 计算居中位置
    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    int x = rcParent.left + (rcParent.right - rcParent.left - width) / 2;
    int y = rcParent.top + (rcParent.bottom - rcParent.top - height) / 2;

    WCHAR title[256];
    LoadString(GetModuleHandle(nullptr), IDS_DLG_TITLE, title, 256);

    // 创建窗口，将 pInterval 作为 lpCreateParams 传入
    HWND hDlg = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        CLASS_NAME,
        title,
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y, width, height,
        hParent, nullptr, GetModuleHandle(nullptr), pInterval
    );

    if (!hDlg) {
        LOG_ERROR("Failed to create settings dialog");
        UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));
        return IDCANCEL;
    }

    // 禁用父窗口，实现模态效果
    EnableWindow(hParent, FALSE);
    ShowWindow(hDlg, SW_SHOW);
    UpdateWindow(hDlg);

    // 模态消息循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_QUIT) {
            break; // 收到 WM_QUIT 退出循环
        }
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 恢复父窗口并置于前台
    EnableWindow(hParent, TRUE);
    SetForegroundWindow(hParent);
    
    // 清理窗口类
    UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));

    return s_dialogResult; // 返回用户在对话框中的操作结果
}