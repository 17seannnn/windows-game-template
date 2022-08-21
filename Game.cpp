#include "Graphics.h"

#include "Game.h"

Polygon2 Game::poly;
b32 Game::m_bRunning = true;

b32 Game::StartUp()
{   
    poly.state = 1;
    poly.vertexCount = 3;
    poly.x = 250;
    poly.y = 250;
    poly.vx = 1;
    poly.vy = 1;
    poly.color = 50;
    poly.aVertex = new FVertex2[poly.vertexCount];

    poly.aVertex[0].x = 0.0f;
    poly.aVertex[1].x = 100.0f;
    poly.aVertex[2].x = 200.0f;
    poly.aVertex[0].y = 0.0f;
    poly.aVertex[1].y = -150.0f;
    poly.aVertex[2].y = 100.0f;

    return true;
}

void Game::ShutDown()
{
    delete[] poly.aVertex;
}

void Game::Update()
{}

void Game::Render()
{
    u8* screen;
    s32 pitch;

    Graphics::ClearScreen();
    if (!Graphics::LockBack(screen, pitch))
        return;

    Graphics::DrawPolygon2(&poly, screen, pitch);

    Graphics::UnlockBack();

    Graphics::RotatePolygon2(&poly, 90);
    Graphics::ScalePolygon2(&poly, rand() % 2 ? 2.0f : 0.5f, rand() % 2 ? 2.0f : 0.5f);

    Sleep(1000);

    Graphics::Flip();
}
