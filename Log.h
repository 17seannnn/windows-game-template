#ifndef LOG_H_
#define LOG_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Types.h"

// Static class
class Log
{
    static HANDLE hConsole;
    static HFILE hFullLog;

    static HFILE hLog;
    static HFILE hWindows;
    static HFILE hClock;
    static HFILE hMath;
    static HFILE hGraphics;
    static HFILE hGame;

public:
    // Each channel represent engine's module
    enum eChannel
    {
        CHANNEL_LOG      = 1 << 0,
        CHANNEL_WINDOWS  = 1 << 1,
        CHANNEL_CLOCK    = 1 << 2,
        CHANNEL_MATH     = 1 << 3,
        CHANNEL_GRAPHICS = 1 << 4,
        CHANNEL_GAME     = 1 << 5
    };

    enum ePriority
    {
        PRIORITY_ERROR   = 1 << 0,
        PRIORITY_WARNING = 1 << 1,
        PRIORITY_NOTE    = 1 << 2
    };

    static b32 StartUp();
    static void ShutDown();

    static void Note(s32 channel, s32 priority, const char* fmt, ...);
};

#endif // LOG_H_