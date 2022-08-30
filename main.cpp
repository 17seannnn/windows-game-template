/* ====== TODO ======
 * - class EngineModule() with virtual functions like AddNote(), so you don't need to write channel every Log::Note()
 * - Set const methods that i forgot
 *
 * - Windowed mode
 * - Maybe put all .lib and .dll files in project directory?
 *
 * - DirectDraw Getcaps info
 * - BMP converters
 * - Structs aligning?
 * - Make engine multithreaded?
 */

#include "Log.h"
#include "Windows.h"
#include "Clock.h"
#include "Graphics.h"
#include "Input.h"
#include "Sound.h"
#include "Game.h"

#define FPS 30

static b32 GT_StartUp(HINSTANCE hInstance)
{
    if (!g_logModule.StartUp())
        return false;

    if (!g_windowsModule.StartUp(hInstance))
        return false;

    if (!g_clockModule.StartUp(FPS))
        return false;

    if (!g_mathModule.StartUp())
        return false;

    if (!g_graphicsModule.StartUp(g_windowsModule.GetWindow()))
        return false;

    if (!g_inputModule.StartUp(g_windowsModule.GetInstance(), g_windowsModule.GetWindow()))
        return false;

    if (!g_soundModule.StartUp(g_windowsModule.GetWindow()))
        return false;

    if (!g_game.StartUp())
        return false;

    return true;
}

static void GT_ShutDown()
{
    g_game.ShutDown();
    g_soundModule.ShutDown();
    g_inputModule.ShutDown();
    g_graphicsModule.ShutDown();
    g_mathModule.ShutDown();
    g_clockModule.ShutDown();
    g_windowsModule.ShutDown();
    g_logModule.ShutDown();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!GT_StartUp(hInstance))
        return Windows::EC_ERROR;

    while (g_game.Running())
    {
        f32 dtTime = g_clockModule.GetDelta();

        if (!g_windowsModule.HandleEvents())
            break; // Break on quit event
        if (!g_inputModule.HandleEvents())
            break;

        g_game.Update(dtTime);
        if (g_windowsModule.IsWindowClosed())
            break; // DirectX may want to get window but it can be closed
        g_game.Render();

        g_clockModule.Sync();
    }

    GT_ShutDown();

    return g_windowsModule.GetExitCode();
}