#include "DesktopUtil.h"

DesktopUtil::DesktopUtil() {}

DesktopUtil::~DesktopUtil() {}

static BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM Lparam)
{
    HWND hDefView = FindWindowEx(hwnd, 0, L"SHELLDLL_DefView", 0);
    if (hDefView != 0)
    {
        DesktopUtil *util = reinterpret_cast<DesktopUtil *>(Lparam);
        HWND workerW = FindWindowEx(0, hwnd, L"WorkerW", 0);
        if (util != nullptr)
        {
            util->g_hWorkerW = workerW;
        }
        return FALSE;
    }
    return TRUE;
}

HWND DesktopUtil::shell_in_progman(HWND hProgman)
{
    HWND hShell = FindWindowEx(hProgman, 0, L"SHELLDLL_DefView", 0);
    return hShell;
}

BOOL DesktopUtil::try_find_worker(HWND hProgman, HWND hWnd)
{
    HWND hWorkerW = FindWindowEx(hProgman, 0, L"WorkerW", 0);
    if (hWorkerW != NULL)
    {
        SetParent(hWnd, hWorkerW);
        return TRUE;
    }
    else
        return FALSE;
}

void DesktopUtil::Initialize(HWND hwnd)
{
    target_hWnd = hwnd;
    BringWindowToBack(hwnd);
}

BOOL DesktopUtil::BringWindowToBack(HWND hWnd)
{
    HWND hProgman = FindWindow(L"Progman", 0);                         // 找到PI窗口
    SendMessageTimeout(hProgman, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, 0); // 给它发特殊消息
    if (hWnd == NULL)
        return FALSE;
    HWND hDefView = shell_in_progman(hProgman);
    if (hDefView != NULL)
    {
        if (try_find_worker(hProgman, hWnd))
        {
            ShowWindow(hDefView, SW_HIDE);
            Sleep(0);
            ShowWindow(hDefView, SW_SHOWNORMAL);
            return TRUE;
        }
        else
            return FALSE;
    }
    else
    {
        SetParent(hWnd, hProgman);                                    // 将目标窗口设苦为PM的子窗口
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this)); // 找到第二个workerw窗口并隐藏它
        if (g_hWorkerW)
        {
            ShowWindow(g_hWorkerW, SW_HIDE);
            return TRUE;
        }
        return FALSE;
    }
}

void DesktopUtil::RepairWindowZorder(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
    {
        return;
    }
    /*
        有可能是空的
        HWND hParent_GetParent = GetParent(m_hWnd);
        // 推荐：获取真实的父窗口句柄
        HWND hParent_GetAncestor = GetAncestor(m_hWnd, GA_PARENT);
        // 顺便打印窗口样式，帮助排查问题
        LONG windowStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
    */
    HWND hParent_GetAncestor = GetAncestor(hWnd, GA_PARENT);
    HWND m_desktop = GetDesktopWindow();
    if (hParent_GetAncestor == m_desktop)
        return;

    HWND hProgman = FindWindow(L"Progman", 0);
    HWND hDefView = shell_in_progman(hProgman);
    HWND m_parent;
    if (hDefView != NULL)
    {
        m_parent = FindWindowEx(hProgman, 0, L"WorkerW", 0);
    }
    else
    {
        m_parent = hProgman;
    }
    if (hParent_GetAncestor != m_parent)
    {
        SetParent(hWnd, m_parent);
        if (m_parent == hProgman)
        {
            EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this)); // 找到第二个workerw窗口并隐藏它
            if (g_hWorkerW)
            {
                ShowWindow(g_hWorkerW, SW_HIDE);
            }
        }
        else
        {
            ShowWindow(hDefView, SW_HIDE);
            Sleep(0);
            ShowWindow(hDefView, SW_SHOWNORMAL);
        }
        SetWindowPos(
            hWnd,
            NULL, // 放在 SHELLDLL_DefView 下面
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
        // UpdateWindow(hWnd);
        // RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}