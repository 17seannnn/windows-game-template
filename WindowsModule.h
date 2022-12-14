#ifndef WINDOWSMODULE_H_
#define WINDOWSMODULE_H_

/* ====== INCLUDES ====== */
#define WIN32_LEAN_AND_MEAN // No extra stuff
#include <windows.h>
#include <windowsx.h>

#include "Types.h"
#include "EngineModule.h"

/* ====== DEFINES ====== */
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

/* ====== STRUCTURES ====== */
class WindowsModule : public EngineModule
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

extern WindowsModule g_windowsModule;

#endif // WINDOWSMODULE_H_