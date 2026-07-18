#pragma once

#include <windows.h>

class SettingsDialog {
public:
    static INT_PTR Show(HWND hParent, int* pInterval);
    static const int MIN_INTERVAL = 300;
    static const int MAX_INTERVAL = 86400;

private:
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};