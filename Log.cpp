/* ====== TODO ======
 * - What if modules ask open log file for themselves? They can handle their names and colors
 * - Use Win32 funcs for files handling
 * - Filters: channels, priorities
 * - Verbosity
 * - Enable, disable console
 */

/* ====== INCLUDES ====== */
#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Log.h"

/* ====== DEFINES ====== */
#define NOTE_PREFIX_BUFSIZE  64
#define NOTE_MESSAGE_BUFSIZE 512
#define NOTE_FINAL_BUFSIZE   1024

#define CHANNEL_PREFIX_UNDEFINED "Undefined"
#define CHANNEL_PREFIX_LOG       "Log"
#define CHANNEL_PREFIX_WINDOWS   "Windows"
#define CHANNEL_PREFIX_MATH      "Math"
#define CHANNEL_PREFIX_GRAPHICS  "Graphics"
#define CHANNEL_PREFIX_GAME      "Game"

#define PRIORITY_PREFIX_UNDEFINED "Undefined"
#define PRIORITY_PREFIX_ERROR     "Error"
#define PRIORITY_PREFIX_WARNING   "Warning"
#define PRIORITY_PREFIX_NOTE      "Note"

#define DIR_LOGS "logs\\"

enum eFgColor
{
    FG_BLACK = 0,
    FG_BLUE = 1,
    FG_GREEN = 2,
    FG_CYAN = 3,
    FG_RED = 4,
    FG_MAGENTA = 5,
    FG_BROWN = 6,
    FG_LIGHTGRAY = 7,
    FG_GRAY = 8,
    FG_LIGHTBLUE = 9,
    FG_LIGHTGREEN = 10,
    FG_LIGHTCYAN = 11,
    FG_LIGHTRED = 12,
    FG_LIGHTMAGENTA = 13,
    FG_YELLOW = 14,
    FG_WHITE = 15
};

enum eBgColor
{
    BG_NAVYBLUE = 16,
    BG_GREEN = 32,
    BG_TEAL = 48,
    BG_MAROON = 64,
    BG_PURPLE = 80,
    BG_OLIVE = 96,
    BG_SILVER = 112,
    BG_GRAY = 128,
    BG_BLUE = 144,
    BG_LIME = 160,
    BG_CYAN = 176,
    BG_RED = 192,
    BG_MAGENTA = 208,
    BG_YELLOW = 224,
    BG_WHITE = 240
};

enum eChannelColor
{
    CHANNEL_COLOR_UNDEFINED = FG_LIGHTMAGENTA,
    CHANNEL_COLOR_LOG       = FG_WHITE,
    CHANNEL_COLOR_WINDOWS   = FG_LIGHTBLUE,
    CHANNEL_COLOR_MATH      = FG_LIGHTRED,
    CHANNEL_COLOR_GRAPHICS  = FG_LIGHTGREEN,
    CHANNEL_COLOR_GAME      = FG_BROWN,
};

enum ePriorityColor
{
    PRIORITY_COLOR_UNDEFINED = BG_TEAL,
    PRIORITY_COLOR_ERROR     = BG_RED,
    PRIORITY_COLOR_WARNING   = BG_NAVYBLUE,
    PRIORITY_COLOR_NOTE      = 0,
};

/* ====== VARIABLES ====== */
HANDLE Log::hConsole;
HFILE Log::hFullLog;

HFILE Log::hLog;
HFILE Log::hWindows;
HFILE Log::hMath;
HFILE Log::hGraphics;
HFILE Log::hGame;

/* ====== METHODS ====== */
b32 Log::StartUp()
{
#ifdef _DEBUG
    // Allocate console
    if (!AllocConsole())
        return false;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Open all log files
    CreateDirectory(DIR_LOGS, NULL);

    OFSTRUCT fileInfo;
    if (-1 == (hFullLog = OpenFile(DIR_LOGS"FullLog.txt", &fileInfo, OF_CREATE)) )
        return false;
    if (-1 == (hLog = OpenFile(DIR_LOGS"ModuleLog.txt", &fileInfo, OF_CREATE)) )
        return false;
    if (-1 == (hWindows = OpenFile(DIR_LOGS"ModuleWindows.txt", &fileInfo, OF_CREATE)) )
        return false;
    if (-1 == (hMath = OpenFile(DIR_LOGS"ModuleMath.txt", &fileInfo, OF_CREATE)) )
        return false;
    if (-1 == (hGraphics = OpenFile(DIR_LOGS"ModuleGraphics.txt", &fileInfo, OF_CREATE)) )
        return false;
    if (-1 == (hGame = OpenFile(DIR_LOGS"ModuleGame.txt", &fileInfo, OF_CREATE)) )
        return false;

    return true;
#endif
}

void Log::ShutDown()
{
#ifdef _DEBUG
    // Close log files
    _lclose(hFullLog);
    _lclose(hLog);
    _lclose(hWindows);
    _lclose(hMath);
    _lclose(hGraphics);
    _lclose(hGame);

    // Detach console
    FreeConsole();
#endif
}

void Log::Note(s32 channel, s32 priority, const char* fmt, ...)
{
#ifdef _DEBUG
    HFILE hFile;
    const char* channelPrefix = "";
    const char* priorityPrefix = "";
    WORD noteColor = 0;

    // Foreground color and channel prefix
    switch (channel)
    {

    case CHANNEL_LOG:
    {
        hFile = hLog;
        channelPrefix = CHANNEL_PREFIX_LOG;
        noteColor |= CHANNEL_COLOR_LOG;
    } break;

    case CHANNEL_WINDOWS:
    {
        hFile = hWindows;
        channelPrefix = CHANNEL_PREFIX_WINDOWS;
        noteColor |= CHANNEL_COLOR_WINDOWS;
    } break;

    case CHANNEL_MATH:
    {
        hFile = hMath;
        channelPrefix = CHANNEL_PREFIX_MATH;
        noteColor |= CHANNEL_COLOR_MATH;
    } break;

    case CHANNEL_GRAPHICS:
    {
        hFile = hGraphics;
        channelPrefix = CHANNEL_PREFIX_GRAPHICS;
        noteColor |= CHANNEL_COLOR_GRAPHICS;
    } break;

    case CHANNEL_GAME:
    {
        hFile = hGame;
        channelPrefix = CHANNEL_PREFIX_GAME;
        noteColor |= CHANNEL_COLOR_GAME;
    } break;

    default:
    {
        hFile = -1;
        channelPrefix = CHANNEL_PREFIX_UNDEFINED;
        noteColor |= CHANNEL_COLOR_UNDEFINED;
    } break;

    }

    // Background color and priority prefix
    switch (priority)
    {
    case PRIORITY_ERROR:
    {
        priorityPrefix = PRIORITY_PREFIX_ERROR;
        noteColor |= PRIORITY_COLOR_ERROR;
    } break;

    case PRIORITY_WARNING:
    {
        priorityPrefix = PRIORITY_PREFIX_WARNING;
        noteColor |= PRIORITY_COLOR_WARNING;
    } break;

    case PRIORITY_NOTE:
    {
        priorityPrefix = PRIORITY_PREFIX_NOTE;
        noteColor |= PRIORITY_COLOR_NOTE;
    } break;

    default:
    {
        priorityPrefix = PRIORITY_PREFIX_UNDEFINED;
        noteColor |= PRIORITY_COLOR_UNDEFINED;
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
    _snprintf(noteFinal, NOTE_FINAL_BUFSIZE, "%s: %s\r\n", notePrefix, noteMessage);
    s32 noteLength = strlen(noteFinal);

    // Output
    SetConsoleTextAttribute(hConsole, noteColor);
    WriteConsole(hConsole, noteFinal, noteLength, NULL, NULL);

    _lwrite(hFullLog, noteFinal, noteLength);
    if (hFile != -1)
        _lwrite(hFile, noteFinal, noteLength);

    FlushFileBuffers((HANDLE)hFullLog);
    FlushFileBuffers((HANDLE)hFile);
#endif
}