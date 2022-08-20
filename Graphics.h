#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Types.h"

#include <ddraw.h>

// Static class
class Graphics
{
    static LPDIRECTDRAW7 m_pDDraw;
    static LPDIRECTDRAWSURFACE7 m_pDDScreen;
    static LPDIRECTDRAWSURFACE7 m_pDDScreenBack;
    static LPDIRECTDRAWPALETTE m_pDDPalette;
    static LPDIRECTDRAWCLIPPER m_pDDClipper;

public:
    static b32 StartUp(s32 width = 640, s32 height = 480, s32 bpp = 8);
    static void ShutDown();

    static void Flip() { m_pDDScreen->Flip(NULL, DDFLIP_WAIT); }
private:
    static LPDIRECTDRAWCLIPPER DDrawAttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, LPRECT clipList, s32 count);
};

#endif GRAPHICS_H_