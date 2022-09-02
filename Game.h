#ifndef GAME_H_
#define GAME_H_

#include "Types.h"
#include "Graphics.h"
 
class Game
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
