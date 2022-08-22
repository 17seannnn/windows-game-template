/* ====== TODO ======
 * - Maybe use something like NOTE(17) or TODO(sean)?
 *
 * - class EngineModule() with virtual functions like AddNote(), so you don't need to write channel every Log::Note()
 * - Set const methods that i forgot
 *
 * - put GetCaps() result in global variable
 * - BMP converters
 * - ShiftPalette()
 * - HandleLight()
 * - Windowed mode
 */

#include "Log.h"
#include "Windows.h"
#include "Graphics.h"
#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!Log::StartUp())
        return Windows::EC_ERROR;
    if (!Windows::StartUp(hInstance))
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

        if (!Windows::HandleEvents())
            break; // Break on quit event
        Game::Update();
        if (Windows::IsWindowClosed())
            break; // DirectX may want to get window but it can be closed
        Game::Render();
    }

    Game::ShutDown();
    Graphics::ShutDown();
    Math::ShutDown();
    Windows::ShutDown();
    Log::ShutDown();

    return Windows::GetExitCode();
}