/* TODO
 * - Spaces instead of tabs
 * - Complete it
*/

#include <windows.h>
#include <windowsx.h>

// Defines
#define WINDOW_CLASS_NAME "WINDOW_CLASS"

// Globals
static HINSTANCE g_hInstance = NULL;
static HWND g_hWindow = NULL;

static bool InitWin(HINSTANCE hInstance)
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
	WinClass.hbrBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WinClass.lpszMenuName = NULL;
	WinClass.lpszClassName = WINDOW_CLASS_NAME;
	WinClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&WinClass))
		return false;

	// Create window
	g_hWindow = CreateWindowEx(NULL,
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
                               NULL  // Useless stuff
    );
    if (!g_hWindow)
        return false;

	// Success
	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitWin(hInstance);

    Sleep(500);

	return 0;
}