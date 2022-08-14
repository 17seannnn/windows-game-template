/* === Includes === */
// Windows
#define WIN32_LEAN_AND_MEAN // No MFC
#define INITGUID // For DirectX

#include <windows.h>
#include <windowsx.h>

// DirectX
#include <ddraw.h>

// C/C++
#include <stdio.h>

// Game
#include "Types.h"
#include "Game.h"

/* === Defines === */
// Codes
#define SUCCESS_CODE 0
#define ERROR_CODE 1

// Screen
#define WINDOW_CLASS_NAME "WINDOW_CLASS"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 24

// Keyboard macroses
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

// RGB macroses
#define _RGB16BIT565(R, G, B) ( ((R & 31) << 11) + ((G & 63) << 5) + (B & 31) )
#define _RGB16BIT555(R, G, B) ( ((R & 31) << 10) + ((G & 31) << 5) + (B & 31) )
#define _RGB24BIT(R, G, B) ( ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )

// DirectDraw macroses
#define DDRAW_INIT_STRUCT(STRUCT) { memset(&STRUCT, 0, sizeof(STRUCT)); STRUCT.dwSize = sizeof(STRUCT); }

/* === Globals === */
// App
static s32 g_nExitCode = SUCCESS_CODE;

// Windows
static HINSTANCE g_hInstance = NULL;
static HWND g_hWindow = NULL;

// DirectX
static LPDIRECTDRAW7 g_pDD = NULL;
static LPDIRECTDRAWPALETTE g_pDDPalette = NULL;
static LPDIRECTDRAWSURFACE7 g_pDDScreen = NULL;

/* === Functions === */
static LRESULT CALLBACK WinProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
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
        PostQuitMessage(0);
    } break;

    default:
        return DefWindowProc(hWindow, msg, wParam, lParam);

    }

    return 0;
}

static b32 WinInit(HINSTANCE hInstance)
{
    // Init rand seed
    srand(GetTickCount());

    // Define global hInstance
    g_hInstance = hInstance;

    // Register class
    WNDCLASSEX WinClass;

    WinClass.cbSize = sizeof(WinClass);
    WinClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    WinClass.lpfnWndProc = WinProc;
    WinClass.cbClsExtra = 0;
    WinClass.cbWndExtra = 0;
    WinClass.hInstance = g_hInstance;
    WinClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WinClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WinClass.lpszMenuName = NULL;
    WinClass.lpszClassName = WINDOW_CLASS_NAME;
    WinClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&WinClass))
        return false;

	// Create window
    g_hWindow = CreateWindowEx(NULL, // Useless
                               WINDOW_CLASS_NAME,
                               "Game Template",
                               WS_POPUP|WS_VISIBLE,
                               0, 0,
                               1, 1,
                               NULL, // Parent
                               NULL, // Menu
                               g_hInstance,
                               NULL  // Useless
    );
    if (!g_hWindow)
        return false;

    // Initialize DirectDraw
    if ( FAILED(DirectDrawCreateEx(NULL, (void**)&g_pDD, IID_IDirectDraw7, NULL)) )
        return false;
    if ( FAILED(g_pDD->SetCooperativeLevel(g_hWindow, DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|
                                                       DDSCL_ALLOWMODEX|DDSCL_ALLOWREBOOT)) )
        return false;

    if ( FAILED(g_pDD->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0)) )
        return false;

    /* BPP 8
    // Create palette
    PALETTEENTRY palette[256];
    for (s32 i = 1; i < 255; ++i)
    {
        palette[i].peRed = rand()%256;
        palette[i].peGreen = rand()%256;
        palette[i].peBlue = rand()%256;
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

    if ( FAILED(g_pDD->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256|
                                      DDPCAPS_INITIALIZE,
                                      palette,
                                      &g_pDDPalette,
                                      NULL)) )
        return false;
    */

    // Primary surface
    DDSURFACEDESC2 DDSurfaceDesc;
    memset(&DDSurfaceDesc, 0, sizeof(DDSurfaceDesc));

    DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
    DDSurfaceDesc.dwFlags = DDSD_CAPS;
    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if ( FAILED(g_pDD->CreateSurface(&DDSurfaceDesc, &g_pDDScreen, NULL)) )
        return false;

    /* BPP 8
    if ( FAILED(g_pDDScreen->SetPalette(g_pDDPalette)) )
        return false;
    */

    // Success
    return true;
}

static b32 WinEvents()
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            g_nExitCode = msg.wParam;
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

static void WinShutDown()
{
    /* BPP 8
    if (g_pDDPalette)
    {
        g_pDDPalette->Release();
        g_pDDPalette = NULL;
    }
    */

    if (g_pDDScreen)
    {
        g_pDDScreen->Release();
        g_pDDScreen = NULL;
    }

    if (g_pDD)
    {
        g_pDD->Release();
        g_pDD= NULL;
    }
}

// DEBUG
static void TrySurface()
{
    DDSURFACEDESC2 DDSurfaceDesc;
    memset(&DDSurfaceDesc, 0, sizeof(DDSurfaceDesc));
    DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);

    if ( FAILED(g_pDDScreen->Lock(NULL, &DDSurfaceDesc, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL)) )
        exit(ERROR_CODE);

    s32 memPitch = DDSurfaceDesc.lPitch >> 1; // Divide by 2, because we going to use u16 instead of u8 bytes
    u16* videoBuffer = (u16*)DDSurfaceDesc.lpSurface;

    for (s32 i = 0; i < 1000; ++i)
    {
        s32 x = rand() % SCREEN_WIDTH;
        s32 y = rand() % SCREEN_HEIGHT;
        videoBuffer[y*memPitch + x] = ((15 & 31) << 11) + ((15 & 31));
    }

    if ( FAILED(g_pDDScreen->Unlock(NULL)) )
        exit(ERROR_CODE);
}

// DEBUG
static void PlotPixel16(LPDIRECTDRAWSURFACE7 pSurface, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    DDSURFACEDESC2 DDSurfaceDesc;
    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    if ( FAILED(pSurface->Lock(NULL, &DDSurfaceDesc, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL)) )
        return;

    u16* videoBuffer = (u16*)DDSurfaceDesc.lpSurface;
    videoBuffer[y*(DDSurfaceDesc.lPitch >> 1) + x] = _RGB16BIT565(r, g, b);

    pSurface->Unlock(NULL);
}

#if 1
// DEBUG
static inline void PlotPixelFast16(u16* videoBuffer, s32 pitch16, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    videoBuffer[y*pitch16 + x] = _RGB16BIT565(r, g, b);
}
#endif

#if 0
// DEBUG ONLY FOR 640 PITCH16 AND LOWER THAN PENTIUM II: Y << 9 + Y << 7 = Y*512 + Y*128
static inline void PlotPixelFast16(u16* videoBuffer, s32 pitch16, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    videoBuffer[(y<<9) + (y<<7) + x] = _RGB16BIT565(r, g, b);
}
#endif

static inline void PlotPixel24(u8* videoBuffer, s32 pitch, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    s32 addr = y*pitch + (x+x+x);
    videoBuffer[addr] = (u8)r;
    videoBuffer[addr+1] = (u8)g;
    videoBuffer[addr+2] = (u8)b;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if ( !WinInit(hInstance) )
        return ERROR_CODE;
    if ( !Game::Init() )
        return ERROR_CODE;

    while (Game::Running())
    {
        // DEBUG
        {
            { // Leave on Escape
                if (KEYDOWN(VK_ESCAPE))
                    break;
            }

#if 0
            { // Not fast PlotPixel16
                for (s32 i = 0; i < 1000; ++i)
                    PlotPixel16(g_pDDScreen, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, rand()%256, rand()%256, rand()%256);
            }
#endif

#if 0
            { // Fast PlotPixel16
                DDSURFACEDESC2 DDSurfaceDesc;
                DDRAW_INIT_STRUCT(DDSurfaceDesc);

                g_pDDScreen->Lock(NULL, &DDSurfaceDesc, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
                u16* videoBuffer = (u16*)DDSurfaceDesc.lpSurface;
                s32 pitch16 = DDSurfaceDesc.lPitch >> 1;

                for (s32 i = 0; i < 1000; ++i)
                    PlotPixelFast16(videoBuffer,
                                    pitch16,
                                    rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
                                    rand()%256, rand()%256, rand()%256);

                g_pDDScreen->Unlock(NULL);
            }
#endif

#if 1
            { // PlotPixel24
                DDSURFACEDESC2 DDSurfaceDesc;
                DDRAW_INIT_STRUCT(DDSurfaceDesc);

                g_pDDScreen->Lock(NULL, &DDSurfaceDesc, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL);
                u8* videoBuffer = (u8*)DDSurfaceDesc.lpSurface;
                s32 pitch = DDSurfaceDesc.lPitch;

                for (s32 i = 0; i < 1000; ++i)
                    PlotPixel24(videoBuffer,
                                pitch,
                                rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
                                rand()%256, rand()%256, rand()%256);

                g_pDDScreen->Unlock(NULL);
            }
#endif

        }

        if (!WinEvents())
            break;  // Break on quit event
        Game::Update();
        Game::Render();
    }

    Game::ShutDown();
    WinShutDown();

    return g_nExitCode;
}