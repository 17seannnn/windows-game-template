#include "Graphics.h"
#include "Log.h"

#include "Game.h"

Polygon2 Game::m_poly;
b32 Game::m_bRunning = true;

b32 Game::StartUp()
{   
    m_poly.state = 1;
    m_poly.vertexCount = 3;
    m_poly.x = 250;
    m_poly.y = 250;
    m_poly.vx = 1;
    m_poly.vy = 1;
    m_poly.color = 50;
    m_poly.aVertex = new FVertex2[m_poly.vertexCount];

    m_poly.aVertex[0].x = 0.0f;
    m_poly.aVertex[1].x = 100.0f;
    m_poly.aVertex[2].x = 200.0f;
    m_poly.aVertex[0].y = 0.0f;
    m_poly.aVertex[1].y = -150.0f;
    m_poly.aVertex[2].y = 100.0f;

    return true;
}

void Game::ShutDown()
{
    delete[] m_poly.aVertex;
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

    Graphics::DrawPolygon2(&m_poly, screen, pitch);

    Graphics::UnlockBack();

    Math::ScalePolygon2(&m_poly, rand() % 2 ? 2.0f : 0.5f, rand() % 2 ? 2.0f : 0.5f);

    Matrix3x3 a, b, r;
    for (s32 i = 0; i < 3; i++)
    {
        for (s32 j = 0; j < 3; j++)
        {
            a.M[i][j] = j+1.0f;
            b.M[i][j] = j+1.0f;
        }
    }
    Math::MultiplyMatrix3x3(a, b, r);

    for (s32 row = 0; row < 3; row++)
        for (s32 col = 0; col < 3; col++)
        {
            Log::Note(Log::CHANNEL_MATH, Log::PRIORITY_NOTE, "A[%d][%d] %f", row, col, a.M[row][col]);
            Log::Note(Log::CHANNEL_MATH, Log::PRIORITY_NOTE, "B[%d][%d] %f", row, col, b.M[row][col]);
            Log::Note(Log::CHANNEL_MATH, Log::PRIORITY_NOTE, "R[%d][%d] %f", row, col, r.M[row][col]);
        }

    Sleep(1000);

    Graphics::Flip();
}
