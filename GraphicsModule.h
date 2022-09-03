#ifndef GRAPHICSMODULE_H_
#define GRAPHICSMODULE_H_

/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>

#include "Types.h"
#include "GTMath.h"

/* ====== DEFINES ====== */
#define _RGB16BIT565(R, G, B) ( ((R & 31) << 11) + ((G & 63) << 5) + (B & 31) )
#define _RGB16BIT555(R, G, B) ( ((R & 31) << 10) + ((G & 31) << 5) + (B & 31) )
#define _RGB24BIT(R, G, B) ( ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )
#define _RGB32BIT(A, R, G, B) ( ((A % 255) << 24) + ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )

/* ====== STRUCTURES ====== */
class GraphicsModule
{
    s32 m_screenWidth;
    s32 m_screenHeight;
    s32 m_screenBPP;

    LPDIRECTDRAW7 m_pDDraw;
    LPDIRECTDRAWSURFACE7 m_pDDScreen;
    LPDIRECTDRAWSURFACE7 m_pDDScreenBack;
    LPDIRECTDRAWPALETTE m_pDDPalette;
    LPDIRECTDRAWCLIPPER m_pDDClipper;

public:
    b32 StartUp(HWND hWindow, s32 width = 640, s32 height = 480, s32 bpp = 8);
    void ShutDown();

    s32 GetScreenWidth() const { return m_screenWidth; }
    s32 GetScreenHeight() const { return m_screenHeight; }

    // Call this function before Lock()
    void ClearScreen();
    void Flip() { m_pDDScreen->Flip(NULL, DDFLIP_WAIT); }
    // Doesn't work with surfaces w/o src color key
    void Blit(const LPRECT dstRect, const LPDIRECTDRAWSURFACE7 srcSurface, const LPRECT srcRect)
        { m_pDDScreenBack->Blt(dstRect, srcSurface, srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); }
    b32 DrawText_GDI(s32 x, s32 y, s32 r, s32 g, s32 b, const char* fmt, ...);

    b32 LockScreen(u8*& buffer, s32& pitch);
    b32 LockBack(u8*& buffer, s32& pitch);
    void UnlockScreen() { m_pDDScreen->Unlock(NULL); }
    void UnlockBack() { m_pDDScreenBack->Unlock(NULL); }

    void PlotPixel16(u16* videoBuffer, s32 pitch16, s32 x, s32 y, s32 r, s32 g, s32 b) const
        { videoBuffer[y*pitch16 + x] = _RGB16BIT565(r, g, b); }
    void PlotPixel24(u8* videoBuffer, s32 pitch, s32 x, s32 y, s32 r, s32 g, s32 b) const;
    void PlotPixel32(u32* videoBuffer, s32 pitch32, s32 x, s32 y, s32 a, s32 r, s32 g, s32 b) const
        { videoBuffer[y*pitch32 + x] = _RGB32BIT(a, r, g, b); }

    void DrawLine8(u8* videoBuffer, s32 pitch, s32 color, s32 fromX, s32 fromY, s32 toX, s32 toY) const;
    void DrawPolygon2(const Polygon2* poly, u8* videoBuffer, s32 pitch) const;

    void DrawTriangle(u8* videoBuffer, s32 pitch, s32 color, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3) const;
    void DrawTopTriangle(u8* videoBuffer, s32 pitch, s32 color, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3) const;
    void DrawBottomTriangle(u8* videoBuffer, s32 pitch, s32 color, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3) const;

    void DrawQuad2(u8* videoBuffer, s32 pitch, s32 color, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4) const;

    LPDIRECTDRAWSURFACE7 LoadBMP(const char* fileName) const;
private:
    void DDrawError(HRESULT error) const;
    LPDIRECTDRAWCLIPPER AttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, const LPRECT clipList, s32 count) const;
    LPDIRECTDRAWSURFACE7 CreateSurface(s32 width, s32 height, b32 bVideoMemory = true, b32 bColorKey = true) const;

    b32 ClipLine(s32& fromX, s32& fromY, s32& toX, s32& toY) const;

    /* Emulation */
    void EmulationBlit(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h) const;
    void EmulationBlitClipped(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h) const;
};

extern GraphicsModule g_graphicsModule;

#endif // GRAPHICSMODULE_H_