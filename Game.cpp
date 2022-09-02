/* ====== INCLUDES ====== */
#include "Graphics.h"
#include "Input.h"
#include "Sound.h"
#include "Log.h"

#include "Game.h"

/* ====== VARIABLES ====== */
Game g_game;

/* ====== METHODS====== */
b32 Game::StartUp()
{
    m_bRunning = true;

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

    if (g_inputModule.KeyDown(DIK_ESCAPE))
        m_bRunning = false;
}

void Game::Render() const
{
    u8* screen;
    s32 pitch;

    { // Render game objects
        g_graphicsModule.ClearScreen();
        if (!g_graphicsModule.LockBack(screen, pitch))
            return;

        g_graphicsModule.DrawQuad2(screen, pitch, 50, 300, 200, 250, 300, 400, 300, 400, 200);

        g_graphicsModule.UnlockBack();
    }

    { // Render debug stuff
        g_graphicsModule.DrawText_GDI(0, 0, 0, 255, 0, "FPS: %f", 1000.0f/m_dtTime);
        g_graphicsModule.DrawText_GDI(0, 100, 0, 255, 0, "X:%d Y:%d Z:%d", g_inputModule.GetMouseRelX(), g_inputModule.GetMouseRelY(), g_inputModule.GetMouseRelZ());
    }

    // Flip screen
    g_graphicsModule.Flip();
}
