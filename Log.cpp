#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Log.h"

#define CONSOLE_BUFSIZE 1024

s8* Log::m_consoleBuffer = NULL;

b32 Log::StartUp()
{
#ifdef _DEBUG
    // Allocate console
    if (!AllocConsole())
        return false;

    // Allocate memory for console's buffer
    m_consoleBuffer = new s8[CONSOLE_BUFSIZE];

    return true;
#endif
}

void Log::ShutDown()
{
#ifdef _DEBUG
    // Free buffer memory
    delete[] m_consoleBuffer;

    // Detach console
    FreeConsole();
#endif
}

void Log::Note(s32 channel, const char* fmt, ...)
{
#ifdef _DEBUG
    // NOTE start here
#endif
}