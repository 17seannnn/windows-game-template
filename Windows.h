#ifndef WINDOWS_H_
#define WINDOWS_H_

#define WIN32_LEAN_AND_MEAN // No extra stuff
#include <windows.h>
#include <windowsx.h>

#include "Types.h"

// Macroses
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

class Windows
{
    s32 m_nExitCode;
    b32 m_bWindowClosed;

    HINSTANCE m_hInstance;
    HWND m_hWindow;
public:
    enum eExitCode
    {
        EC_SUCCESS = 0,
        EC_ERROR
    };

    b32 StartUp(HINSTANCE hInstance, const char* title = "Game Template");
    void ShutDown();

    b32 HandleEvents();
    void ShowMouse() const { ShowCursor(TRUE); }
    void HideMouse() const { ShowCursor(FALSE); }

    s32 GetExitCode() const { return m_nExitCode; }
    b32 IsWindowClosed() const { return m_bWindowClosed; }

    HINSTANCE GetInstance() { return m_hInstance; }
    HWND GetWindow() { return m_hWindow; }
private:
    static LRESULT CALLBACK WinProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

};

extern Windows g_windowsModule;

#endif // WINDOWS_H_