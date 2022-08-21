#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Types.h"

#include <ddraw.h>

#define _RGB16BIT565(R, G, B) ( ((R & 31) << 11) + ((G & 63) << 5) + (B & 31) )
#define _RGB16BIT555(R, G, B) ( ((R & 31) << 10) + ((G & 31) << 5) + (B & 31) )
#define _RGB24BIT(R, G, B) ( ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )
#define _RGB32BIT(A, R, G, B) ( ((A % 255) << 24) + ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )

#ifndef PI
#define PI 3.14159f
#endif

struct SVertex2
{
    s32 x, y;
};

struct Polygon2
{
    s32 state;
    s32 vertexCount;
    s32 x, y;
    s32 vx, vy;
    s32 color;
    SVertex2* aVertex;

    Polygon2() : aVertex(NULL) {}
};

// Static class
class Graphics
{
    static s32 m_screenWidth;
    static s32 m_screenHeight;
    static s32 m_screenBPP;

    static LPDIRECTDRAW7 m_pDDraw;
    static LPDIRECTDRAWSURFACE7 m_pDDScreen;
    static LPDIRECTDRAWSURFACE7 m_pDDScreenBack;
    static LPDIRECTDRAWPALETTE m_pDDPalette;
    static LPDIRECTDRAWCLIPPER m_pDDClipper;

    static f32 sinLook[361];
    static f32 cosLook[361];

public:
    static b32 StartUp(s32 width = 640, s32 height = 480, s32 bpp = 8);
    static void ShutDown();

    // Call this function before Lock()
    static void ClearScreen();
    static void Flip() { m_pDDScreen->Flip(NULL, DDFLIP_WAIT); }
    // Doesn't work with surfaces w/o src color key
    static void Blit(const LPRECT dstRect, const LPDIRECTDRAWSURFACE7 srcSurface, const LPRECT srcRect)
        { m_pDDScreenBack->Blt(dstRect, srcSurface, srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); }

    static b32 LockScreen(u8*& buffer, s32& pitch);
    static b32 LockBack(u8*& buffer, s32& pitch);
    static void UnlockScreen() { m_pDDScreen->Unlock(NULL); }
    static void UnlockBack() { m_pDDScreenBack->Unlock(NULL); }

    static void PlotPixel16(u16* videoBuffer, s32 pitch16, s32 x, s32 y, s32 r, s32 g, s32 b)
        { videoBuffer[y*pitch16 + x] = _RGB16BIT565(r, g, b); }
    static void PlotPixel24(u8* videoBuffer, s32 pitch, s32 x, s32 y, s32 r, s32 g, s32 b);
    static void PlotPixel32(u32* videoBuffer, s32 pitch32, s32 x, s32 y, s32 a, s32 r, s32 g, s32 b)
        { videoBuffer[y*pitch32 + x] = _RGB32BIT(a, r, g, b); }

    static void DrawLine8(u8* videoBuffer, s32 pitch, s32 color, s32 fromX, s32 fromY, s32 toX, s32 toY);
    static void DrawPolygon2(const Polygon2* poly, u8* videoBuffer, s32 pitch);
    static void TranslatePolygon2(Polygon2* poly, s32 dx, s32 dy);
    static void RotatePolygon2(Polygon2* poly, s32 angle);

    static LPDIRECTDRAWSURFACE7 LoadBMP(const char* fileName);

    static s32 GetScreenWidth() { return m_screenWidth; }
    static s32 GetScreenHeight() { return m_screenHeight; }
private:
    static void DDrawError(HRESULT error);
    static LPDIRECTDRAWCLIPPER AttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, const LPRECT clipList, s32 count);
    static LPDIRECTDRAWSURFACE7 CreateSurface(s32 width, s32 height, b32 bVideoMemory = true, b32 bColorKey = true);

    static b32 ClipLine(s32& fromX, s32& fromY, s32& toX, s32& toY);

    // Emulation
    static void EmulationBlit(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h);
    static void EmulationBlitClipped(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h);
};

#endif GRAPHICS_H_