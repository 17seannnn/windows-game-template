#ifndef DEBUGLOGMANAGER_H_
#define DEBUGLOGMANAGER_H_

/* ====== INCLUDES ======= */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Types.h"

/* ====== STRUCTURES ======= */
class DebugLogManager
{
    HANDLE hConsole;
    HFILE hFullLog;

    HFILE hLog;
    HFILE hWindows;
    HFILE hGraphics;
    HFILE hInput;
    HFILE hSound;
    HFILE hGame;

public:
    // Each channel represent engine's module
    enum eChannel
    {
        CHANNEL_LOG      = 1 << 0,
        CHANNEL_WINDOWS  = 1 << 1,
        CHANNEL_CLOCK    = 1 << 2,
        CHANNEL_MATH     = 1 << 3,
        CHANNEL_GRAPHICS = 1 << 4,
        CHANNEL_INPUT    = 1 << 5,
        CHANNEL_SOUND    = 1 << 6,
        CHANNEL_GAME     = 1 << 7
    };

    enum ePriority
    {
        PRIORITY_ERROR   = 1 << 0,
        PRIORITY_WARNING = 1 << 1,
        PRIORITY_NOTE    = 1 << 2
    };

    b32 StartUp();
    void ShutDown();

    void AddNote(s32 channel, s32 priority, const char* name, const char* fmt, va_list vl);
};

extern DebugLogManager g_debugLogMgr;

#endif // DEBUGLOGMANAGER_H_