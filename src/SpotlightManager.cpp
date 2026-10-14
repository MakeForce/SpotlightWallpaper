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
    LOG_INFO("Scanning Spotlight assets: {}", Logger::WstrToStr(assetsPath));

    if (assetsPath.empty()) {
        LOG_ERROR("Failed to get assets path");
        return false;
    }

    // 使用宽字符 API 防止中文路径问题
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW((assetsPath + L"\\*").c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        LOG_ERROR("FindFirstFile failed, path may not exist");
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
    } while (FindNextFileW(hFind, &findData));
    
    FindClose(hFind);

    // 按最后修改时间降序排序 (最新的在前面)
    std::sort(m_wallpapers.begin(), m_wallpapers.end(), [](const std::wstring& a, const std::wstring& b) {
        WIN32_FILE_ATTRIBUTE_DATA da, db;
        GetFileAttributesExW(a.c_str(), GetFileExInfoStandard, &da);
        GetFileAttributesExW(b.c_str(), GetFileExInfoStandard, &db);
        return CompareFileTime(&da.ftLastWriteTime, &db.ftLastWriteTime) > 0;
    });

    LOG_INFO("Found {} files, {} valid wallpapers", totalFiles, validFiles);
    
    // 记录扫描成功的时间
    m_lastScanTime = GetTickCount(); 
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
    // 1. 如果列表为空，或者距离上次扫描已经超过 1 小时 (3600000 ms)
    // 自动触发扫描，以感知系统新增的壁纸
    if (m_wallpapers.empty() || (GetTickCount() - m_lastScanTime > 3600000)) {
        LOG_INFO("Auto-rescanning to detect new wallpapers...");
        if (!Scan()) {
            return L"";
        }
    }

    if (m_wallpapers.empty()) return L"";
    const int maxAttempts = static_cast<int>(m_wallpapers.size());
    
    int current = index;
    current = current + 1;
    if (current >= maxAttempts) {
        current = 0;
    }

    while (current < maxAttempts)
    {
        // 获取当前索引对应的路径
        std::wstring path = m_wallpapers[index];
        // 实时验证文件是否还存在（应对系统清理）
        if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
            // 文件存在，返回该路径
            index = current;
            return path;
        }
        current++;
    }
    current = 0;
    while (current <= index)
    {
        // 获取当前索引对应的路径
        std::wstring path = m_wallpapers[index];
        // 实时验证文件是否还存在（应对系统清理）
        if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
            // 文件存在，返回该路径
            index = current;
            return path;
        }
        current++;
    }
    
    // 3. 如果整个列表里的文件都被删光了，强制重新扫描一次
    LOG_WARN("All wallpapers in list are missing. Forcing rescan...");
    if (Scan()) {
        // 重扫成功后，再次尝试获取（此时 index 已在 Scan 中被重置为 0）
        return GetNextWallpaper(); 
    }
    
    return L"";
}
