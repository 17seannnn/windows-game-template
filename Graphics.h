#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Types.h"

#include <ddraw.h>

// Static class
class Graphics
{
    // DirectX
    static LPDIRECTDRAW7 m_pDDraw;
    static LPDIRECTDRAWSURFACE7 m_pDDScreen;
    static LPDIRECTDRAWSURFACE7 m_pDDScreenBack;
    static LPDIRECTDRAWPALETTE m_pDDPalette;
    static LPDIRECTDRAWCLIPPER m_pDDClipper;

public:
    static b32 StartUp(s32 width = 640, s32 height = 480, s32 bpp = 8);
    static void ShutDown();

    static void Flip() { m_pDDScreen->Flip(NULL, DDFLIP_WAIT); }
    // Doesn't work with surfaces w/o src color key
    static void Blit(LPRECT dstRect, LPDIRECTDRAWSURFACE7 srcSurface, LPRECT srcRect)
        { m_pDDScreenBack->Blt(dstRect, srcSurface, srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); }

    static LPDIRECTDRAWSURFACE7 LoadBMP(const char* fileName);
private:
    static void DDrawError(HRESULT error);
    static LPDIRECTDRAWCLIPPER AttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, LPRECT clipList, s32 count);

    static LPDIRECTDRAWSURFACE7 CreateSurface(s32 w, s32 h, b32 bVideoMemory = true, b32 bColorKey = true);
};

#endif GRAPHICS_H_