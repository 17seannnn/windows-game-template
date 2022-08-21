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

    static s32 x = 0, y = 0;
    Polygon2 poly;
    poly.state = 1;
    poly.vertexCount = 3;
    poly.x = x;
    poly.y = y;
    poly.vx = 1;
    poly.vy = 1;
    poly.color = 50;
    poly.aVertex = new SVertex2[poly.vertexCount];

    poly.aVertex[0].x = 0;
    poly.aVertex[1].x = 100;
    poly.aVertex[2].x = 200;
    poly.aVertex[0].y = 0;
    poly.aVertex[1].y = -150;
    poly.aVertex[2].y = 100;

    Graphics::ClearScreen();
    if (!Graphics::LockBack(screen, pitch))
        return;

    Graphics::DrawPolygon2(screen, pitch, &poly);

    Graphics::UnlockBack();

    delete[] poly.aVertex;

    x += poly.vx;
    y += poly.vy;

    Sleep(33);

    Graphics::Flip();
}
