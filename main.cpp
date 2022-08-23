/* ====== TODO ======
 * - class EngineModule() with virtual functions like AddNote(), so you don't need to write channel every Log::Note()
 * - Set const methods that i forgot
 *
 * - maybe do something like g_math.StartUp() instead of Math::StartUp()? Static class is interesting paradigm but... i think it's not that flexible
 * - put GetCaps() result in global variable
 * - BMP converters
 * - ShiftPalette()
 * - HandleLight()
 * - Windowed mode
 */

#include "Log.h"
#include "Windows.h"
#include "Clock.h"
#include "Graphics.h"
#include "Game.h"

#define FPS 30

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!Log::StartUp())
        return Windows::EC_ERROR;
    if (!Windows::StartUp(hInstance))
        return Windows::EC_ERROR;
    if (!Clock::StartUp(FPS))
        return Windows::EC_ERROR;
    if (!Math::StartUp())
        return Windows::EC_ERROR;
    if (!Graphics::StartUp())
        return Windows::EC_ERROR;
    if (!Game::StartUp())
        return Windows::EC_ERROR;

    while (Game::Running())
    {
        // DEBUG
        if (KEYDOWN(VK_ESCAPE))
            break;

        f32 dtTime = Clock::GetDelta();

        if (!Windows::HandleEvents())
            break; // Break on quit event
        Game::Update();
        if (Windows::IsWindowClosed())
            break; // DirectX may want to get window but it can be closed
        Game::Render();

        Clock::Sync();
    }

    Game::ShutDown();
    Graphics::ShutDown();
    Math::ShutDown();
    Clock::ShutDown();
    Windows::ShutDown();
    Log::ShutDown();

    return Windows::GetExitCode();
}