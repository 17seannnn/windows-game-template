/* === Includes === */
// Windows
#define WIN32_LEAN_AND_MEAN // No MFC
#define INITGUID // For DirectX

#include <windows.h>
#include <windowsx.h>

// DirectX
#include <ddraw.h>

// Game
#include "Types.h"
#include "Game.h"

/* === Defines === */
// Codes
#define SUCCESS_CODE 0
#define ERROR_CODE 1

// Screen
#define WINDOW_CLASS_NAME "WINDOW_CLASS"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 640
#define SCREEN_BPP 8

// Keyboard macroses
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

/* === Globals === */
// App
static int g_nExitCode = SUCCESS_CODE;

// Windows
static HINSTANCE g_hInstance = NULL;
static HWND g_hWindow = NULL;

// DirectX
static LPDIRECTDRAW7 g_lpDD = NULL;
static LPDIRECTDRAWPALETTE g_lpDDPalette = NULL;
static LPDIRECTDRAWSURFACE7 g_lpDDScreen = NULL;

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
                               (GetSystemMetrics(SM_CXSCREEN) - SCREEN_WIDTH)/2,
                               (GetSystemMetrics(SM_CYSCREEN) - SCREEN_HEIGHT)/2,
                               SCREEN_WIDTH,
                               SCREEN_HEIGHT,
                               NULL, // Parent
                               NULL, // Menu
                               g_hInstance,
                               NULL  // Useless
    );
    if (!g_hWindow)
        return false;

    // Initialize DirectDraw
    if ( FAILED(DirectDrawCreateEx(NULL, (void**)&g_lpDD, IID_IDirectDraw7, NULL)) )
        return false;
    g_lpDD->SetCooperativeLevel(g_hWindow, DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|
                                           DDSCL_ALLOWMODEX|DDSCL_ALLOWREBOOT);
    g_lpDD->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0);

    // Create pallete
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

    if ( FAILED(g_lpDD->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256|
                                      DDPCAPS_INITIALIZE,
                                      palette,
                                      &g_lpDDPalette,
                                      NULL)) )
        return false;

    // Primary surface
    DDSURFACEDESC2 DDSurfaceDesc;
    memset(&DDSurfaceDesc, 0, sizeof(DDSurfaceDesc));
    
    DDSurfaceDesc.dwSize = sizeof(DDSurfaceDesc);
    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if ( FAILED(g_lpDD->CreateSurface(&DDSurfaceDesc, &g_lpDDScreen, NULL)) )
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
    if (g_lpDD)
        g_lpDD->Release();
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
        if (KEYDOWN(VK_ESCAPE))
            break;

        if (!WinEvents())
            break;  // Break on quit event
        Game::Update();
        Game::Render();
    }

    Game::ShutDown();
    WinShutDown();

    return g_nExitCode;
}