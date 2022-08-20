#include <stdio.h>

#include "BMP.h"

void FlipBMP(u8* image, s32 bytesPerLine, s32 height)
{
    // Allocate memory
    s32 size = bytesPerLine * height;
    u8* buffer = new u8[size];
    if (!buffer)
        return;

    // Copy image in buffer
    memcpy(buffer, image, size);

    // Flip
    for (s32 i = 0; i < height; ++i)
        memcpy(&image[(height-1 - i)*bytesPerLine], 
               &buffer[i*bytesPerLine],
               bytesPerLine);

    // Free memory
    delete[] buffer;
}

b32 LoadBMP(const char* fileName, BMPFile* bmp)
{
    // Open file
    OFSTRUCT fileInfo;
    s32 fileHandle = OpenFile(fileName, &fileInfo, OF_READ);
    if (fileHandle == -1)
        return false;

    // Read file header
    _lread(fileHandle, &bmp->file, sizeof(bmp->file));
    if (bmp->file.bfType != BMP_ID)
    {
        _lclose(fileHandle);
        return false;
    }

    // Read info header
    _lread(fileHandle, &bmp->info, sizeof(bmp->info));

    // Read palette if we have
    if (bmp->info.biBitCount == 8)
    {
        _lread(fileHandle, bmp->palette, sizeof(bmp->palette[0]) * PALETTE_COLORS);
        
        // RGB -> BGR
        for (s32 i = 0; i < PALETTE_COLORS; ++i)
        {
            s32 temp = bmp->palette[i].peBlue;
            bmp->palette[i].peBlue = bmp->palette[i].peRed;
            bmp->palette[i].peRed = (u8)temp;

            // Flag
            bmp->palette[i].peFlags = PC_NOCOLLAPSE;
        }
    }

    // Check for errors
    s32 bitCount = bmp->info.biBitCount;
    if (bitCount != 8 && bitCount != 16 && bitCount != 24 && bitCount != 32)
    {
        _lclose(fileHandle);
        return false;
    }

    // Get right position for image reading
    _llseek(fileHandle, -(s32)bmp->info.biSizeImage, SEEK_END);

    // Try to allocate memory
    bmp->buffer = new u8[bmp->info.biSizeImage];
    if (!bmp->buffer)
    {
        _lclose(fileHandle);
        return false;
    }

    // Read image data
    _lread(fileHandle, bmp->buffer, bmp->info.biSizeImage);

    // Close file
    _lclose(fileHandle);

    // Flip image
    FlipBMP(bmp->buffer, bmp->info.biWidth * bmp->info.biBitCount/8, bmp->info.biHeight);

    return true;
}

void UnloadBMP(BMPFile* bmp)
{
    if (bmp->buffer)
    {
        delete[] bmp->buffer;
        bmp->buffer = NULL;
    }
}