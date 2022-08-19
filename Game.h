#ifndef GAME_H_
#define GAME_H_

#include "Types.h"
 
// Static class
class Game
{
    static b32 m_bRunning;
public:
    static b32 StartUp();
    static void ShutDown();

    static void Update();
    static void Render();

    static b32 Running() { return m_bRunning; }
private:
    Game() {}
    ~Game() {}
};

#endif // GAME_H_
