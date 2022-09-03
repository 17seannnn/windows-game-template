/* ====== INCLUDES ====== */
#include <stdlib.h>

#include "WindowsModule.h"

/* ====== DEFINES ====== */
#define WINDOW_CLASS_NAME "WINDOWCLASS1"

/* ====== VARIABLES ====== */
WindowsModule g_windowsModule;

/* ====== METHODS ====== */
b32 WindowsModule::StartUp(HINSTANCE hInstance, const char* title)
{
    // Defaults
    m_nExitCode = EC_SUCCESS;
    m_bWindowClosed = false;

    // Save hInstance
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

    // No comments
    HideMouse();
    
    // Init rand seed
    srand(GetTickCount());

    // Success
    return true;
}

void WindowsModule::ShutDown()
{
    ShowMouse();
}

b32 WindowsModule::HandleEvents()
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

LRESULT CALLBACK WindowsModule::WinProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
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
        g_windowsModule.m_bWindowClosed = true;
        PostQuitMessage(0);
    } break;

    default:
        return DefWindowProc(hWindow, msg, wParam, lParam);

    }

    return 0;
}