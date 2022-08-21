/* ====== TODO ======
 * - Use my surface or texture structure

 * - DrawLine()
 * - ClipLine()
 * - DrawPolygon2D()
 * - TranslatePolygon2D()
 * - RotatePolygon2D()
 * - Sin/Cos table
 * - ScalePolygon2D()

 * - GDIDisplayText()
 * - DisplayText()
 * - ScaleSurface()
 * - ClearSurface()
 * - DisplayRect()
 */

/* ====== INCLUDES ====== */
#include <stdio.h>

#include "Windows.h"
#include "BMP.h"
#include "Log.h"

#define INITGUID // For DirectX in "Graphics.h"
#include "Graphics.h"
#undef INITGUID

/* ====== DEFINES ====== */
#define PALETTE_COLORS 256
#define COLOR_KEY 0

#define DDRAW_INIT_STRUCT(STRUCT) { memset(&STRUCT, 0, sizeof(STRUCT)); STRUCT.dwSize = sizeof(STRUCT); }

/* ====== VARIABLES ====== */
s32 Graphics::m_screenWidth = 0;
s32 Graphics::m_screenHeight = 0;
s32 Graphics::m_screenBPP = 0;

LPDIRECTDRAW7 Graphics::m_pDDraw = NULL;
LPDIRECTDRAWSURFACE7 Graphics::m_pDDScreen = NULL;
LPDIRECTDRAWSURFACE7 Graphics::m_pDDScreenBack = NULL;
LPDIRECTDRAWPALETTE Graphics::m_pDDPalette = NULL;
LPDIRECTDRAWCLIPPER Graphics::m_pDDClipper = NULL;

/* ====== METHODS ====== */
b32 Graphics::StartUp(s32 width, s32 height, s32 bpp)
{
    // Set screen variables
    m_screenWidth = width;
    m_screenHeight = height;
    m_screenBPP = bpp;

    // Initialize DirectDraw
    if ( FAILED(DirectDrawCreateEx(NULL, (void**)&m_pDDraw, IID_IDirectDraw7, NULL)) )
        return false;
    // Cooperative level with window
    if ( FAILED(m_pDDraw->SetCooperativeLevel(Windows::GetWindow(),
                                              DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|
                                              DDSCL_ALLOWMODEX|DDSCL_ALLOWREBOOT)) )
        return false;
    // Display mode
    if ( FAILED(m_pDDraw->SetDisplayMode(width, height, bpp, 0, 0)) )
        return false;

    // Primary surface
    DDSURFACEDESC2 DDSurfaceDesc;
    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    DDSurfaceDesc.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
    DDSurfaceDesc.dwBackBufferCount = 1;

    if ( FAILED(m_pDDraw->CreateSurface(&DDSurfaceDesc, &m_pDDScreen, NULL)) )
        return false;

    // Back surface
    DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    if ( FAILED(m_pDDScreen->GetAttachedSurface(&DDSurfaceDesc.ddsCaps, &m_pDDScreenBack)) )
        return false;

    // Palette
    if (bpp == 8)
    {
        PALETTEENTRY palette[PALETTE_COLORS];

        for (s32 i = 1; i < PALETTE_COLORS-1; i++)
        {
            palette[i].peRed = rand() % 256;
            palette[i].peGreen = rand() % 256;
            palette[i].peBlue = rand() % 256;
            palette[i].peFlags = PC_NOCOLLAPSE;
        }

        palette[0].peRed = 0;
        palette[0].peGreen = 0;
        palette[0].peBlue = 0;
        palette[0].peFlags = PC_NOCOLLAPSE;

        palette[255].peRed = 255;
        palette[255].peGreen = 255;
        palette[255].peBlue = 255;
        palette[255].peFlags = PC_NOCOLLAPSE;

        if ( FAILED(m_pDDraw->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256|DDPCAPS_INITIALIZE, palette, &m_pDDPalette, NULL)) )
            return false;
        if ( FAILED(m_pDDScreen->SetPalette(m_pDDPalette)) )
            return false;
    }

    // Clipper
    RECT clipList[1] = { { 0, 0, width, height } };

    m_pDDClipper = AttachClipper(m_pDDScreenBack, clipList, 1);
    if (!m_pDDClipper)
        return false;

    // Make note
    Log::Note(Log::CHANNEL_GRAPHICS, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Graphics::ShutDown()
{
    if (m_pDDClipper)
    {
        m_pDDClipper->Release();
        m_pDDClipper = NULL;
    }

    if (m_pDDPalette)
    {
        m_pDDPalette->Release();
        m_pDDPalette = NULL;
    }

    if (m_pDDScreenBack)
    {
        m_pDDScreenBack->Release();
        m_pDDScreenBack = NULL;
    }

    if (m_pDDScreen)
    {
        m_pDDScreen->SetPalette(NULL);
        m_pDDScreen->SetClipper(NULL);
        m_pDDScreen->Release();
        m_pDDScreen = NULL;
    }

    if (m_pDDraw)
    {
        m_pDDraw->Release();
        m_pDDraw= NULL;
    }

    Log::Note(Log::CHANNEL_GRAPHICS, Log::PRIORITY_NOTE, "Module shut down");
}

b32 Graphics::LockScreen(u8*& buffer, s32& pitch)
{
    DDSURFACEDESC2 DDSurfaceDesc;
    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    if ( FAILED(m_pDDScreen->Lock(NULL, &DDSurfaceDesc, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL)) )
        return false;

    buffer = (u8*)DDSurfaceDesc.lpSurface;
    pitch = DDSurfaceDesc.lPitch;

    return true;
}

b32 Graphics::LockBack(u8*& buffer, s32& pitch)
{
    DDSURFACEDESC2 DDSurfaceDesc;
    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    if ( FAILED(m_pDDScreenBack->Lock(NULL, &DDSurfaceDesc, DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR, NULL)) )
        return false;

    buffer = (u8*)DDSurfaceDesc.lpSurface;
    pitch = DDSurfaceDesc.lPitch;

    return true;
}

void Graphics::PlotPixel24(u8* videoBuffer, s32 pitch, s32 x, s32 y, s32 r, s32 g, s32 b)
{
    s32 addr = y*pitch + (x+x+x);
    videoBuffer[addr] = (u8)r;
    videoBuffer[addr+1] = (u8)g;
    videoBuffer[addr+2] = (u8)b;
}

void Graphics::DrawLine8(u8* videoBuffer, s32 pitch, s32 color, s32 fromX, s32 fromY, s32 toX, s32 toY)
{
    s32 dx = toX - fromX, dy = toY - fromY;
    s32 incX, incY;
    s32 error;

    // Init start position
    videoBuffer += fromY*pitch + fromX;

    // Set direction
    if (dx >= 0)
    {
        incX = 1;
    }
    else
    {
        incX = -1;
        dx = -dx;
    }

    if (dy >= 0)
    {
        incY = pitch;
    }
    else
    {
        incY = -pitch;
        dy = -dy;
    }

    s32 dx2 = dx << 1, dy2 = dy << 1;

    // Draw the line
    if (dx > dy)
    {
        // Set error according to start position
        error = dy2 - dx;

        for (s32 i = 0; i < dx; i++)
        {
            // Set color
            *videoBuffer = (u8)color;

            // Handle error
            if (error >= 0)
            {
                error -= dx2;
                videoBuffer += incY;
            }

            // Correct error
            error += dy2;

            // Continue
            videoBuffer += incX;
        }
    }
    else
    {
        // Set error according to start position
        error = dx2 - dy;

        for (s32 i = 0; i < dy; i++)
        {
            // Set color
            *videoBuffer = (u8)color;

            // Handle error
            if (error >= 0)
            {
                error -= dy2;
                videoBuffer += incX;
            }

            // Correct error
            error += dx2;

            // Continue
            videoBuffer += incY;
        }
    }
}

void Graphics::DDrawError(HRESULT error)
{
    char dderr[256];

    switch (error)
    {
    case DDERR_DDSCAPSCOMPLEXREQUIRED : sprintf(dderr, "DDERR_DDSCAPSCOMPLEXREQUIRED: New for DirectX 7.0. The surface requires the DDSCAPS_COMPLEX flag."); break;
    case DDERR_DEVICEDOESNTOWNSURFACE : sprintf(dderr, "DDERR_DEVICEDOESNTOWNSURFACE: Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device."); break;
    case DDERR_EXPIRED : sprintf(dderr, "DDERR_EXPIRED: The data has expired and is therefore no longer valid."); break;
    case DDERR_INVALIDSTREAM : sprintf(dderr, "DDERR_INVALIDSTREAM: The specified stream contains invalid data."); break;
    case DDERR_MOREDATA : sprintf(dderr, "DDERR_MOREDATA: There is more data available than the specified buffer size can hold."); break;
    case DDERR_NEWMODE : sprintf(dderr, "DDERR_NEWMODE: New for DirectX 7.0. When IDirectDraw7::StartModeTest is called with the DDSMT_ISTESTREQUIRED flag, it may return this value to denote that some or all of the resolutions can and should be tested. IDirectDraw7::EvaluateMode returns this value to indicate that the test has switched to a new display mode."); break;
    case DDERR_NODRIVERSUPPORT : sprintf(dderr, "DDERR_NODRIVERSUPPORT: New for DirectX 7.0. Testing cannot proceed because the display adapter driver does not enumerate refresh rates."); break;
    case DDERR_NOFOCUSWINDOW : sprintf(dderr, "DDERR_NOFOCUSWINDOW: An attempt was made to create or set a device window without first setting the focus window."); break;
    case DDERR_NOMONITORINFORMATION : sprintf(dderr, "DDERR_NOMONITORINFORMATION: New for DirectX 7.0. Testing cannot proceed because the monitor has no associated EDID data."); break;
    case DDERR_NONONLOCALVIDMEM : sprintf(dderr, "DDERR_NONONLOCALVIDMEM: An attempt was made to allocate nonlocal video memory from a device that does not support nonlocal video memory."); break;
    case DDERR_NOOPTIMIZEHW : sprintf(dderr, "DDERR_NOOPTIMIZEHW: The device does not support optimized surfaces."); break;
    case DDERR_NOSTEREOHARDWARE : sprintf(dderr, "DDERR_NOSTEREOHARDWARE: There is no stereo hardware present or available."); break;
    case DDERR_NOSURFACELEFT : sprintf(dderr, "DDERR_NOSURFACELEFT: There is no hardware present that supports stereo surfaces."); break;
    case DDERR_NOTLOADED : sprintf(dderr, "DDERR_NOTLOADED: The surface is an optimized surface, but it has not yet been allocated any memory."); break;
    case DDERR_OVERLAPPINGRECTS : sprintf(dderr, "DDERR_OVERLAPPINGRECTS: The source and destination rectangles are on the same surface and overlap each other."); break;
    case DDERR_TESTFINISHED : sprintf(dderr, "DDERR_TESTFINISHED: New for DirectX 7.0. When returned by the IDirectDraw7::StartModeTest method, this value means that no test could be initiated because all the resolutions chosen for testing already have refresh rate information in the registry. When returned by IDirectDraw7::EvaluateMode, the value means that DirectDraw has completed a refresh rate test."); break;
    case DDERR_VIDEONOTACTIVE : sprintf(dderr, "DDERR_VIDEONOTACTIVE: The video port is not active."); break;
    case DDERR_ALREADYINITIALIZED : sprintf(dderr, "DDERR_ALREADYINITIALIZED: The object has already been initialized."); break;
    case DDERR_CANNOTATTACHSURFACE : sprintf(dderr, "DDERR_CANNOTATTACHSURFACE: A surface cannot be attached to another requested surface."); break;
    case DDERR_CANNOTDETACHSURFACE : sprintf(dderr, "DDERR_CANNOTDETACHSURFACE: A surface cannot be detached from another requested surface."); break;
    case DDERR_CURRENTLYNOTAVAIL : sprintf(dderr, "DDERR_CURRENTLYNOTAVAIL: No support is currently available"); break;
    case DDERR_EXCEPTION : sprintf(dderr, "DDERR_EXCEPTION: An exception was encountered while performing the requested operation."); break;
    case DDERR_GENERIC : sprintf(dderr, "DDERR_GENERIC: There is an undefined error condition."); break;
    case DDERR_HEIGHTALIGN : sprintf(dderr, "DDERR_HEIGHTALIGN: The height of the provided rectangle is not a multiple of the required alignment."); break;
    case DDERR_INCOMPATIBLEPRIMARY : sprintf(dderr, "DDERR_INCOMPATIBLEPRIMARY: The primary surface creation request does not match the existing primary surface."); break;
    case DDERR_INVALIDCAPS : sprintf(dderr, "DDERR_INVALIDCAPS: One or more of the capability bits passed to the callback function are incorrect."); break;
    case DDERR_INVALIDCLIPLIST : sprintf(dderr, "DDERR_INVALIDCLIPLIST: DirectDraw does not support the provided clip list."); break;
    case DDERR_INVALIDMODE : sprintf(dderr, "DDERR_INVALIDMODE: DirectDraw does not support the requested mode."); break;
    case DDERR_INVALIDOBJECT : sprintf(dderr, "DDERR_INVALIDOBJECT: DirectDraw received a pointer that was an invalid DirectDraw object."); break;
    case DDERR_INVALIDPARAMS : sprintf(dderr, "DDERR_INVALIDPARAMS: One or more of the parameters passed to the method are incorrect."); break;
    case DDERR_INVALIDPIXELFORMAT : sprintf(dderr, "DDERR_INVALIDPIXELFORMAT: The pixel format was invalid as specified."); break;
    case DDERR_INVALIDRECT : sprintf(dderr, "DDERR_INVALIDRECT: The provided rectangle was invalid."); break;
    case DDERR_LOCKEDSURFACES : sprintf(dderr, "DDERR_LOCKEDSURFACES: One or more surfaces are locked, causing the failure of the requested operation."); break;
    case DDERR_NO3D : sprintf(dderr, "DDERR_NO3D: No 3-D hardware or emulation is present."); break;
    case DDERR_NOALPHAHW : sprintf(dderr, "DDERR_NOALPHAHW: No alpha-acceleration hardware is present or available, causing the failure of the requested operation."); break;
    case DDERR_NOCLIPLIST : sprintf(dderr, "DDERR_NOCLIPLIST: No clip list is available."); break;
    case DDERR_NOCOLORCONVHW : sprintf(dderr, "DDERR_NOCOLORCONVHW: No color-conversion hardware is present or available."); break;
    case DDERR_NOCOOPERATIVELEVELSET : sprintf(dderr, "DDERR_NOCOOPERATIVELEVELSET: A create function was called without the IDirectDraw7::SetCooperativeLevel method."); break;
    case DDERR_NOCOLORKEY : sprintf(dderr, "DDERR_NOCOLORKEY: The surface does not currently have a color key."); break;
    case DDERR_NOCOLORKEYHW : sprintf(dderr, "DDERR_NOCOLORKEYHW: There is no hardware support for the destination color key."); break;
    case DDERR_NODIRECTDRAWSUPPORT : sprintf(dderr, "DDERR_NODIRECTDRAWSUPPORT: DirectDraw support is not possible with the current display driver."); break;
    case DDERR_NOEXCLUSIVEMODE : sprintf(dderr, "DDERR_NOEXCLUSIVEMODE: The operation requires the application to have exclusive mode, but the application does not have exclusive mode."); break;
    case DDERR_NOFLIPHW : sprintf(dderr, "DDERR_NOFLIPHW: Flipping visible surfaces is not supported."); break;
    case DDERR_NOGDI : sprintf(dderr, "DDERR_NOGDI: No GDI is present."); break;
    case DDERR_NOMIRRORHW : sprintf(dderr, "DDERR_NOMIRRORHW: No mirroring hardware is present or available."); break;
    case DDERR_NOTFOUND : sprintf(dderr, "DDERR_NOTFOUND: The requested item was not found."); break;
    case DDERR_NOOVERLAYHW : sprintf(dderr, "DDERR_NOOVERLAYHW: No overlay hardware is present or available."); break;
    case DDERR_NORASTEROPHW : sprintf(dderr, "DDERR_NORASTEROPHW: No appropriate raster-operation hardware is present or available."); break;
    case DDERR_NOROTATIONHW : sprintf(dderr, "DDERR_NOROTATIONHW: No rotation hardware is present or available."); break;
    case DDERR_NOSTRETCHHW : sprintf(dderr, "DDERR_NOSTRETCHHW: There is no hardware support for stretching."); break;
    case DDERR_NOT4BITCOLOR : sprintf(dderr, "DDERR_NOT4BITCOLOR: The DirectDrawSurface object is not using a 4-bit color palette, and the requested operation requires a 4-bit color palette."); break;
    case DDERR_NOT4BITCOLORINDEX : sprintf(dderr, "DDERR_NOT4BITCOLORINDEX: The DirectDrawSurface object is not using a 4-bit color index palette, and the requested operation requires a 4-bit color index palette."); break;
    case DDERR_NOT8BITCOLOR : sprintf(dderr, "DDERR_NOT8BITCOLOR: The DirectDrawSurface object is not using an 8-bit color palette, and the requested operation requires an 8-bit color palette."); break;
    case DDERR_NOTEXTUREHW : sprintf(dderr, "DDERR_NOTEXTUREHW: The operation cannot be carried out because no texture-mapping hardware is present or available."); break;
    case DDERR_NOVSYNCHW : sprintf(dderr, "DDERR_NOVSYNCHW: There is no hardware support for vertical blank synchronized operations."); break;
    case DDERR_NOZBUFFERHW : sprintf(dderr, "DDERR_NOZBUFFERHW: The operation to create a z-buffer in display memory or to perform a blit, using a z-buffer cannot be carried out because there is no hardware support for z-buffers."); break;
    case DDERR_NOZOVERLAYHW : sprintf(dderr, "DDERR_NOZOVERLAYHW: The overlay surfaces cannot be z-layered, based on the z-order because the hardware does not support z-ordering of overlays."); break;
    case DDERR_OUTOFCAPS : sprintf(dderr, "DDERR_OUTOFCAPS: The hardware needed for the requested operation has already been allocated."); break;
    case DDERR_OUTOFMEMORY : sprintf(dderr, "DDERR_OUTOFMEMORY: DirectDraw does not have enough memory to perform the operation."); break;
    case DDERR_OUTOFVIDEOMEMORY : sprintf(dderr, "DDERR_OUTOFVIDEOMEMORY: DirectDraw does not have enough display memory to perform the operation."); break;
    case DDERR_OVERLAYCANTCLIP : sprintf(dderr, "DDERR_OVERLAYCANTCLIP: The hardware does not support clipped overlays."); break;
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE : sprintf(dderr, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE: An attempt was made to have more than one color key active on an overlay."); break;
    case DDERR_PALETTEBUSY : sprintf(dderr, "DDERR_PALETTEBUSY: Access to this palette is refused because the palette is locked by another thread."); break;
    case DDERR_COLORKEYNOTSET : sprintf(dderr, "DDERR_COLORKEYNOTSET: No source color key is specified for this operation."); break;
    case DDERR_SURFACEALREADYATTACHED : sprintf(dderr, "DDERR_SURFACEALREADYATTACHED: An attempt was made to attach a surface to another surface to which it is already attached."); break;
    case DDERR_SURFACEALREADYDEPENDENT : sprintf(dderr, "DDERR_SURFACEALREADYDEPENDENT: An attempt was made to make a surface a dependency of another surface on which it is already dependent."); break;
    case DDERR_SURFACEBUSY : sprintf(dderr, "DDERR_SURFACEBUSY: Access to the surface is refused because the surface is locked by another thread."); break;
    case DDERR_CANTLOCKSURFACE : sprintf(dderr, "DDERR_CANTLOCKSURFACE: Access to this surface is refused because an attempt was made to lock the primary surface without DCI support."); break;
    case DDERR_SURFACEISOBSCURED : sprintf(dderr, "DDERR_SURFACEISOBSCURED: Access to the surface is refused because the surface is obscured."); break;
    case DDERR_SURFACELOST : sprintf(dderr, "DDERR_SURFACELOST: Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface7::Restore method on this surface to restore the memory associated with it."); break;
    case DDERR_SURFACENOTATTACHED : sprintf(dderr, "DDERR_SURFACENOTATTACHED: The requested surface is not attached."); break;
    case DDERR_TOOBIGHEIGHT : sprintf(dderr, "DDERR_TOOBIGHEIGHT: The height requested by DirectDraw is too large."); break;
    case DDERR_TOOBIGSIZE : sprintf(dderr, "DDERR_TOOBIGSIZE: The size requested by DirectDraw is too large. However, the individual height and width are valid sizes."); break;
    case DDERR_TOOBIGWIDTH : sprintf(dderr, "DDERR_TOOBIGWIDTH: The width requested by DirectDraw is too large."); break;
    case DDERR_UNSUPPORTED : sprintf(dderr, "DDERR_UNSUPPORTED: The operation is not supported."); break;
    case DDERR_UNSUPPORTEDFORMAT : sprintf(dderr, "DDERR_UNSUPPORTEDFORMAT: The pixel format requested is not supported by DirectDraw."); break;
    case DDERR_UNSUPPORTEDMASK : sprintf(dderr, "DDERR_UNSUPPORTEDMASK: The bitmask in the pixel format requested is not supported by DirectDraw."); break;
    case DDERR_VERTICALBLANKINPROGRESS : sprintf(dderr, "DDERR_VERTICALBLANKINPROGRESS: A vertical blank is in progress."); break;
    case DDERR_WASSTILLDRAWING : sprintf(dderr, "DDERR_WASSTILLDRAWING: The previous blit operation that is transferring information to or from this surface is incomplete."); break;
    case DDERR_XALIGN : sprintf(dderr, "DDERR_XALIGN: The provided rectangle was not horizontally aligned on a required boundary."); break;
    case DDERR_INVALIDDIRECTDRAWGUID : sprintf(dderr, "DDERR_INVALIDDIRECTDRAWGUID: The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier."); break;
    case DDERR_DIRECTDRAWALREADYCREATED : sprintf(dderr, "DDERR_DIRECTDRAWALREADYCREATED: A DirectDraw object representing this driver has already been created for this process."); break;
    case DDERR_NODIRECTDRAWHW : sprintf(dderr, "DDERR_NODIRECTDRAWHW: Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware."); break;
    case DDERR_PRIMARYSURFACEALREADYEXISTS : sprintf(dderr, "DDERR_PRIMARYSURFACEALREADYEXISTS: This process has already created a primary surface."); break;
    case DDERR_NOEMULATION : sprintf(dderr, "DDERR_NOEMULATION: Software emulation is not available."); break;
    case DDERR_REGIONTOOSMALL : sprintf(dderr, "DDERR_REGIONTOOSMALL: The region passed to the IDirectDrawClipper::GetClipList method is too small."); break;
    case DDERR_CLIPPERISUSINGHWND : sprintf(dderr, "DDERR_CLIPPERISUSINGHWND: An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle."); break;
    case DDERR_NOCLIPPERATTACHED : sprintf(dderr, "DDERR_NOCLIPPERATTACHED: No DirectDrawClipper object is attached to the surface object."); break;
    case DDERR_NOHWND : sprintf(dderr, "DDERR_NOHWND: Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle."); break;
    case DDERR_HWNDSUBCLASSED : sprintf(dderr, "DDERR_HWNDSUBCLASSED: DirectDraw is prevented from restoring state because the DirectDraw cooperative-level window handle has been subclassed."); break;
    case DDERR_HWNDALREADYSET : sprintf(dderr, "DDERR_HWNDALREADYSET: The DirectDraw cooperative-level window handle has already been set. It cannot be reset while the process has surfaces or palettes created."); break;
    case DDERR_NOPALETTEATTACHED : sprintf(dderr, "DDERR_NOPALETTEATTACHED: No palette object is attached to this surface."); break;
    case DDERR_NOPALETTEHW : sprintf(dderr, "DDERR_NOPALETTEHW: There is no hardware support for 16- or 256-color palettes."); break;
    case DDERR_BLTFASTCANTCLIP : sprintf(dderr, "DDERR_BLTFASTCANTCLIP: A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface7::BltFast method."); break;
    case DDERR_NOBLTHW : sprintf(dderr, "DDERR_NOBLTHW: No blitter hardware is present."); break;
    case DDERR_NODDROPSHW : sprintf(dderr, "DDERR_NODDROPSHW: No DirectDraw raster-operation (ROP) hardware is available."); break;
    case DDERR_OVERLAYNOTVISIBLE : sprintf(dderr, "DDERR_OVERLAYNOTVISIBLE: The IDirectDrawSurface7::GetOverlayPosition method was called on a hidden overlay."); break;
    case DDERR_NOOVERLAYDEST : sprintf(dderr, "DDERR_NOOVERLAYDEST: The IDirectDrawSurface7::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface7::UpdateOverlay method has not been called on to establish as a destination."); break;
    case DDERR_INVALIDPOSITION : sprintf(dderr, "DDERR_INVALIDPOSITION: The position of the overlay on the destination is no longer legal."); break;
    case DDERR_NOTAOVERLAYSURFACE : sprintf(dderr, "DDERR_NOTAOVERLAYSURFACE: An overlay component is called for a nonoverlay surface."); break;
    case DDERR_EXCLUSIVEMODEALREADYSET : sprintf(dderr, "DDERR_EXCLUSIVEMODEALREADYSET: An attempt was made to set the cooperative level when it was already set to exclusive."); break;
    case DDERR_NOTFLIPPABLE : sprintf(dderr, "DDERR_NOTFLIPPABLE: An attempt was made to flip a surface that cannot be flipped."); break;
    case DDERR_CANTDUPLICATE : sprintf(dderr, "DDERR_CANTDUPLICATE: Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated."); break;
    case DDERR_NOTLOCKED : sprintf(dderr, "DDERR_NOTLOCKED: An attempt was made to unlock a surface that was not locked."); break;
    case DDERR_CANTCREATEDC : sprintf(dderr, "DDERR_CANTCREATEDC: Windows cannot create any more device contexts (DCs), or a DC has requested a palette-indexed surface when the surface had no palette and the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC)."); break;
    case DDERR_NODC : sprintf(dderr, "DDERR_NODC: No device context (DC) has ever been created for this surface."); break;
    case DDERR_WRONGMODE : sprintf(dderr, "DDERR_WRONGMODE: This surface cannot be restored because it was created in a different mode."); break;
    case DDERR_IMPLICITLYCREATED : sprintf(dderr, "DDERR_IMPLICITLYCREATED: The surface cannot be restored because it is an implicitly created surface."); break;
    case DDERR_NOTPALETTIZED : sprintf(dderr, "DDERR_NOTPALETTIZED: The surface being used is not a palette-based surface."); break;
    case DDERR_UNSUPPORTEDMODE : sprintf(dderr, "DDERR_UNSUPPORTEDMODE: The display is currently in an unsupported mode."); break;
    case DDERR_NOMIPMAPHW : sprintf(dderr, "DDERR_NOMIPMAPHW: No mipmap-capable texture mapping hardware is present or available."); break;
    case DDERR_INVALIDSURFACETYPE : sprintf(dderr, "DDERR_INVALIDSURFACETYPE: The surface was of the wrong type."); break;
    case DDERR_DCALREADYCREATED : sprintf(dderr, "DDERR_DCALREADYCREATED: A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface."); break;
    case DDERR_CANTPAGELOCK : sprintf(dderr, "DDERR_CANTPAGELOCK: An attempt to page-lock a surface failed. Page lock does not work on a display-memory surface or an emulated primary surface."); break;
    case DDERR_CANTPAGEUNLOCK : sprintf(dderr, "DDERR_CANTPAGEUNLOCK: An attempt to page-unlock a surface failed. Page unlock does not work on a display-memory surface or an emulated primary surface."); break;
    case DDERR_NOTPAGELOCKED : sprintf(dderr, "DDERR_NOTPAGELOCKED: An attempt was made to page-unlock a surface with no outstanding page locks."); break;
    case DDERR_NOTINITIALIZED : sprintf(dderr, "DDERR_NOTINITIALIZED: An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized."); break;

    default : sprintf(dderr, "Unknown Error"); break;
    }
    
    Log::Note(Log::CHANNEL_GRAPHICS, Log::PRIORITY_ERROR, dderr);
}

LPDIRECTDRAWCLIPPER Graphics::AttachClipper(LPDIRECTDRAWSURFACE7 pDDSurface, LPRECT clipList, s32 count)
{
    b32 bResult = true;
    LPDIRECTDRAWCLIPPER pDDClipper;
    LPRGNDATA pRegionData;

    // Create clipper
    if ( FAILED(m_pDDraw->CreateClipper(0, &pDDClipper, NULL)) )
        return NULL;

    // Init region data
    pRegionData = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + sizeof(RECT)*count);
    memcpy(pRegionData->Buffer, clipList, sizeof(RECT)*count);

    // Set region data header
    pRegionData->rdh.dwSize = sizeof(RGNDATAHEADER);
    pRegionData->rdh.iType = RDH_RECTANGLES;
    pRegionData->rdh.nCount = count;
    pRegionData->rdh.nRgnSize = count*sizeof(RECT);

    pRegionData->rdh.rcBound.left = 64000;
    pRegionData->rdh.rcBound.right = -64000;
    pRegionData->rdh.rcBound.top = 64000;
    pRegionData->rdh.rcBound.bottom = -64000;

    // Resize bound
    for (s32 i = 0; i < count; i++)
    {
        // Left
        if (clipList[i].left < pRegionData->rdh.rcBound.left)
            pRegionData->rdh.rcBound.left = clipList[i].left;
        // Right
        if (clipList[i].right > pRegionData->rdh.rcBound.right)
            pRegionData->rdh.rcBound.right = clipList[i].right;
        // Top
        if (clipList[i].top < pRegionData->rdh.rcBound.top)
            pRegionData->rdh.rcBound.top = clipList[i].top;
        // Bottom
        if (clipList[i].bottom > pRegionData->rdh.rcBound.bottom)
            pRegionData->rdh.rcBound.bottom = clipList[i].bottom;
    }

    // Set clip list
    if ( FAILED(pDDClipper->SetClipList(pRegionData, 0)) )
        bResult = false;

    // Set clipper for surface
    if ( FAILED(pDDSurface->SetClipper(pDDClipper)) )
        bResult = false;

    // Free memory
    free(pRegionData);

    // Return clipper
    if (bResult)
    {
        return pDDClipper;
    }
    else
    {
        pDDClipper->Release();
        return NULL;
    }
}

LPDIRECTDRAWSURFACE7 Graphics::CreateSurface(s32 width, s32 height, b32 bVideoMemory, b32 bColorKey)
{
    // Init structures
    DDSURFACEDESC2 DDSurfaceDesc;
    LPDIRECTDRAWSURFACE7 pDDSurface;

    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

    DDSurfaceDesc.dwWidth = width;
    DDSurfaceDesc.dwHeight = height;

    // Place to which memory
    if (bVideoMemory)
        DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
    else
        DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

    // Create surface
    if ( FAILED(m_pDDraw->CreateSurface(&DDSurfaceDesc, &pDDSurface, NULL)) && bVideoMemory )
    {
        // Try to place stuff in system memory
        DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        s32 res = m_pDDraw->CreateSurface(&DDSurfaceDesc, &pDDSurface, NULL);
        if (FAILED(res))
        {
            Log::Note(Log::CHANNEL_GRAPHICS, Log::PRIORITY_ERROR, "Can't make surface %dx%d, check DDrawError below", width, height);
            DDrawError(res);
            return NULL;
        }
        Log::Note(Log::CHANNEL_GRAPHICS, Log::PRIORITY_WARNING, "Have no videomemory for %dx%d surface, put it in system memory", width, height);
    }

    // Set color key
    if (bColorKey)
    {
        DDCOLORKEY DDColorKey;
        DDColorKey.dwColorSpaceLowValue = COLOR_KEY;
        DDColorKey.dwColorSpaceHighValue = COLOR_KEY;

        pDDSurface->SetColorKey(DDCKEY_SRCBLT, &DDColorKey);
    }

    return pDDSurface;
}

LPDIRECTDRAWSURFACE7 Graphics::LoadBMP(const char* fileName)
{
    BMPFile bmp(fileName);
    if (!bmp.buffer)
        return NULL;

    // Create surface
    LPDIRECTDRAWSURFACE7 pDDSurface = CreateSurface(bmp.info.biWidth, bmp.info.biHeight);
    if (!pDDSurface)
        return NULL;

    // Init description
    DDSURFACEDESC2 DDSurfaceDesc;
    DDRAW_INIT_STRUCT(DDSurfaceDesc);

    // Copy
    pDDSurface->Lock(NULL, &DDSurfaceDesc, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL);

    u8* dst = (u8*)DDSurfaceDesc.lpSurface;
    u8* src = (u8*)bmp.buffer;

    s32 surfacePitch = DDSurfaceDesc.lPitch;
    s32 bmpPitch = bmp.info.biWidth * (bmp.info.biBitCount/8);

    for (s32 i = 0; i < bmp.info.biHeight; i++)
    {
        memcpy(dst, src, bmpPitch); // TODO/NOTE bitmap buffer contains pixels in BGR format

        dst += surfacePitch;
        src += bmpPitch;
    }

    pDDSurface->Unlock(NULL);

    return pDDSurface;
}

void Graphics::EmulationBlit(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h)
{
    videoBuffer += posY*pitch32 + posX; // Start position for videoBuffer pointer

    for (s32 y = 0; y < h; y++)
    {
        for (s32 x = 0; x < w; x++)
        {
            u32 pixel;
            if ((pixel = bitMap[x])) // Plot opaque pixels only
                videoBuffer[x] = pixel;
        }
        videoBuffer += pitch32;
        bitMap += w;
    }
}

void Graphics::EmulationBlitClipped(u32* videoBuffer, s32 pitch32, s32 posX, s32 posY, u32* bitMap, s32 w, s32 h)
{
    // Check if it's visible
    if (posX >= m_screenWidth  || posX + w <= 0 ||
        posY >= m_screenHeight || posY + h <= 0)
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
    if (posX + w > m_screenWidth)
        dst.right = m_screenWidth - 1;
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
    if (posY + h > m_screenHeight)
        dst.bottom = m_screenHeight - 1;
    else
        dst.bottom = (posY + h) - 1;

    // Difference
    dX = dst.right - dst.left + 1;
    dY = dst.bottom - dst.top + 1;

    // Start position
    videoBuffer += dst.top*pitch32 + dst.left;
    bitMap += srcOffsetY*w + srcOffsetX;

    // Blitting
    for (s32 y = 0; y < dY; y++)
    {
        for (s32 x = 0; x < dX; x++)
        {
            u32 pixel;
            if ((pixel = bitMap[x]) != _RGB32BIT(255, 0, 0, 0)) // Plot opaque pixels only
                videoBuffer[x] = pixel;
        }
        videoBuffer += pitch32;
        bitMap += w;
    }
}