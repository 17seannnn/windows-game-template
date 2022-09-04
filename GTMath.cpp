/* ====== TODO ======
 */

/* ====== INCLUDES ====== */
#include <math.h>

#include "GTMath.h"

/* ====== VARIABLES ====== */
static f32 m_sinLook[361];
static f32 m_cosLook[361];

const Mat44 g_IMat44 = { 1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0,
                         0, 0, 0, 1 };

const Mat43 g_IMat43 = { 1, 0, 0,
                         0, 1, 0,
                         0, 0, 1,
                         0, 0, 0 };

const Mat33 g_IMat33 = { 1, 0, 0,
                         0, 1, 0,
                         0, 0, 1 };

const Mat22 g_IMat22 = { 1, 0,
                         0, 1 };

/* ====== METHODS ====== */
b32 GTM::StartUp()
{
    // Sin/Cos look
    for (s32 i = 0; i < 361; ++i)
    {
        f32 angle = DEG_TO_RAD((f32)i);
        m_sinLook[i] = sinf(angle);
        m_cosLook[i] = cosf(angle);
    }

    return true;
}

void GTM::ShutDown()
{
}

f32 SinLook(s32 angle)
{
    return m_sinLook[angle];
}

f32 CosLook(s32 angle)
{
    return m_cosLook[angle];
}

s32 GTM::FastDist2(s32 x, s32 y)
{
    // Get absolute coordinates
    x = abs(x);
    y = abs(y);

    // Get minimal value
    s32 min = MIN(x, y);

    // Return distance
    return x + y - (min >> 1) - (min >> 2) + (min >> 4);
}

f32 GTM::FastDist3(f32 fx, f32 fy, f32 fz)
{
    // Absolute values
    s32 x = (s32)(fabsf(fx) * 1024);
    s32 y = (s32)(fabsf(fy) * 1024);
    s32 z = (s32)(fabsf(fz) * 1024);

    s32 temp;
    if (x > y) SWAP(x, y, temp);
    if (y > z) SWAP(y, z, temp);
    if (x > y) SWAP(x, y, temp);

    s32 dist = z + 11*(y >> 5) + (x >> 2);

    return (f32)(dist >> 10);
}

void GTM::TranslatePoly2(Poly2* poly, f32 dx, f32 dy)
{
    if (!poly)
        return;

    poly->x += dx;
    poly->y += dy;
}

void GTM::RotatePoly2(Poly2* poly, s32 angle)
{
    if (!poly)
        return;

    for (s32 i = 0; i < poly->vtxCount; ++i)
    {
        f32 x = poly->aVtx[i].x*m_cosLook[angle] - poly->aVtx[i].y*m_sinLook[angle];
        f32 y = poly->aVtx[i].x*m_sinLook[angle] + poly->aVtx[i].y*m_cosLook[angle];

        poly->aVtx[i].x = x;
        poly->aVtx[i].y = y;
    }
}

void GTM::ScalePoly2(Poly2* poly, f32 scaleX, f32 scaleY)
{
    if (!poly)
        return;

    for (s32 i = 0; i < poly->vtxCount; ++i)
    {
        poly->aVtx[i].x *= scaleX;
        poly->aVtx[i].y *= scaleY;
    }
}

b32 GTM::FindBoxPoly2(Poly2* poly, f32 minX, f32 minY, f32 maxX, f32 maxY)
{
    // Check if polygon is correct
    if (!poly || poly->vtxCount <= 0)
        return false;

    // Init coords
    minX = minY = maxX = maxY = 0;

    // Find box
    for (s32 i = 0; i < poly->vtxCount; ++i)
    {
        if (poly->aVtx[i].x < minX)
            minX = poly->aVtx[i].x;
        if (poly->aVtx[i].x > maxX)
            maxX = poly->aVtx[i].x;

        if (poly->aVtx[i].y < minY)
            minY = poly->aVtx[i].y;
        if (poly->aVtx[i].y > maxY)
            maxY = poly->aVtx[i].y;
    }

    return true;
}

void GTM::MulMat33(const Mat33* m1, const Mat33* m2, Mat33* mr)
{
    for (s32 row = 0; row < 3; ++row)
    {
        for (s32 col = 0; col < 3; ++col)
        {
            f32 sum = 0;

            for (s32 i = 0; i < 3; ++i)
                sum += m1->c[row][i] * m2->c[i][col];

            mr->c[row][col] = sum;
        }
    }
}

void GTM::MulMat13x33(const Mat13* m1, const Mat33* m2, Mat13* mr)
{
    for (s32 col = 0; col < 3; ++col)
    {
        f32 sum = 0;

        for (s32 i = 0; i < 3; ++i)
            sum += m1->c[i] * m2->c[i][col];

        mr->c[col] = sum;
    }
}

void GTM::MulMat12x32(const Mat12* m1, const Mat32* m2, Mat12* mr)
{
    for (s32 col = 0; col < 2; ++col)
    {
        f32 sum = 0;

        // Sum first two multiplications
        for (s32 i = 0; i < 2; ++i)
            sum += m1->c[i] * m2->c[i][col];

        // Sum last fictive element of first matrix with last element of second
        sum += m2->c[2][col];

        // Set element
        mr->c[col] = sum;
    }
}

fixed16 GTM::MulFixed16(fixed16 f1, fixed16 f2)
{
    fixed16 res;

    _asm
    {
        mov     eax, f1         // f1 -> eax
        imul    f2              // f1 * f2 = EDX:EAX
        shrd    eax, edx, 16    // 16 low bits of eax -> edx
        mov     res, eax        // eax -> res
    }

    return res;
}

fixed16 GTM::DivFixed16(fixed16 f1, fixed16 f2)
{
    return 0; // TODO(sean)
}
