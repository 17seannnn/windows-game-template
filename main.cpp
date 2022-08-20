/* OPTIONAL TODO LIST */

/* MAIN */
// - Use postincrement for types like s32 because preincrement can produce cpu stall

/* LOG */
// - Log in file and console with channels
// - Output every channel in console but with differenct colors
// - Filters for console
// - Verbosity for console
// - Flush everything

/* WINDOWS */
// - ShowMouse()
// - HideMouse()

/* GRAPHICS */
// - Save w, h, bpp in vars

// - DrawLine()
// - ClipLine()
// - DrawPolygon2D()
// - TranslatePolygon2D()
// - RotatePolygon2D()
// - Sin/Cos table
// - ScalePolygon2D()

// - GDIDisplayText()
// - DisplayText()
// - ScaleSurface()
// - ClearSurface()
// - DisplayRect()

/* OTHER STUFF */
// - put GetCaps() result in global variable
// - BMP converters
// - ShiftPalette()
// - HandleLight()
// - Windowed mode

#include "Log.h"
#include "Windows.h"
#include "Graphics.h"
#include "Game.h"

#if 0
/* === BMP === */
#define BMP_ID 0x4d42

struct BMPFile
{
    BITMAPFILEHEADER file;
    BITMAPINFOHEADER info;
    PALETTEENTRY palette[PALETTE_COLORS];
    u8* buffer;
};

static void FlipBMP(u8* image, s32 bytesPerLine, s32 height)
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

static b32 LoadBMP(const char* fileName, BMPFile* bmp)
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

static void UnloadBMP(BMPFile* bmp)
{
    if (bmp->buffer)
    {
        delete[] bmp->buffer;
        bmp->buffer = NULL;
    }
}

/* === Graphics === */
static inline void PlotPixel16(u16* videoBuffer, s32 pitch16, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    videoBuffer[y*pitch16 + x] = _RGB16BIT565(r, g, b);
}

static inline void PlotPixel24(u8* videoBuffer, s32 pitch, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    s32 addr = y*pitch + (x+x+x);
    videoBuffer[addr] = (u8)r;
    videoBuffer[addr+1] = (u8)g;
    videoBuffer[addr+2] = (u8)b;
}

static inline void PlotPixel32(u32* videoBuffer, s32 pitch32, s32 x, s32 y, s32 a, s32 r, s32 g, s32 b)
{
    videoBuffer[y*pitch32 + x] = _RGB32BIT(a, r, g, b);
}

static void Blit(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h)
{
    videoBuffer += posY*pitch32 + posX; // Start position for videoBuffer pointer

    for (s32 y = 0; y < h; ++y)
    {
        for (s32 x = 0; x < w; ++x)
        {
            u32 pixel;
            if ((pixel = bitMap[x])) // Plot opaque pixels only
                videoBuffer[x] = pixel;
        }
        videoBuffer += pitch32;
        bitMap += w;
    }
}

static void BlitClipped(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h)
{
    // Check if it's visible
    if (posX >= SCREEN_WIDTH  || posX + w <= 0 ||
        posY >= SCREEN_HEIGHT || posY + h <= 0)
        return;

    // Align rectangles
    RECT dst;
    s32 srcOffsetX, srcOffsetY;
    s32 dX, dY;

    // Left
    if (posX < 0)
    {
        dst.left = 0;
        srcOffsetX = dst.left - posX;
    }
    else
    {
        dst.left = posX;
        srcOffsetX = 0;
    }

    // Right
    if (posX + w > SCREEN_WIDTH)
        dst.right = SCREEN_WIDTH - 1;
    else
        dst.right = (posX + w) - 1;

    // Top
    if (posY < 0)
    {
        dst.top = 0;
        srcOffsetY = dst.top - posY;
    }
    else
    {
        dst.top = posY;
        srcOffsetY = 0;
    }

    // Bottom
    if (posY + h > SCREEN_HEIGHT)
        dst.bottom = SCREEN_HEIGHT - 1;
    else
        dst.bottom = (posY + h) - 1;

    // Difference
    dX = dst.right - dst.left + 1;
    dY = dst.bottom - dst.top + 1;

    // Start position
    videoBuffer += dst.top*pitch32 + dst.left;
    bitMap += srcOffsetY*w + srcOffsetX;

    // Blitting
    for (s32 y = 0; y < dY; ++y)
    {
        for (s32 x = 0; x < dX; ++x)
        {
            u32 pixel;
            if ((pixel = bitMap[x]) != _RGB32BIT(255, 0, 0, 0)) // Plot opaque pixels only
                videoBuffer[x] = pixel;
        }
        videoBuffer += pitch32;
        bitMap += w;
    }
}
#endif // #ifdef 0

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!Log::StartUp())
        return Windows::EC_ERROR;
    if (!Windows::StartUp(hInstance))
        return Windows::EC_ERROR;
    if (!Graphics::StartUp())
        return Windows::EC_ERROR;
    if (!Game::StartUp())
        return Windows::EC_ERROR;

    while (Game::Running())
    {
        // DEBUG
        if (KEYDOWN(VK_ESCAPE))
            break;

        if (!Windows::HandleEvents())
            break; // Break on quit event
        Game::Update();
        if (Windows::IsWindowClosed())
            break; // DirectX may want to get window but it can be closed
        Game::Render();
    }

    Game::ShutDown();
    Graphics::ShutDown();
    Windows::ShutDown();
    Log::ShutDown();

    return Windows::GetExitCode();
}