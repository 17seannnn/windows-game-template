#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Log.h"

#define CONSOLE_BUFSIZE 1024

s8* Log::m_consoleBuffer = NULL;

b32 Log::StartUp()
{
    // Allocate console
    if (!AllocConsole())
        return false;

    // Allocate memory for console's buffer
    m_consoleBuffer = new s8[CONSOLE_BUFSIZE];

    return true;
}

void Log::ShutDown()
{
    // Free buffer memory
    delete[] m_consoleBuffer;

    // Detach console
    FreeConsole();
}

void Log::Note(s32 stream, const char* fmt, ...)
{
}