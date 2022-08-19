#include "Windows.h"

#define INITGUID // For DirectX in "Graphics.h"
#include "Graphics.h"

#define PALETTE_COLORS 256
#define COLOR_KEY 0

#define DDRAW_INIT_STRUCT(STRUCT) { memset(&STRUCT, 0, sizeof(STRUCT)); STRUCT.dwSize = sizeof(STRUCT); }

#define _RGB16BIT565(R, G, B) ( ((R & 31) << 11) + ((G & 63) << 5) + (B & 31) )
#define _RGB16BIT555(R, G, B) ( ((R & 31) << 10) + ((G & 31) << 5) + (B & 31) )
#define _RGB24BIT(R, G, B) ( ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )
#define _RGB32BIT(A, R, G, B) ( ((A % 255) << 24) + ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )

LPDIRECTDRAW7 Graphics::m_pDDraw = NULL;
LPDIRECTDRAWSURFACE7 Graphics::m_pDDScreen = NULL;
LPDIRECTDRAWSURFACE7 Graphics::m_pDDScreenBack = NULL;
LPDIRECTDRAWPALETTE Graphics::m_pDDPalette = NULL;
LPDIRECTDRAWCLIPPER Graphics::m_pDDClipper = NULL;

b32 Graphics::StartUp(s32 width, s32 height, s32 bpp)
{
    // Initialize DirectDraw
    if ( FAILED(DirectDrawCreateEx(NULL, (void**)&m_pDDraw, IID_IDirectDraw7, NULL)) )
        return false;
    // Cooperative level with window
    if ( FAILED(m_pDDraw->SetCooperativeLevel(Windows::GetWindow(),
                                              DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|
                                              DDSCL_ALLOWMODEX|DDSCL_ALLOWREBOOT)) )
        return false;
    // Display mode
    if ( FAILED(m_pDDraw->SetDisplayMode(width, height, bpp, 0, 0)) )
        return false;

    // Primary surface
    DDSURFACEDESC2 DDSurfaceDesc;
    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    DDSurfaceDesc.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
    DDSurfaceDesc.dwBackBufferCount = 1;

    if ( FAILED(m_pDDraw->CreateSurface(&DDSurfaceDesc, &m_pDDScreen, NULL)) )
        return false;

    // Back surface
    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    if ( FAILED(m_pDDScreen->GetAttachedSurface(&DDSurfaceDesc.ddsCaps, &m_pDDScreenBack)) )
        return false;

    // Palette
    if (bpp == 8)
    {
        PALETTEENTRY palette[PALETTE_COLORS];

        for (s32 i = 1; i < PALETTE_COLORS-1; ++i)
        {
            palette[i].peRed = rand() % 256;
            palette[i].peGreen = rand() % 256;
            palette[i].peBlue = rand() % 256;
            palette[i].peFlags = PC_NOCOLLAPSE;
        }

        palette[0].peRed = 0;
        palette[0].peGreen = 0;
        palette[0].peBlue = 0;
        palette[0].peFlags = PC_NOCOLLAPSE;

        palette[255].peRed = 255;
        palette[255].peGreen = 255;
        palette[255].peBlue = 255;
        palette[255].peFlags = PC_NOCOLLAPSE;

        if ( FAILED(m_pDDraw->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256|DDPCAPS_INITIALIZE, palette, &m_pDDPalette, NULL)) )
            return false;
        if ( FAILED(m_pDDScreen->SetPalette(m_pDDPalette)) )
            return false;
    }

    // Clipper
    RECT clipList[1] = { { 0, 0, width, height } };

    m_pDDClipper = DDrawAttachClipper(m_pDDScreenBack, clipList, 1);
    if (!m_pDDClipper)
        return false;

    return true;
}

void Graphics::ShutDown()
{
    if (m_pDDClipper)
    {
        m_pDDClipper->Release();
        m_pDDClipper = NULL;
    }

    if (m_pDDPalette)
    {
        m_pDDPalette->Release();
        m_pDDPalette = NULL;
    }

    if (m_pDDScreenBack)
    {
        m_pDDScreenBack->Release();
        m_pDDScreenBack = NULL;
    }

    if (m_pDDScreen)
    {
        m_pDDScreen->SetPalette(NULL);
        m_pDDScreen->SetClipper(NULL);
        m_pDDScreen->Release();
        m_pDDScreen = NULL;
    }

    if (m_pDDraw)
    {
        m_pDDraw->Release();
        m_pDDraw= NULL;
    }

}

LPDIRECTDRAWCLIPPER Graphics::DDrawAttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, LPRECT clipList, s32 count)
{
    b32 bResult = true;
    LPDIRECTDRAWCLIPPER pDDClipper;
    LPRGNDATA pRegionData;

    // Create clipper
    if ( FAILED(m_pDDraw->CreateClipper(0, &pDDClipper, NULL)) )
        return NULL;

    // Init region data
    pRegionData = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + sizeof(RECT)*count);
    memcpy(pRegionData->Buffer, clipList, sizeof(RECT)*count);

    // Set region data header
    pRegionData->rdh.dwSize = sizeof(RGNDATAHEADER);
    pRegionData->rdh.iType = RDH_RECTANGLES;
    pRegionData->rdh.nCount = count;
    pRegionData->rdh.nRgnSize = count*sizeof(RECT);

    pRegionData->rdh.rcBound.left = 64000;
    pRegionData->rdh.rcBound.right = -64000;
    pRegionData->rdh.rcBound.top = 64000;
    pRegionData->rdh.rcBound.bottom = -64000;

    // Resize bound
    for (s32 i = 0; i < count; ++i)
    {
        // Left
        if (clipList[i].left < pRegionData->rdh.rcBound.left)
            pRegionData->rdh.rcBound.left = clipList[i].left;
        // Right
        if (clipList[i].right > pRegionData->rdh.rcBound.right)
            pRegionData->rdh.rcBound.right = clipList[i].right;
        // Top
        if (clipList[i].top < pRegionData->rdh.rcBound.top)
            pRegionData->rdh.rcBound.top = clipList[i].top;
        // Bottom
        if (clipList[i].bottom > pRegionData->rdh.rcBound.bottom)
            pRegionData->rdh.rcBound.bottom = clipList[i].bottom;
    }

    // Set clip list
    if ( FAILED(pDDClipper->SetClipList(pRegionData, 0)) )
        bResult = false;

    // Set clipper for surface
    if ( FAILED(pDDSurface->SetClipper(pDDClipper)) )
        bResult = false;

    // Free memory
    free(pRegionData);

    // Return clipper
    if (bResult)
    {
        return pDDClipper;
    }
    else
    {
        pDDClipper->Release();
        return NULL;
    }
}
