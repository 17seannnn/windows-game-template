#ifndef WINDOWS_H_
#define WINDOWS_H_

#define WIN32_LEAN_AND_MEAN // No extra stuff
#define INITGUID // For DirectX

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#include "Types.h"

// Macroses
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)
#define DDRAW_INIT_STRUCT(STRUCT) { memset(&STRUCT, 0, sizeof(STRUCT)); STRUCT.dwSize = sizeof(STRUCT); }

// Static class
class Windows
{
    static s32 m_nExitCode;
    static b32 m_bWindowClosed; // NOTE think about this bool... maybe we don't need this

    static HINSTANCE m_hInstance;
    static HWND m_hWindow;

    static LPDIRECTDRAW7 m_pDDraw;
    static LPDIRECTDRAWSURFACE7 m_pDDScreen;
    static LPDIRECTDRAWSURFACE7 m_pDDScreenBack;
    static LPDIRECTDRAWPALETTE m_pDDPalette;
    static LPDIRECTDRAWCLIPPER m_pDDClipper;
public:
    enum eExitCode
    {
        EC_SUCCESS = 0,
        EC_ERROR
    };

    static b32 StartUp(HINSTANCE hInstance, const char* title = "Game Template", s32 width = 640, s32 height = 480, s32 bpp = 8);
    static void ShutDown();

    static b32 HandleEvents();

    static s32 GetExitCode() { return m_nExitCode; }
    static b32 IsWindowClosed() { return m_bWindowClosed; }

    static HINSTANCE GetInstance() { return m_hInstance; }
    static HWND GetWindow() { return m_hWindow; }
    static LPDIRECTDRAW7 GetDDraw() { return m_pDDraw; }
private:
    Windows() {}
    ~Windows() {}

    static LRESULT CALLBACK WinProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

};

#endif // WINDOWS_H_