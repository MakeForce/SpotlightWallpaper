#pragma once

#include <functional>

class TrayIcon;

class Window {
public:
    Window();
    ~Window();

    bool Initialize(HINSTANCE hInstance, int width, int height);
    HWND GetHandle() const;
    bool ProcessMessages();
    void Close();
    void SetTrayIcon(TrayIcon* tray);
    void SetCallback(std::function<void(UINT)> callback);
    void SetRenderCallback(std::function<void()> callback);
    void Invalidate();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd = nullptr;
    HINSTANCE m_hInstance = nullptr;
    bool m_isRunning = true;
    TrayIcon* m_tray = nullptr;
    std::function<void(UINT)> m_callback;
    std::function<void()> m_renderCallback;
};