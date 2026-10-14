#pragma once
#include <vector>
#include <string>
#include <windows.h> // 引入 DWORD 等

class SpotlightManager {
public:
    SpotlightManager();
    ~SpotlightManager();

    bool Scan();
    const std::vector<std::wstring>& GetWallpapers() const;
    std::wstring GetLatestWallpaper() const;
    std::wstring GetNextWallpaper();

private:
    int index = 0;
    ULONG_PTR m_gdiplusToken = 0;
    DWORD m_lastScanTime = 0; // 新增：记录上次成功扫描的时间戳

    bool IsValidWallpaper(const std::wstring& filePath);
    bool GetImageSize(const std::wstring& filePath, UINT& width, UINT& height);
    std::wstring GetAssetsPath();

    std::vector<std::wstring> m_wallpapers;
};