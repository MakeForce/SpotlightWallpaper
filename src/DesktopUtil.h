#pragma once

#include <windows.h>

/**
 * @brief 必须的 Window Style
 *
 * WS_POPUP: 弹出式，全屏无边框
 *
 * WS_CLIPSIBLINGS: 剪裁兄弟窗口，不绘制兄弟窗口
 */
#define wStyle WS_POPUP | WS_CLIPSIBLINGS

/**
 * @brief 必须的 Extended Window Styles
 *
 * WS_EX_TRANSPARENT: 允许鼠标点击穿透到桌面图标（如果需要）
 *
 * WS_EX_LAYERED: 支持分层窗口（透明度）
 *
 * WS_EX_TOOLWINDOW: 关键：不在任务栏和 Win+Tab 任务视图中显示！
 *
 * WS_EX_NOACTIVATE:  关键：永远不会抢夺系统焦点
 */
#define wExStyle WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE

class DesktopUtil
{
public:
    DesktopUtil(/* args */);
    ~DesktopUtil();
    void Initialize(HWND hwnd);
    HWND g_hWorkerW;

private:
    HWND target_hWnd;
    HWND shell_in_progman(HWND hProgman);
    BOOL try_find_worker(HWND hProgman, HWND hWnd);
    BOOL BringWindowToBack(HWND hWnd);
    void RepairWindowZorder(HWND hWnd);
};
