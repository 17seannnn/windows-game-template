// Windows stuff >>>>
#define WIN32_LEAN_AND_MEAN // No MFC

#include <windows.h>
#include <windowsx.h>
// <<<< Windows stuff

// Defines >>>>

// Window
#define WINDOW_CLASS_NAME "WINDOW_CLASS"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 640

// Keyboard macroses
#define KEYDOWN(VK) (GetAsyncKeyState(VK) & 0x8000)
#define KEYUP(VK)   (GetAsyncKeyState(VK) & 0x8000 ? 0 : 1)

// <<<< Defines

// Globals >>>>
static int g_nExitCode = 0;
static HINSTANCE g_hInstance = NULL;
static HWND g_hWindow = NULL;
// <<<< Globals

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

static bool WinInit(HINSTANCE hInstance)
{
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
                               WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                               (GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH)/2,
                               (GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT)/2,
                               WINDOW_WIDTH,
                               WINDOW_HEIGHT,
                               NULL, // Parent
                               NULL, // Menu
                               g_hInstance,
                               NULL  // Useless
    );
    if (!g_hWindow)
        return false;

    // Success
    return true;
}

static bool WinEvents()
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    WinInit(hInstance);

    for (;;)
    {
        if (!WinEvents())
            break;  // Break on quit event

        if (KEYDOWN(VK_ESCAPE))
            SendMessage(g_hWindow, WM_CLOSE, 0, 0);
    }

    MessageBeep(MB_ICONEXCLAMATION);

    return g_nExitCode;
}