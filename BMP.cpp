#include <stdio.h>

#include "BMP.h"

#define BMP_ID 0x4d42

BMPFile::BMPFile(const char* fileName)
    : buffer(NULL)
{
    // Open file
    OFSTRUCT fileInfo;
    s32 fileHandle = OpenFile(fileName, &fileInfo, OF_READ);
    if (fileHandle == HFILE_ERROR)
        return;

    // Read file header
    _lread(fileHandle, &file, sizeof(file));
    if (file.bfType != BMP_ID)
    {
        _lclose(fileHandle);
        return;
    }

    // Read info header
    _lread(fileHandle, &info, sizeof(info));

    // Read palette if we have
    if (info.biBitCount == 8)
    {
        _lread(fileHandle, palette, sizeof(palette[0]) * PALETTE_COLORS);

        // RGB -> BGR
        for (s32 i = 0; i < PALETTE_COLORS; ++i)
        {
            s32 temp = palette[i].peBlue;
            palette[i].peBlue = palette[i].peRed;
            palette[i].peRed = (u8)temp;

            // Flag
            palette[i].peFlags = PC_NOCOLLAPSE;
        }
    }

    // Check for errors
    if (info.biBitCount != 8  && info.biBitCount != 16 &&
        info.biBitCount != 24 && info.biBitCount != 32)
    {
        _lclose(fileHandle);
        return;
    }

    // Get right position for image reading
    _llseek(fileHandle, -(s32)info.biSizeImage, SEEK_END);

    // Try to allocate memory
    buffer = new u8[info.biSizeImage];
    if (!buffer)
    {
        _lclose(fileHandle);
        return;
    }

    // Read image data
    _lread(fileHandle, buffer, info.biSizeImage);

    // Close file
    _lclose(fileHandle);

    // Flip image
    Flip(info.biWidth * info.biBitCount/8, info.biHeight);
}

BMPFile::~BMPFile()
{
    if (buffer)
    {
        delete[] buffer;
        buffer = NULL;
    }
}

void BMPFile::Flip(s32 bytesPerLine, s32 height)
{
    // Allocate memory
    s32 size = bytesPerLine * height;
    u8* flip = new u8[size];
    if (!flip)
        return;

    // Copy image in buffer
    memcpy(flip, buffer, size);

    // Flip
    for (s32 i = 0; i < height; ++i)
        memcpy(&buffer[(height-1 - i)*bytesPerLine],
               &flip[i*bytesPerLine],
               bytesPerLine);

    // Free memory
    delete[] flip;
}
