#pragma once

#include <string>

class StartupManager {
public:
    static bool IsStartupEnabled();
    static bool EnableStartup();
    static bool DisableStartup();

private:
    static std::wstring GetRegistryPath();
    static std::wstring GetExePath();
};