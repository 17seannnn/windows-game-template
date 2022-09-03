/* ====== TODO ======\
 * - Rename everything like: class Windows -> WindowsModule
 * - All modules inherite EngineModule
 * - AddNote() instead of g_debugLogMgr.Note();
 * - DebugLogManager::PRIORITY_* -> PR_*
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
#include "ClockModule.h"
#include "Graphics.h"
#include "Input.h"
#include "Sound.h"
#include "Game.h"

#define FPS 30

namespace GT
{

static b32 StartUp(HINSTANCE hInstance)
{
    if (!g_logModule.StartUp())
        return false;

    if (!g_windowsModule.StartUp(hInstance))
        return false;

    if (!g_clockModule.StartUp(FPS))
        return false;

    if (!GTM::StartUp())
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

static void ShutDown()
{
    g_game.ShutDown();
    g_soundModule.ShutDown();
    g_inputModule.ShutDown();
    g_graphicsModule.ShutDown();
    GTM::ShutDown();
    g_clockModule.ShutDown();
    g_windowsModule.ShutDown();
    g_logModule.ShutDown();
}

} // namespace GT

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!GT::StartUp(hInstance))
        return Windows::EC_ERROR;

    while (g_game.Running())
    {
        if (!g_windowsModule.HandleEvents())
            break;
        if (!g_inputModule.HandleEvents())
            break;

        f32 dtTime = g_clockModule.GetDelta();

        g_game.Update(dtTime);
        if (g_windowsModule.IsWindowClosed())
            break;
        g_game.Render();

        g_clockModule.Sync();
    }

    GT::ShutDown();

    return g_windowsModule.GetExitCode();
}