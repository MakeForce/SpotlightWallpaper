#include "pch.h"
#include "DesktopLayerManager.h"
#include "Logger.h"

static HWND g_hWorkerW = NULL;

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HWND hDefView = FindWindowEx(hwnd, 0, L"SHELLDLL_DefView", 0);
    if (hDefView != NULL) {
        g_hWorkerW = FindWindowEx(0, hwnd, L"WorkerW", 0);
        return FALSE;
    }
    return TRUE;
}

bool DesktopLayerManager::InsertWindowBehindIcons(HWND hwnd) {
    Logger::Log(L"DesktopLayerManager::InsertWindowBehindIcons");

    HWND hProgman = FindWindow(L"Progman", NULL);
    if (!hProgman) {
        Logger::LogError(L"Failed to find Progman");
        return false;
    }
    Logger::Log(L"Found Progman: " + std::to_wstring(reinterpret_cast<size_t>(hProgman)));

    SendMessageTimeout(hProgman, 0x052C, 0, 0, SMTO_NORMAL, 0x3E8, NULL);
    Logger::Log(L"Sent 0x052C to Progman");
    
    Sleep(50);

    HWND hDefView = FindWindowEx(hProgman, 0, L"SHELLDLL_DefView", NULL);
    if (hDefView) {
        Logger::Log(L"Found SHELLDLL_DefView: " + std::to_wstring(reinterpret_cast<size_t>(hDefView)));

        HWND hWorkerW = FindWindowEx(hProgman, 0, L"WorkerW", NULL);
        if (hWorkerW) {
            Logger::Log(L"Found WorkerW: " + std::to_wstring(reinterpret_cast<size_t>(hWorkerW)));
            SetParent(hwnd, hWorkerW);
            Logger::Log(L"SetParent to WorkerW");

            ShowWindow(hDefView, SW_HIDE);
            Sleep(0);
            ShowWindow(hDefView, SW_SHOWNORMAL);
            Logger::Log(L"Toggle SHELLDLL_DefView visibility");
            return true;
        } else {
            Logger::LogError(L"Failed to find WorkerW under Progman");
            return false;
        }
    } else {
        Logger::Log(L"No SHELLDLL_DefView, setting parent to Progman");
        SetParent(hwnd, hProgman);
        Logger::Log(L"SetParent to Progman");

        EnumWindows(EnumWindowsProc, 0);
        if (g_hWorkerW) {
            Logger::Log(L"Found g_hWorkerW: " + std::to_wstring(reinterpret_cast<size_t>(g_hWorkerW)));
            ShowWindow(g_hWorkerW, SW_HIDE);
            Logger::Log(L"Hid WorkerW");
            return true;
        }

        Logger::LogError(L"Failed to find WorkerW");
        return false;
    }
}