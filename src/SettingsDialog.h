#pragma once

#include <windows.h>

class SettingsDialog {
public:
    // 显示模态设置对话框，pInterval 为输入/输出参数（单位：秒）
    // 返回 IDOK 或 IDCANCEL
    static INT_PTR Show(HWND hParent, int* pInterval);

    // 允许的最小和最大刷新间隔（单位：秒）
    static const int MIN_INTERVAL = 300;    // 5 分钟
    static const int MAX_INTERVAL = 86400;  // 24 小时
};