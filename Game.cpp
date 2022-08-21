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

    for (s32 i = 0; i < 1000; i++)
        Graphics::DrawLine8(screen, pitch, rand() % 256, rand() % Graphics::GetScreenWidth(), rand() % Graphics::GetScreenHeight(),
                                                         rand() % Graphics::GetScreenWidth(), rand() % Graphics::GetScreenHeight());

    Graphics::UnlockBack();

    Graphics::Flip();
}
