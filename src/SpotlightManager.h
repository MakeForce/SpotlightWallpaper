#pragma once

#include <vector>
#include <string>

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
    bool IsValidWallpaper(const std::wstring& filePath);
    bool GetImageSize(const std::wstring& filePath, UINT& width, UINT& height);
    std::wstring GetAssetsPath();

    std::vector<std::wstring> m_wallpapers;
};