#ifndef GAME_H_
#define GAME_H_

/* ====== INCLUDES ====== */
#include "Types.h"
#include "GraphicsModule.h"

/* ====== STRUCTURES ====== */
class Game : public EngineModule
{
    f32 m_dtTime;
    b32 m_bRunning;
public:
    b32 StartUp();
    void ShutDown();

    b32 Running() const { return m_bRunning; }

    void Update(f32 dtTime);
    void Render() const;
};

extern Game g_game;

#endif // GAME_H_
