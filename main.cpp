/* ====== Includes ====== */
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

/* ====== Defines ====== */
// App
#define SUCCESS_CODE 0
#define ERROR_CODE 1

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 8

// Windows
#define WINDOW_CLASS_NAME "WINDOW_CLASS"
#define CONSOLE_BUFSIZE 1024

#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

// DirectX
#define PALETTE_COLORS 256

#define _RGB16BIT565(R, G, B) ( ((R & 31) << 11) + ((G & 63) << 5) + (B & 31) )
#define _RGB16BIT555(R, G, B) ( ((R & 31) << 10) + ((G & 31) << 5) + (B & 31) )
#define _RGB24BIT(R, G, B) ( ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )
#define _RGB32BIT(A, R, G, B) ( ((A % 255) << 24) + ((R & 255) << 16) + ((G & 255) << 8) + (B & 255) )

#define DDRAW_INIT_STRUCT(STRUCT) { memset(&STRUCT, 0, sizeof(STRUCT)); STRUCT.dwSize = sizeof(STRUCT); }

// BMP
#define BMP_ID 0x4d42

/* ====== Globals ====== */
// App
static s32 g_nExitCode = SUCCESS_CODE;

// Windows
static HINSTANCE g_hInstance = NULL;
static HWND g_hWindow = NULL;
static char* g_consoleBuffer = NULL;

static b32 g_bWindowClosed = false;

// DirectX
static LPDIRECTDRAW7 g_pDD = NULL;
static LPDIRECTDRAWSURFACE7 g_pDDScreen = NULL;
static LPDIRECTDRAWSURFACE7 g_pDDScreenBack = NULL;
static LPDIRECTDRAWPALETTE g_pDDPalette = NULL;
static LPDIRECTDRAWCLIPPER g_pDDClipper = NULL;

/* ====== Structures ====== */
struct BMPFile
{
    BITMAPFILEHEADER file;
    BITMAPINFOHEADER info;
    PALETTEENTRY palette[PALETTE_COLORS];
    u8* buffer;
};

/* ====== Functions ====== */

#ifdef _DEBUG
/* === DEBUG PROTOTYPES */
static void ConsoleOut(const char* fmt, ...);
#endif

/* === BitMap === */
static void FlipBMP(u8* image, s32 bytesPerLine, s32 height)
{
    // Allocate memory
    s32 size = bytesPerLine * height;
    u8* buffer = (u8*)malloc(size);
    if (!buffer)
        return;

    // Copy image in buffer
    memcpy(buffer, image, size);

    // Flip
    for (s32 i = 0; i < height; ++i)
        memcpy(&image[(height-1 - i)*bytesPerLine], 
               &buffer[i*bytesPerLine],
               bytesPerLine);

    // Free memory
    free(buffer);
}

static b32 LoadBMP(const char* fileName, BMPFile* bmp)
{
    // Open file
    OFSTRUCT fileInfo;
    s32 fileHandle = OpenFile(fileName, &fileInfo, OF_READ);
    if (fileHandle == -1)
        return false;

    // Read file header
    _lread(fileHandle, &bmp->file, sizeof(bmp->file));
    if (bmp->file.bfType != BMP_ID)
    {
        _lclose(fileHandle);
        return false;
    }

    // Read info header
    _lread(fileHandle, &bmp->info, sizeof(bmp->info));

    // Read palette if we have
    if (bmp->info.biBitCount == 8)
    {
        _lread(fileHandle, bmp->palette, sizeof(bmp->palette[0]) * PALETTE_COLORS);
        
        // RGB -> BGR
        for (s32 i = 0; i < PALETTE_COLORS; ++i)
        {
            s32 temp = bmp->palette[i].peBlue;
            bmp->palette[i].peBlue = bmp->palette[i].peRed;
            bmp->palette[i].peRed = (u8)temp;

            // Flag
            bmp->palette[i].peFlags = PC_NOCOLLAPSE;
        }
    }

    // Check for errors
    s32 bitCount = bmp->info.biBitCount;
    if (bitCount != 8 && bitCount != 16 && bitCount != 24 && bitCount != 32)
    {
        _lclose(fileHandle);
        return false;
    }

    // Get right position for image reading
    _llseek(fileHandle, bmp->info.biSizeImage, SEEK_END);

    // Try to allocate memory
    bmp->buffer = (u8*)malloc(bmp->info.biSizeImage);
    if (!bmp->buffer)
    {
        _lclose(fileHandle);
        return false;
    }

    // Read image data
    _lread(fileHandle, bmp->buffer, bmp->info.biSizeImage);

    // Close file
    _lclose(fileHandle);

    // Flip image
    FlipBMP(bmp->buffer, bmp->info.biWidth * bmp->info.biBitCount/8, bmp->info.biHeight);

    return true;
}

static void UnloadBMP(BMPFile* bmp)
{
    if (bmp->buffer)
    {
        free(bmp->buffer);
        bmp->buffer = NULL;
    }
}

/* === DirectX === */
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

static void Blit(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h)
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

static void BlitClipped(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h)
{
    // Check if it's visible
    if (posX >= SCREEN_WIDTH  || posX + w <= 0 ||
        posY >= SCREEN_HEIGHT || posY + h <= 0)
        return;

    // Align rectangles
    RECT dst;
    s32 srcOffsetX, srcOffsetY;
    s32 dX, dY;

    // Left
    if (posX < 0)
    {
        dst.left = 0;
        srcOffsetX = dst.left - posX;
    }
    else
    {
        dst.left = posX;
        srcOffsetX = 0;
    }

    // Right
    if (posX + w > SCREEN_WIDTH)
        dst.right = SCREEN_WIDTH - 1;
    else
        dst.right = (posX + w) - 1;

    // Top
    if (posY < 0)
    {
        dst.top = 0;
        srcOffsetY = dst.top - posY;
    }
    else
    {
        dst.top = posY;
        srcOffsetY = 0;
    }

    // Bottom
    if (posY + h > SCREEN_HEIGHT)
        dst.bottom = SCREEN_HEIGHT - 1;
    else
        dst.bottom = (posY + h) - 1;

    // Difference
    dX = dst.right - dst.left + 1;
    dY = dst.bottom - dst.top + 1;

    // Start position
    videoBuffer += dst.top*pitch32 + dst.left;
    bitMap += srcOffsetY*w + srcOffsetX;

    // Blitting
    for (s32 y = 0; y < dY; ++y)
    {
        for (s32 x = 0; x < dX; ++x)
        {
            u32 pixel;
            if ((pixel = bitMap[x]) != _RGB32BIT(255, 0, 0, 0)) // Plot opaque pixels only
                videoBuffer[x] = pixel;
        }
        videoBuffer += pitch32;
        bitMap += w;
    }
}

LPDIRECTDRAWCLIPPER DDrawAttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, LPRECT clipList, s32 count)
{
    b32 bResult = true;
    LPDIRECTDRAWCLIPPER pDDClipper;
    LPRGNDATA pRegionData;

    // Create clipper
    if ( FAILED(g_pDD->CreateClipper(0, &pDDClipper, NULL)) )
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

/* === Windows === */

#ifdef _DEBUG
static void ConsoleOut(const char* fmt, ...)
{
    va_list vl;

    va_start(vl, fmt);
    _vsnprintf(g_consoleBuffer, CONSOLE_BUFSIZE, fmt, vl);
    va_end(vl);

    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), g_consoleBuffer, strlen(g_consoleBuffer), NULL, NULL);
}
#endif

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

#ifdef _DEBUG
    // NOTE HERE CONSOLE
    //if (!AllocConsole())
    //    return false;
    g_consoleBuffer = (char*)malloc(CONSOLE_BUFSIZE);
#endif

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

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // Peek message from any window
    {
        if (msg.message == WM_QUIT)
        {
            g_nExitCode = msg.wParam;
            return false;
        }

        TranslateMessage(&msg); // Keyboard input
        DispatchMessage(&msg);  // Send this to WinProc
    }

    return true;
}

static void WinShutDown()
{
    if (g_pDDClipper)
    {
        g_pDDClipper->Release();
        g_pDDClipper = NULL;
    }

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

#ifdef _DEBUG
    free(g_consoleBuffer);
    FreeConsole();
#endif
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!WinInit(hInstance))
        return ERROR_CODE;
    if (!Game::Init())
        return ERROR_CODE;

    // DEBUG
    BMPFile bmp;
    if (!LoadBMP("bitmap8.bmp", &bmp))
        return ERROR_CODE;

    PALETTEENTRY palette[PALETTE_COLORS];
    for (s32 i = 0; i < PALETTE_COLORS; ++i)
    {
        palette[i].peRed = rand() % 256;
        palette[i].peGreen = rand() % 256;
        palette[i].peBlue = rand() % 256;
        palette[i].peFlags = PC_NOCOLLAPSE;
    }
    if ( FAILED(g_pDD->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256|DDPCAPS_INITIALIZE, palette, &g_pDDPalette, NULL)) )
        return ERROR_CODE;
    g_pDDPalette->SetEntries(0, 0, PALETTE_COLORS, bmp.palette);

    // \DEBUG

    while (Game::Running())
    {
        // Debug
        if (KEYDOWN(VK_ESCAPE))
        {
            g_bWindowClosed = true;
            PostMessage(g_hWindow, WM_CLOSE, 0, 0);
        }

        if (!WinEvents())
            break;  // Break on quit event
        Game::Update();
        if (g_bWindowClosed)
            break; // DirectX may want to get window but it can be closed
        Game::Render();

        // DEBUG
        DDSURFACEDESC2 DDSurfaceDesc;
        DDRAW_INIT_STRUCT(DDSurfaceDesc);

        g_pDDScreen->Lock(NULL, &DDSurfaceDesc, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);
        memcpy((void*)DDSurfaceDesc.lpSurface, bmp.buffer, SCREEN_WIDTH*SCREEN_HEIGHT);
        g_pDDScreen->Unlock(NULL);
        // \DEBUG

        // Flip buffers
        // DEBUG
        //g_pDDScreen->Flip(NULL, DDFLIP_WAIT);
    }

    // DEBUG
    UnloadBMP(&bmp);

    Game::ShutDown();
    WinShutDown();

    return g_nExitCode;
}