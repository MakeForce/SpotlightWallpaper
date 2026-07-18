#pragma once

class D2DRenderer {
public:
    D2DRenderer();
    ~D2DRenderer();

    bool Initialize(HWND hwnd);
    bool LoadImage(const std::wstring& filePath);
    void Render();
    void Resize(int width, int height);

private:
    ComPtr<ID2D1Factory> m_factory;
    ComPtr<ID2D1HwndRenderTarget> m_renderTarget;
    ComPtr<ID2D1Bitmap> m_bitmap;
    ComPtr<IWICImagingFactory> m_wicFactory;
};