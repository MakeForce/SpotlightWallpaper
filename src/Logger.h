#pragma once

#include <fstream>
#include <string>
#include <ctime>

class Logger {
public:
    static void Initialize(const std::wstring& filePath);
    static void Log(const std::wstring& message);
    static void Log(const char* message);
    static void LogError(const std::wstring& message);
    static void LogError(const char* message);

private:
    static std::wofstream s_file;
    static bool s_initialized;
};