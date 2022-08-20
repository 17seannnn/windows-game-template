#ifndef BMP_H_
#define BMP_H_

#define WIN32_LEAN_AND_MEAN // No extra stuff
#include <windows.h>

#include "Types.h"

#define BMP_ID 0x4d42

#ifndef PALETTE_COLORS
#define PALETTE_COLORS 256
#endif

struct BMPFile
{
    BITMAPFILEHEADER file;
    BITMAPINFOHEADER info;
    PALETTEENTRY palette[PALETTE_COLORS];
    u8* buffer;
};

void FlipBMP(u8* image, s32 bytesPerLine, s32 height);
b32 LoadBMP(const char* fileName, BMPFile* bmp);
void UnloadBMP(BMPFile* bmp);

#endif // BMP_H_