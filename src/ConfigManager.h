#pragma once

#include <string>

class ConfigManager {
public:
    static const int MIN_REFRESH_INTERVAL = 300;
    static const int DEFAULT_REFRESH_INTERVAL = 3600;

    ConfigManager();
    ~ConfigManager();

    bool Load();
    bool Save();

    bool IsAutoRefreshEnabled() const;
    void SetAutoRefreshEnabled(bool enabled);

    int GetRefreshInterval() const;
    void SetRefreshInterval(int seconds);

private:
    std::wstring GetConfigPath();

    bool m_autoRefreshEnabled;
    int m_refreshInterval;
};