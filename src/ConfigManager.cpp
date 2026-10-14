#include "pch.h"
#include "ConfigManager.h"
#include "Logger.h"

ConfigManager::ConfigManager() 
    : m_autoRefreshEnabled(false), 
      m_refreshInterval(DEFAULT_REFRESH_INTERVAL) {
}

ConfigManager::~ConfigManager() {
}

std::wstring ConfigManager::GetConfigPath() {
    WCHAR appData[MAX_PATH];
    SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, appData);
    return std::wstring(appData) + L"\\SpotlightWallpaper\\config.ini";
}

bool ConfigManager::Load() {
    std::wstring configPath = GetConfigPath();
    
    m_autoRefreshEnabled = GetPrivateProfileInt(L"Settings", L"AutoRefresh", 0, configPath.c_str()) != 0;
    m_refreshInterval = GetPrivateProfileInt(L"Settings", L"RefreshInterval", DEFAULT_REFRESH_INTERVAL, configPath.c_str());
    
    if (m_refreshInterval < MIN_REFRESH_INTERVAL) {
        m_refreshInterval = MIN_REFRESH_INTERVAL;
    }
    
    LOG_INFO("Loaded config: AutoRefresh={}, RefreshInterval={}", m_autoRefreshEnabled, m_refreshInterval);
    return true;
}

bool ConfigManager::Save() {
    std::wstring configPath = GetConfigPath();
    
    std::wstring dir = configPath.substr(0, configPath.find_last_of(L"\\"));
    CreateDirectory(dir.c_str(), nullptr);
    
    WritePrivateProfileString(L"Settings", L"AutoRefresh", 
        m_autoRefreshEnabled ? L"1" : L"0", configPath.c_str());
    WritePrivateProfileString(L"Settings", L"RefreshInterval", 
        std::to_wstring(m_refreshInterval).c_str(), configPath.c_str());
    
    LOG_INFO("Saved config: AutoRefresh={}, RefreshInterval={}", m_autoRefreshEnabled, m_refreshInterval);
    return true;
}

bool ConfigManager::IsAutoRefreshEnabled() const {
    return m_autoRefreshEnabled;
}

void ConfigManager::SetAutoRefreshEnabled(bool enabled) {
    m_autoRefreshEnabled = enabled;
}

int ConfigManager::GetRefreshInterval() const {
    return m_refreshInterval;
}

void ConfigManager::SetRefreshInterval(int seconds) {
    if (seconds < MIN_REFRESH_INTERVAL) {
        seconds = MIN_REFRESH_INTERVAL;
    }
    m_refreshInterval = seconds;
}