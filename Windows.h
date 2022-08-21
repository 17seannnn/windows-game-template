#ifndef WINDOWS_H_
#define WINDOWS_H_

#define WIN32_LEAN_AND_MEAN // No extra stuff
#include <windows.h>
#include <windowsx.h>

#include "Types.h"

// Macroses
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

// Static class
class Windows
{
    static s32 m_nExitCode;
    static b32 m_bWindowClosed; // NOTE think about this bool... maybe we don't need this.. or maybe need?

    static HINSTANCE m_hInstance;
    static HWND m_hWindow;
public:
    enum eExitCode
    {
        EC_SUCCESS = 0,
        EC_ERROR
    };

    static b32 StartUp(HINSTANCE hInstance, const char* title = "Game Template");
    static void ShutDown();

    static b32 HandleEvents();
    static void ShowMouse() { ShowCursor(TRUE); }
    static void HideMouse() { ShowCursor(FALSE); }

    static s32 GetExitCode() { return m_nExitCode; }
    static b32 IsWindowClosed() { return m_bWindowClosed; }

    static HINSTANCE GetInstance() { return m_hInstance; }
    static HWND GetWindow() { return m_hWindow; }
private:
    Windows() {}
    ~Windows() {}

    static LRESULT CALLBACK WinProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

};

#endif // WINDOWS_H_