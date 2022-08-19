#ifndef LOG_H_
#define LOG_H_

#include "Types.h"

// Static class
class Log
{
    static s8* m_consoleBuffer;
public:
    enum eStreamDescriptor
    {
        // Default
        STREAM_CONSOLE      = 1 << 0,
        STREAM_DEFAULT_FILE = 1 << 1,
        STREAM_DEFAULT      = STREAM_CONSOLE | STREAM_DEFAULT_FILE,

        // Engine modules
        STREAM_LOG      = 1 << 2,
        STREAM_WINDOWS  = 1 << 3,
        STREAM_GRAPHICS = 1 << 4,
        STREAM_GAME     = 1 << 5
    };

    static b32 StartUp();
    static void ShutDown();

    static void Note(s32 stream, const char* fmt, ...);
};

#endif // LOG_H_