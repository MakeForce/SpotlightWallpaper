#include "pch.h"
#include "SpotlightManager.h"
#include "Logger.h"
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

static double GetScreenRatio() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    return static_cast<double>(screenWidth) / screenHeight;
}

static bool IsRatioMatchScreen(UINT width, UINT height) {
    double screenRatio = GetScreenRatio();
    double imageRatio = static_cast<double>(width) / height;
    double diff = abs(imageRatio - screenRatio);
    return diff <= 0.05;
}

SpotlightManager::SpotlightManager() {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
}

SpotlightManager::~SpotlightManager() {
    if (m_gdiplusToken != 0) {
        GdiplusShutdown(m_gdiplusToken);
    }
}

bool SpotlightManager::GetImageSize(const std::wstring& filePath, UINT& width, UINT& height) {
    Image image(filePath.c_str());
    if (image.GetLastStatus() != Ok) {
        return false;
    }
    width = image.GetWidth();
    height = image.GetHeight();
    return true;
}

std::wstring SpotlightManager::GetAssetsPath() {
    wchar_t appData[MAX_PATH];
    if (!SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, appData)) {
        std::wstring localAppData(appData);
        return localAppData + L"\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets";
    }
    return L"";
}

bool SpotlightManager::IsValidWallpaper(const std::wstring& filePath) {
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (!GetFileAttributesEx(filePath.c_str(), GetFileExInfoStandard, &fileData)) {
        return false;
    }

    LARGE_INTEGER size;
    size.HighPart = fileData.nFileSizeHigh;
    size.LowPart = fileData.nFileSizeLow;

    if (size.QuadPart < 100 * 1024) return false;
    if (size.QuadPart > 20 * 1024 * 1024) return false;

    UINT width = 0, height = 0;
    if (!GetImageSize(filePath, width, height)) {
        return false;
    }

    return IsRatioMatchScreen(width, height);
}

bool SpotlightManager::Scan() {
    m_wallpapers.clear();

    std::wstring assetsPath = GetAssetsPath();
    Logger::Log(L"Scanning Spotlight assets: " + assetsPath);

    if (assetsPath.empty()) {
        Logger::LogError(L"Failed to get assets path");
        return false;
    }

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((assetsPath + L"\\*").c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        Logger::LogError(L"FindFirstFile failed, path may not exist");
        return false;
    }

    int totalFiles = 0;
    int validFiles = 0;

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

        totalFiles++;
        std::wstring fileName(findData.cFileName);
        std::wstring fullPath = assetsPath + L"\\" + fileName;

        if (IsValidWallpaper(fullPath)) {
            m_wallpapers.push_back(fullPath);
            validFiles++;
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);

    std::sort(m_wallpapers.begin(), m_wallpapers.end(), [](const std::wstring& a, const std::wstring& b) {
        WIN32_FILE_ATTRIBUTE_DATA da, db;
        GetFileAttributesEx(a.c_str(), GetFileExInfoStandard, &da);
        GetFileAttributesEx(b.c_str(), GetFileExInfoStandard, &db);
        return CompareFileTime(&da.ftLastWriteTime, &db.ftLastWriteTime) > 0;
    });

    Logger::Log(L"Found " + std::to_wstring(totalFiles) + L" files, " + std::to_wstring(validFiles) + L" valid wallpapers");

    size_t count = m_wallpapers.size();
    if (count > 3) count = 3;
    for (size_t i = 0; i < count; i++) {
        Logger::Log(L"Wallpaper " + std::to_wstring(i + 1) + L": " + m_wallpapers[i]);
    }

    return !m_wallpapers.empty();
}

const std::vector<std::wstring>& SpotlightManager::GetWallpapers() const {
    return m_wallpapers;
}

std::wstring SpotlightManager::GetLatestWallpaper() const {
    if (m_wallpapers.empty()) return L"";
    return m_wallpapers[0];
}

std::wstring SpotlightManager::GetNextWallpaper() {
    if (m_wallpapers.empty()) return L"";
    std::wstring path = m_wallpapers[index];
    index = index + 1;
    if (index >= m_wallpapers.size()) {
        index = 0;
    }
    return path;
}