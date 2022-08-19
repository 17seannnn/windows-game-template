#define INITGUID // For DirectX

#include <ddraw.h>

#include "Windows.h"

#define WINDOW_CLASS_NAME "WINDOWCLASS1"

// Graphics
#define PALETTE_COLORS 256
#define COLOR_KEY 0

#define _RGB16BIT565(R, G, B) ( ((R & 31) << 11) + ((G & 63) << 5) + (B & 31) )
#define _RGB16BIT555(R, G, B) ( ((R & 31) << 10) + ((G & 31) << 5) + (B & 31) )
#define _RGB24BIT(R, G, B) ( ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )
#define _RGB32BIT(A, R, G, B) ( ((A % 255) << 24) + ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )

s32 Windows::m_nExitCode = Windows::eExitCode::EC_SUCCESS;
b32 Windows::m_bWindowClosed = false;

HINSTANCE Windows::m_hInstance = NULL;
HWND Windows::m_hWindow = NULL;

LPDIRECTDRAW7 Windows::m_pDDraw = NULL;
LPDIRECTDRAWSURFACE7 Windows::m_pDDScreen = NULL;
LPDIRECTDRAWSURFACE7 Windows::m_pDDScreenBack = NULL;
LPDIRECTDRAWPALETTE Windows::m_pDDPalette = NULL;
LPDIRECTDRAWCLIPPER Windows::m_pDDClipper = NULL;

/* === GRAPHICS === */
LPDIRECTDRAWCLIPPER DDrawAttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, LPRECT clipList, s32 count)
{
    b32 bResult = true;
    LPDIRECTDRAWCLIPPER pDDClipper;
    LPRGNDATA pRegionData;

    // Create clipper
    // TODO remove Windows:: , attach function to graphics
    if ( FAILED(Windows::GetDDraw()->CreateClipper(0, &pDDClipper, NULL)) )
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

/* === WINDOWS === */
b32 Windows::StartUp(HINSTANCE hInstance, const char* title, s32 width, s32 height, s32 bpp)
{
    // Init rand seed
    srand(GetTickCount());

    // Define global hInstance
    m_hInstance = hInstance;

    // Register class
    WNDCLASSEX winClass;

    winClass.cbSize = sizeof(winClass);
    winClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    winClass.lpfnWndProc = WinProc;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = m_hInstance;
    winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = WINDOW_CLASS_NAME;
    winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&winClass))
        return false;

	// Create window
    m_hWindow = CreateWindowEx(NULL,                // Useless
                               WINDOW_CLASS_NAME,   // Class name
                               title,               // Window title
                               WS_POPUP|WS_VISIBLE, // Fullscreen
                               0, 0,                // Top left corner
                               1, 1,                // width/height
                               NULL,                // Parent
                               NULL,                // Menu
                               m_hInstance,         // Instance
                               NULL                 // Useless
    );
    if (!m_hWindow)
        return false;

#ifdef _DEBUG // TODO should be in Log
    //if (!AllocConsole())
    //    return false;
    //g_consoleBuffer = (char*)malloc(CONSOLE_BUFSIZE);
#endif

    // Initialize DirectDraw
    if ( FAILED(DirectDrawCreateEx(NULL, (void**)&m_pDDraw, IID_IDirectDraw7, NULL)) )
        return false;
    // Cooperative level with window
    if ( FAILED(m_pDDraw->SetCooperativeLevel(m_hWindow, DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|
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

    // Success
    return true;
}

void Windows::ShutDown()
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

#ifdef _DEBUG // TODO Should be in Log
    //free(g_consoleBuffer);
    //FreeConsole();
#endif
}

b32 Windows::HandleEvents()
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // Peek message from any window
    {
        if (msg.message == WM_QUIT)
        {
            m_nExitCode = msg.wParam;
            return false;
        }

        TranslateMessage(&msg); // Keyboard input
        DispatchMessage(&msg);  // Send this to WinProc
    }

    return true;
}

LRESULT CALLBACK Windows::WinProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    
    case WM_CREATE:
    {} break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWindow, &ps);
        EndPaint(hWindow, &ps);
    } break;

    case WM_DESTROY:
    {
        m_bWindowClosed = true;
        PostQuitMessage(0);
    } break;

    default:
        return DefWindowProc(hWindow, msg, wParam, lParam);

    }

    return 0;
}