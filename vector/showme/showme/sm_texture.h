

#ifndef __SM_TEXTURE_H__
#define __SM_TEXTURE_H__

#include "libavz.h"
#include "sm_types.h"
#include <unordered_map>

inline constexpr uint32_t SMArgb(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    return uint32_t(a << 24) | uint32_t(r << 16) | uint32_t(g << 8) | uint32_t(b);
}

class __SMTexture {
    int mWidth;
    int mHeight;
    float u;
    float v;
    IDirectDrawSurface7* texture;
    SMTextureNeedInfo* textNeedInfo;

public:
    ~__SMTexture()
    {
        if (texture) {
            texture->Release();
        }
    }
    __SMTexture(wchar_t chr, SMTextureNeedInfo* _textNeedInfo)
        : mWidth(0)
        , mHeight(0)
        , u(0)
        , v(0)
        , texture(nullptr)
        , textNeedInfo(_textNeedInfo)
    {
        wchar_t str[2] = {chr, L'\0'};
        HDC hDc = CreateCompatibleDC(nullptr);
        HFONT anOldFont = (HFONT)SelectObject(hDc, textNeedInfo->HFont);
        HBRUSH anOldBrush = (HBRUSH)SelectObject(hDc, GetStockObject(BLACK_BRUSH));
        RECT aRect = {0, 0, textNeedInfo->fontW, textNeedInfo->fontH + 1};
        DrawTextW(hDc, str, 1, &aRect, DT_CALCRECT | DT_NOPREFIX);
        if (aRect.right == 0) {
            SelectObject(hDc, anOldBrush);
            SelectObject(hDc, anOldFont);
            return;
        }
        BITMAPINFO aBitmapInfo;
        memset(&aBitmapInfo, 0, sizeof(aBitmapInfo));
        BITMAPINFOHEADER& aHeader = aBitmapInfo.bmiHeader;
        aHeader.biSize = sizeof(aHeader);
        aHeader.biWidth = aRect.right;
        aHeader.biHeight = -textNeedInfo->fontH - 1;
        aHeader.biPlanes = 1;
        aHeader.biBitCount = 32;
        aHeader.biCompression = BI_RGB;
        DWORD* aBits = nullptr;
        HBITMAP aBitmap = CreateDIBSection(hDc, &aBitmapInfo, DIB_RGB_COLORS, (void**)&aBits, nullptr, 0);
        if (!aBitmap) {
            return;
        }
        HBITMAP anOldBitmap = (HBITMAP)SelectObject(hDc, aBitmap);
        Rectangle(hDc, 0, 0, aRect.right, textNeedInfo->fontH + 1);
        SetBkMode(hDc, TRANSPARENT);
        SetTextColor(hDc, RGB(255, 255, 255));
        RECT rect = {0, 1, aRect.right, textNeedInfo->fontH + 1};
        DrawTextW(hDc, str, 1, &rect, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
        GdiFlush();
        SelectObject(hDc, anOldBrush);
        SelectObject(hDc, anOldFont);
        SelectObject(hDc, anOldBitmap);
        mWidth = aRect.right;
        mHeight = textNeedInfo->fontH + 1;
        texture = createTextureSurface(mWidth, mHeight);
        DDSURFACEDESC2 aDesc;
        aDesc.dwSize = sizeof(aDesc);
        texture->Lock(nullptr, &aDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr);
        u = (float)(mWidth - 1) / aDesc.dwWidth;
        v = (float)(mHeight - 1) / aDesc.dwHeight;
        char* dst = (char*)aDesc.lpSurface;
        for (int i = 0; i < mHeight; ++i) {
            DWORD* _dst = (DWORD*)dst;
            DWORD* _bits = aBits;
            for (int j = 0; j < mWidth; ++j)
                *_dst++ = ((*_bits++) << 24) | 0xFFFFFFul;
            aBits += mWidth;
            dst += aDesc.lPitch;
        }
        texture->Unlock(nullptr);
        DeleteObject(aBitmap);
    }
    void draw(DWORD color, int x, int y) const
    {
        SMTexVertex aVertex[4] = {
            {x, y, color, 0, 0},
            {x, y + mHeight, color, 0, v},
            {x + mWidth, y, color, u, 0},
            {x + mWidth, y + mHeight, color, u, v}};
        textNeedInfo->device->SetTexture(0, texture);
        textNeedInfo->device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 452, aVertex, 4, 0);
    }

    IDirectDrawSurface7* createTextureSurface(int theWidth, int theHeight)
    {
        textNeedInfo->device->SetTexture(0, nullptr);
        DDSURFACEDESC2 aDesc;
        IDirectDrawSurface7* aSurface;
        ZeroMemory(&aDesc, sizeof(aDesc));
        aDesc.dwSize = sizeof(aDesc);
        aDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        aDesc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
        aDesc.ddsCaps.dwCaps2 = DDSCAPS2_D3DTEXTUREMANAGE;
        aDesc.dwWidth = theWidth;
        aDesc.dwHeight = theHeight;
        aDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        aDesc.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
        aDesc.ddpfPixelFormat.dwRGBBitCount = 32;
        aDesc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
        aDesc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
        aDesc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
        aDesc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
        textNeedInfo->ddraw->CreateSurface(&aDesc, &aSurface, nullptr);
        return aSurface;
    }
};

class __SMDraw {
    IDirectDrawSurface7* surface = nullptr;

public:
    SMTextureNeedInfo textNeedInfo;
    int fontSize = 20;
    std::wstring fontName = L"宋体";

    void drawRect(int x, int y, int w, int h, DWORD color)
    {
        SMGeoVertex aVertex[4] = {
            {x, y, color},
            {x, y + h, color},
            {x + w, y, color},
            {x + w, y + h, color}};
        textNeedInfo.device->SetTexture(0, nullptr);
        textNeedInfo.device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 68, aVertex, 4, 0);
    }

    SMTextureNeedInfo* getTextureNeedInfo()
    {
        return &textNeedInfo;
    }

    bool refresh()
    {
        void***** p = nullptr;
        auto p1 = p[0x6A9EC0 / sizeof(void*)];
        if (!p1)
            return false;
        auto p2 = p1[0x36C / sizeof(void*)];
        if (!p2)
            return false;
        auto p3 = p2[0x30 / sizeof(void*)];
        if (!p3)
            return false;
        surface = (IDirectDrawSurface7*)p3[0x14 / sizeof(void*)];
        textNeedInfo.device = (IDirect3DDevice7*)p3[0x20 / sizeof(void*)];
        textNeedInfo.ddraw = (IDirectDraw7*)p3[0x10 / sizeof(void*)];
        textNeedInfo.device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        textNeedInfo.device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        textNeedInfo.device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);
        textNeedInfo.device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
        textNeedInfo.device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFG_POINT);
        textNeedInfo.device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        textNeedInfo.device->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        textNeedInfo.device->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        if (textNeedInfo.HFont) {
            return true;
        }
        int script = ANSI_CHARSET;
        textNeedInfo.HFont = CreateFontW(-fontSize, 0, 0, 0, 100, false, false, false, false, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH | FF_DONTCARE, fontName.c_str());

        if (!textNeedInfo.HFont) {
            return false;
        }

        textNeedInfo.fontW = fontSize * 1.1;
        textNeedInfo.fontH = fontSize * 1.1;
        return true;
    }
};

#endif