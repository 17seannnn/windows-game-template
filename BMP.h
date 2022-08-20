#ifndef BMP_H_
#define BMP_H_

#define WIN32_LEAN_AND_MEAN // No extra stuff
#include <windows.h>

#include "Types.h"

#ifndef PALETTE_COLORS
#define PALETTE_COLORS 256
#endif

struct BMPFile
{
    BITMAPFILEHEADER file;
    BITMAPINFOHEADER info;
    PALETTEENTRY palette[PALETTE_COLORS];
    u8* buffer;

    BMPFile(const char* fileName);
    ~BMPFile();
private:
    void Flip(s32 bytesPerLine, s32 height);
};

#endif // BMP_H_