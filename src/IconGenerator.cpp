#include "pch.h"
#include "IconGenerator.h"
#include "Logger.h"

HICON IconGenerator::CreateAppIcon() {
    const int size = 64;

    HDC hScreen = GetDC(nullptr);
    HDC hMem = CreateCompatibleDC(hScreen);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    VOID* pBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hMem, &bi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMem, hBmp);

    // Background: dark blue gradient (top-left dark, bottom-right light)
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            BYTE r = (BYTE)(20 + (x * 60 / size));
            BYTE g = (BYTE)(30 + (y * 80 / size));
            BYTE b = (BYTE)(80 + ((x + y) * 100 / (size * 2)));
            BYTE* px = (BYTE*)pBits + (y * size + x) * 4;
            px[0] = b;  // B
            px[1] = g;  // G
            px[2] = r;  // R
            px[3] = 255; // A
        }
    }

    // Draw aperture blades (6 blades, rotating)
    int cx = size / 2;
    int cy = size / 2;
    int outerR = size / 2 - 4;
    int innerR = size / 5;

    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 220, 100));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 200, 80));
    HPEN hOldPen = (HPEN)SelectObject(hMem, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hMem, hBrush);

    // 6 aperture blades
    for (int i = 0; i < 6; i++) {
        double angle1 = (i * 60.0) * 3.14159265 / 180.0;
        double angle2 = ((i * 60.0) + 60.0) * 3.14159265 / 180.0;

        POINT pts[4];
        pts[0].x = cx;
        pts[0].y = cy;
        pts[1].x = cx + (int)(outerR * cos(angle1));
        pts[1].y = cy + (int)(outerR * sin(angle1));
        pts[2].x = cx + (int)(innerR * cos(angle1 + 0.3));
        pts[2].y = cy + (int)(innerR * sin(angle1 + 0.3));
        pts[3].x = cx + (int)(innerR * cos(angle2 - 0.3));
        pts[3].y = cy + (int)(innerR * sin(angle2 - 0.3));
        Polygon(hMem, pts, 4);
    }

    // Center highlight
    HBRUSH hCenterBrush = CreateSolidBrush(RGB(255, 255, 220));
    SelectObject(hMem, hCenterBrush);
    Ellipse(hMem, cx - innerR / 2, cy - innerR / 2, cx + innerR / 2, cy + innerR / 2);

    // Cleanup GDI
    SelectObject(hMem, hOldPen);
    SelectObject(hMem, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
    DeleteObject(hCenterBrush);

    // Mask bitmap
    HBITMAP hMask = CreateBitmap(size, size, 1, 1, nullptr);
    HDC hMaskDC = CreateCompatibleDC(hScreen);
    HBITMAP hOldMask = (HBITMAP)SelectObject(hMaskDC, hMask);
    PatBlt(hMaskDC, 0, 0, size, size, WHITENESS);
    SelectObject(hMaskDC, hOldMask);
    DeleteDC(hMaskDC);

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = hMask;
    ii.hbmColor = hBmp;

    HICON hIcon = CreateIconIndirect(&ii);

    SelectObject(hMem, hOldBmp);
    DeleteDC(hMem);
    ReleaseDC(nullptr, hScreen);
    DeleteObject(hBmp);
    DeleteObject(hMask);

    if (hIcon) {
        Logger::Log(L"App icon created");
    } else {
        Logger::LogError(L"Failed to create app icon");
    }

    return hIcon;
}