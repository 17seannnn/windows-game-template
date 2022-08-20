#ifndef LOG_H_
#define LOG_H_

#include "Types.h"

// Static class
class Log
{
    static s8* m_consoleBuffer;
public:
    enum eChannel
    {
        CHANNEL_LOG      = 1 << 0,
        CHANNEL_WINDOWS  = 1 << 1,
        CHANNEL_GRAPHICS = 1 << 2,
        CHANNEL_GAME     = 1 << 3
    };

    static b32 StartUp();
    static void ShutDown();

    static void Note(s32 stream, const char* fmt, ...);
};

#endif // LOG_H_