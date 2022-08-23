#ifndef GAME_H_
#define GAME_H_

#include "Types.h"
#include "Graphics.h"
 
// Static class
class Game
{
    static f32 m_dtTime;
    static b32 m_bRunning;
    
    static Polygon2 m_poly;
public:
    static b32 StartUp();
    static void ShutDown();

    static void Update(f32 dtTime);
    static void Render();

    static b32 Running() { return m_bRunning; }
private:
    Game() {}
    ~Game() {}
};

#endif // GAME_H_
