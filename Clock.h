#ifndef CLOCK_H_
#define CLOCK_H_

#include "Types.h"

// Static class
class Clock
{
    static u32 m_startTime;
    static u32 m_msSyncDelay;
public:
    static b32 StartUp(s32 fps);
    static void ShutDown();

    static f32 GetDelta();
    static void Sync();
};

#endif CLOCK_H_