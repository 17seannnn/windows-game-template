#ifndef GAME_H_
#define GAME_H_

#include "Types.h"
#include "Graphics.h"
 
class Game
{
    f32 m_dtTime;
    b32 m_bRunning;
    
    Polygon2 m_poly;
public:
    b32 StartUp();
    void ShutDown();

    void Update(f32 dtTime);
    void Render();

    b32 Running() { return m_bRunning; }
};

extern Game g_game;

#endif // GAME_H_
