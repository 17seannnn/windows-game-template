#include "Graphics.h"

#include "Game.h"

b32 Game::m_bRunning = true;

b32 Game::StartUp()
{
    return true;
}

void Game::ShutDown()
{}

void Game::Update()
{}

void Game::Render()
{
    u8* screen;
    s32 pitch;

    if (!Graphics::LockBack(screen, pitch))
        return;

    Graphics::DrawLine8(screen, pitch, rand() % 256, 0, 0, 256, 1);

    Graphics::UnlockBack();

    Graphics::Flip();
}
