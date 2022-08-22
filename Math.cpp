/* ====== INCLUDES ====== */
#include <math.h>

#include "Log.h"

#include "Math.h"

/* ====== DEFINES ====== */
#define DEG_TO_RAD(DEG) ((DEG) * PI/180)
#define RAD_TO_DEG(RAD) ((RAD) * 180/PI)

/* ====== VARIABLES ====== */
f32 Math::m_sinLook[361];
f32 Math::m_cosLook[361];

/* ====== METHODS ====== */
b32 Math::StartUp()
{
    // Sin/Cos look
    for (s32 i = 0; i < 361; ++i)
    {
        f32 angle = DEG_TO_RAD((f32)i);
        m_sinLook[i] = sinf(angle);
        m_cosLook[i] = cosf(angle);
    }

    // Add note
    Log::Note(Log::CHANNEL_MATH, Log::PRIORITY_NOTE, "Module started");

    return true;
}

void Math::ShutDown()
{
    // Add note
    Log::Note(Log::CHANNEL_MATH, Log::PRIORITY_NOTE, "Module shut down");
}

void Math::TranslatePolygon2(Polygon2* poly, f32 dx, f32 dy)
{
    if (!poly)
        return;

    poly->x += dx;
    poly->y += dy;
}

void Math::RotatePolygon2(Polygon2* poly, s32 angle)
{
    if (!poly)
        return;

    for (s32 i = 0; i < poly->vertexCount; ++i)
    {
        f32 x = poly->aVertex[i].x*m_cosLook[angle] - poly->aVertex[i].y*m_sinLook[angle];
        f32 y = poly->aVertex[i].x*m_sinLook[angle] + poly->aVertex[i].y*m_cosLook[angle];

        poly->aVertex[i].x = x;
        poly->aVertex[i].y = y;
    }
}

void Math::ScalePolygon2(Polygon2* poly, f32 scaleX, f32 scaleY)
{
    if (!poly)
        return;

    for (s32 i = 0; i < poly->vertexCount; ++i)
    {
        poly->aVertex[i].x *= scaleX;
        poly->aVertex[i].y *= scaleY;
    }
}

void Math::MulMat33(const Mat33& m1, const Mat33& m2, Mat33& mr)
{
    for (s32 row = 0; row < 3; ++row)
    {
        for (s32 col = 0; col < 3; ++col)
        {
            f32 sum = 0;

            for (s32 i = 0; i < 3; ++i)
                sum += m1.c[row][i] * m2.c[i][col];

            mr.c[row][col] = sum;
        }
    }
}

void Math::MulMat13x33(const Mat13& m1, const Mat33& m2, Mat13& mr)
{
    for (s32 col = 0; col < 3; ++col)
    {
        f32 sum = 0;

        for (s32 i = 0; i < 3; ++i)
            sum += m1.c[i] * m2.c[i][col];

        mr.c[col] = sum;
    }
}

void Math::MulMat12x32(const Mat12& m1, const Mat32& m2, Mat12& mr)
{
    /* NOTE Just a memo...
     *  Rotation:
     *  [ 100 150 ]
     *       *
     *  [ cos   sin  0 ]
     *  [ -sin  cos  0 ]
     *  [  0     0   1 ]
     *
     *  Translation:
     *  [ 100 150 ] - Vector
     *       *
     *  [ 1   0   0 ] - Matrix
     *  [ 0   1   0 ]
     *  [ dx  dy  1 ]
     */

    for (s32 col = 0; col < 2; ++col)
    {
        f32 sum = 0;

        // Sum first two multiplications
        for (s32 i = 0; i < 2; ++i)
            sum += m1.c[i] * m2.c[i][col];

        // Sum last fictive element of first matrix with last element of second
        sum += m2.c[2][col];

        // Set element
        mr.c[col] = sum;
    }
}