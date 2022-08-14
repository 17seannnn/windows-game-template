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
#define SCREEN_BPP 16

// Keyboard macroses
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

// RGB macroses
#define _RGB16BIT565(R, G, B) ( ((R & 31) << 11) + ((G & 63) << 5) + (B & 31) )
#define _RGB16BIT555(R, G, B) ( ((R & 31) << 10) + ((G & 31) << 5) + (B & 31) )
#define _RGB24BIT(R, G, B) ( ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )
#define _RGB32BIT(A, R, G, B) ( ((A % 255) << 24) + ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )

// DirectDraw macroses
#define DDRAW_INIT_STRUCT(STRUCT) { memset(&STRUCT, 0, sizeof(STRUCT)); STRUCT.dwSize = sizeof(STRUCT); }

/* === Globals === */
// App
static s32 g_nExitCode = SUCCESS_CODE;

// Windows
static HINSTANCE g_hInstance = NULL;
static HWND g_hWindow = NULL;
static bool g_bWindowClosed = false;

// DirectX
static LPDIRECTDRAW7 g_pDD = NULL;
static LPDIRECTDRAWSURFACE7 g_pDDScreen = NULL;
static LPDIRECTDRAWSURFACE7 g_pDDScreenBack = NULL;

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

    // Primary surface
    DDSURFACEDESC2 DDSurfaceDesc;
    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    DDSurfaceDesc.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
    DDSurfaceDesc.dwBackBufferCount = 1;

    if ( FAILED(g_pDD->CreateSurface(&DDSurfaceDesc, &g_pDDScreen, NULL)) )
        return false;

    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    if ( FAILED(g_pDDScreen->GetAttachedSurface(&DDSurfaceDesc.ddsCaps, &g_pDDScreenBack)) )
        return false;

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
    if (g_pDDScreenBack)
    {
        g_pDDScreenBack->Release();
        g_pDDScreenBack = NULL;
    }

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

static inline void PlotPixel16(u16* videoBuffer, s32 pitch16, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    videoBuffer[y*pitch16 + x] = _RGB16BIT565(r, g, b);
}

static inline void PlotPixel24(u8* videoBuffer, s32 pitch, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    s32 addr = y*pitch + (x+x+x);
    videoBuffer[addr] = (u8)r;
    videoBuffer[addr+1] = (u8)g;
    videoBuffer[addr+2] = (u8)b;
}

static inline void PlotPixel32(u32* videoBuffer, s32 pitch32, s32 x, s32 y, s32 a, s32 r, s32 g, s32 b)
{
    videoBuffer[y*pitch32 + x] = _RGB32BIT(a, r, g, b);
}

static void BlitBitMap(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h)
{
    videoBuffer += posY*pitch32 + posX; // Start position for videoBuffer pointer

    for (s32 y = 0; y < h; ++y)
    {
        for (s32 x = 0; x < w; ++x)
        {
            u32 pixel;
            if ((pixel = bitMap[x])) // Plot opaque pixels only
                videoBuffer[x] = pixel;
        }
        videoBuffer += pitch32;
        bitMap += w;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if ( !WinInit(hInstance) )
        return ERROR_CODE;
    if ( !Game::Init() )
        return ERROR_CODE;

    { // DEBUG INIT
        DDSURFACEDESC2 DDSurfaceDesc;
        DDRAW_INIT_STRUCT(DDSurfaceDesc);

        g_pDDScreenBack->Lock(NULL, &DDSurfaceDesc, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);
        u16* videoBuffer = (u16*)DDSurfaceDesc.lpSurface;
        s32 pitch16 = DDSurfaceDesc.lPitch >> 1;

        for (s32 y = 0; y < SCREEN_HEIGHT; ++y)
        {
            u32 color = _RGB16BIT565(0, y >> 3, 0); // Green gradient
            color |= color << 16; // Two WORDs in one DWORD

            // Real game programmer stuff
            _asm
            {
                CLD                                     ; clear direction
                MOV     EAX, color                      ; color -> EAX
                MOV     ECX, (SCREEN_WIDTH >> 1)        ; we fill videoBuffer with DWORD so we'll fill it SCREEN_WIDTH/2 times
                MOV     EDI, videoBuffer                ; videoBuffer -> EDI
                REP     STOSD                           ; move stuff
            }

            videoBuffer += pitch16;
        }

        g_pDDScreenBack->Unlock(NULL);
    }

    while (Game::Running())
    {
        { // DEBUG QUIT
            // Leave on Escape
            if (KEYDOWN(VK_ESCAPE))
            {
                g_bWindowClosed = true;
                PostMessage(g_hWindow, WM_CLOSE, 0, 0);
            }
        }

        if (!WinEvents())
            break;  // Break on quit event
        Game::Update();
        if (g_bWindowClosed)
            break; // DirectX may want to get window but it can be closed
        Game::Render();

        { // DEBUG MAIN
            RECT dstRect, srcRect;

            dstRect.left = 0;
            dstRect.right = 10;
            dstRect.top = 0;
            dstRect.bottom = 10;

            srcRect.left = 0;
            srcRect.right = SCREEN_WIDTH;
            srcRect.top = 0;
            srcRect.bottom = SCREEN_HEIGHT;

            g_pDDScreen->Blt(&dstRect, g_pDDScreenBack, &srcRect, DDBLT_WAIT, NULL);
        }

        // Flip buffers
        // DEBUG uncomment
        //g_pDDScreen->Flip(NULL, DDFLIP_WAIT);
    }

    Game::ShutDown();
    WinShutDown();

    return g_nExitCode;
}