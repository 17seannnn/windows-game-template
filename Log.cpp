#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Log.h"

#define NOTE_PREFIX_BUFSIZE 64
#define NOTE_MESSAGE_BUFSIZE 512
#define NOTE_FINAL_BUFSIZE 1024

#define CHANNEL_PREFIX_UNDEFINED "Undefined"
#define CHANNEL_PREFIX_LOG       "Log"
#define CHANNEL_PREFIX_WINDOWS   "Windows"
#define CHANNEL_PREFIX_GRAPHICS  "Graphics"
#define CHANNEL_PREFIX_GAME      "Game"

#define PRIORITY_PREFIX_UNDEFINED "Undefined"
#define PRIORITY_PREFIX_ERROR     "Error"
#define PRIORITY_PREFIX_WARNING   "Warning"
#define PRIORITY_PREFIX_NOTE      "Note"

b32 Log::StartUp()
{
#ifdef _DEBUG
    // Allocate console
    if (!AllocConsole())
        return false;

    return true;
#endif
}

void Log::ShutDown()
{
#ifdef _DEBUG
    // Detach console
    FreeConsole();
#endif
}

void Log::Note(s32 channel, s32 priority, const char* fmt, ...)
{
#ifdef _DEBUG
    const char* channelPrefix = "";
    const char* priorityPrefix = "";

    switch (channel)
    {

    case CHANNEL_LOG:
    {
        channelPrefix = CHANNEL_PREFIX_LOG;
    } break;

    case CHANNEL_WINDOWS:
    {
        channelPrefix = CHANNEL_PREFIX_WINDOWS;
    } break;

    case CHANNEL_GRAPHICS:
    {
        channelPrefix = CHANNEL_PREFIX_GRAPHICS;
    } break;

    case CHANNEL_GAME:
    {
        channelPrefix = CHANNEL_PREFIX_GAME;
    } break;

    default:
    {
        channelPrefix = CHANNEL_PREFIX_UNDEFINED;
    } break;

    }

    switch (priority)
    {
    case PRIORITY_ERROR:
    {
        priorityPrefix = PRIORITY_PREFIX_ERROR;
    } break;

    case PRIORITY_WARNING:
    {
        priorityPrefix = PRIORITY_PREFIX_WARNING;
    } break;

    case PRIORITY_NOTE:
    {
        priorityPrefix = PRIORITY_PREFIX_NOTE;
    } break;

    default:
    {
        priorityPrefix = PRIORITY_PREFIX_UNDEFINED;
    } break;

    }

    // Get note prefix
    char notePrefix[NOTE_PREFIX_BUFSIZE];
    _snprintf(notePrefix, NOTE_PREFIX_BUFSIZE, "<%s> %s", channelPrefix, priorityPrefix);

    // Get note message
    va_list vl;
    va_start(vl, fmt);
    char noteMessage[NOTE_MESSAGE_BUFSIZE];
    _vsnprintf(noteMessage, NOTE_MESSAGE_BUFSIZE, fmt, vl);
    va_end(vl);

    // Get final note
    char noteFinal[NOTE_FINAL_BUFSIZE];
    _snprintf(noteFinal, NOTE_FINAL_BUFSIZE, "%s: %s\n", notePrefix, noteMessage);

    // Output
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), noteFinal, strlen(noteFinal), NULL, NULL);
#endif
}