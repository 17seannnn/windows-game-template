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
    Graphics::LockBack(screen, pitch);

    for (s32 i = 0; i < 1000; i++)
        screen[(rand() % Graphics::GetScreenHeight())*pitch + rand() % Graphics::GetScreenWidth()] = rand() % 256;

    Graphics::UnlockBack();

    Graphics::Flip();
}
