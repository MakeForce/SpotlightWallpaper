#include "pch.h"
#include "Logger.h"

std::wofstream Logger::s_file;
bool Logger::s_initialized = false;

void Logger::Initialize(const std::wstring& filePath) {
    s_file.open(filePath, std::ios::app);
    if (s_file.is_open()) {
        s_initialized = true;
        Log(L"Logger initialized");
    }
}

void Logger::Log(const std::wstring& message) {
    if (!s_initialized || !s_file.is_open()) return;
    
    std::time_t now = std::time(nullptr);
    char* timeCStr = std::ctime(&now);
    size_t len = strlen(timeCStr);
    std::wstring timeStr(len, L' ');
    for (size_t i = 0; i < len; i++) {
        timeStr[i] = static_cast<wchar_t>(timeCStr[i]);
    }
    timeStr.pop_back();
    
    s_file << L"[" << timeStr << L"] " << message << std::endl;
    s_file.flush();
}

void Logger::Log(const char* message) {
    std::wstring wmsg(message, message + strlen(message));
    Log(wmsg);
}

void Logger::LogError(const std::wstring& message) {
    Log(L"ERROR: " + message);
}

void Logger::LogError(const char* message) {
    std::wstring wmsg(message, message + strlen(message));
    LogError(wmsg);
}