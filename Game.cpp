/* ====== INCLUDES ====== */
#include "Graphics.h"
#include "Input.h"
#include "Log.h"

#include "Game.h"

/* ====== VARIABLES ====== */
f32 Game::m_dtTime = 0;
b32 Game::m_bRunning = true;

Polygon2 Game::m_poly;

/* ====== METHODS====== */
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

void Game::Update(f32 dtTime)
{
    m_dtTime = dtTime;

    if (Input::KeyDown(DIK_ESCAPE))
        m_bRunning = false;
}

void Game::Render()
{
    u8* screen;
    s32 pitch;

    { // Render game objects
        Graphics::ClearScreen();
        if (!Graphics::LockBack(screen, pitch))
            return;

        Graphics::DrawQuad2(screen, pitch, 50, 300, 200, 250, 300, 400, 300, 400, 200);

        Graphics::UnlockBack();
    }

    { // Render debug stuff
        Graphics::DrawText_GDI(0, 0, 0, 255, 0, "FPS: %f", 1000.0f/m_dtTime);
    }

    // Flip screen
    Graphics::Flip();
}
