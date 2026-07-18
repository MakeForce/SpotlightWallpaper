#include "pch.h"
#include "D2DRenderer.h"
#include "Logger.h"

D2DRenderer::D2DRenderer() {}

D2DRenderer::~D2DRenderer() {
    m_bitmap.Reset();
    m_renderTarget.Reset();
    m_factory.Reset();
    m_wicFactory.Reset();
}

bool D2DRenderer::Initialize(HWND hwnd) {
    Logger::Log(L"D2DRenderer::Initialize");

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_factory.GetAddressOf());
    if (FAILED(hr)) {
        Logger::LogError(L"D2D1CreateFactory failed");
        return false;
    }

    RECT rect;
    GetClientRect(hwnd, &rect);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
    );
    
    hr = m_factory->CreateHwndRenderTarget(
        props,
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)),
        m_renderTarget.GetAddressOf()
    );
    if (FAILED(hr)) {
        Logger::LogError(L"CreateHwndRenderTarget failed");
        return false;
    }

    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(m_wicFactory.GetAddressOf()));
    if (FAILED(hr)) {
        Logger::LogError(L"WIC factory failed");
        return false;
    }

    Logger::Log(L"D2DRenderer OK");
    return true;
}

bool D2DRenderer::LoadImage(const std::wstring& filePath) {
    Logger::Log(L"D2DRenderer::LoadImage: " + filePath);

    m_bitmap.Reset();

    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
    if (FAILED(hr)) {
        Logger::LogError(L"CreateDecoderFromFilename failed");
        return false;
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(hr)) {
        Logger::LogError(L"GetFrame failed");
        return false;
    }

    ComPtr<IWICFormatConverter> converter;
    hr = m_wicFactory->CreateFormatConverter(converter.GetAddressOf());
    if (FAILED(hr)) {
        Logger::LogError(L"CreateFormatConverter failed");
        return false;
    }

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) {
        Logger::LogError(L"Converter init failed");
        return false;
    }

    hr = m_renderTarget->CreateBitmapFromWicBitmap(converter.Get(), nullptr, m_bitmap.GetAddressOf());
    if (FAILED(hr)) {
        Logger::LogError(L"CreateBitmapFromWicBitmap failed");
        return false;
    }

    Logger::Log(L"Image loaded OK");
    return true;
}

void D2DRenderer::Render() {
    m_renderTarget->BeginDraw();
    m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    if (m_bitmap) {
        D2D1_SIZE_F rtSize = m_renderTarget->GetSize();
        D2D1_SIZE_F bitmapSize = m_bitmap->GetSize();

        float scaleX = rtSize.width / bitmapSize.width;
        float scaleY = rtSize.height / bitmapSize.height;
        float scale = max(scaleX, scaleY);

        float x = (rtSize.width - bitmapSize.width * scale) / 2;
        float y = (rtSize.height - bitmapSize.height * scale) / 2;

        D2D1_RECT_F destRect = D2D1::RectF(x, y, x + bitmapSize.width * scale, y + bitmapSize.height * scale);
        m_renderTarget->DrawBitmap(m_bitmap.Get(), destRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    m_renderTarget->EndDraw();
}

void D2DRenderer::Resize(int width, int height) {
    if (m_renderTarget) {
        m_renderTarget->Resize(D2D1::SizeU(width, height));
    }
}