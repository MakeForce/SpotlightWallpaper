#include "pch.h"
#include "StartupManager.h"
#include "Logger.h"

std::wstring StartupManager::GetRegistryPath() {
    return L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
}

std::wstring StartupManager::GetExePath() {
    WCHAR path[MAX_PATH];
    GetModuleFileName(nullptr, path, MAX_PATH);
    return std::wstring(path);
}

bool StartupManager::IsStartupEnabled() {
    HKEY hKey = nullptr;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, GetRegistryPath().c_str(), 0, KEY_READ, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }

    WCHAR value[MAX_PATH];
    DWORD size = sizeof(value);
    result = RegQueryValueEx(hKey, L"SpotlightWallpaper", nullptr, nullptr, reinterpret_cast<LPBYTE>(value), &size);
    
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS) {
        Logger::Log(L"Startup is enabled");
        return true;
    }
    
    Logger::Log(L"Startup is disabled");
    return false;
}

bool StartupManager::EnableStartup() {
    HKEY hKey = nullptr;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, GetRegistryPath().c_str(), 0, KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        Logger::LogError(L"Failed to open registry key");
        return false;
    }

    std::wstring exePath = GetExePath();
    result = RegSetValueEx(hKey, L"SpotlightWallpaper", 0, REG_SZ, 
        reinterpret_cast<const BYTE*>(exePath.c_str()), 
        (exePath.size() + 1) * sizeof(WCHAR));
    
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS) {
        Logger::Log(L"Startup enabled");
        return true;
    }
    
    Logger::LogError(L"Failed to enable startup");
    return false;
}

bool StartupManager::DisableStartup() {
    HKEY hKey = nullptr;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, GetRegistryPath().c_str(), 0, KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        Logger::LogError(L"Failed to open registry key");
        return false;
    }

    result = RegDeleteValue(hKey, L"SpotlightWallpaper");
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) {
        Logger::Log(L"Startup disabled");
        return true;
    }
    
    Logger::LogError(L"Failed to disable startup");
    return false;
}