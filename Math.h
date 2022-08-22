#ifndef MATH_H_
#define MATH_H_

/* ====== INCLUDES ====== */
#include <stddef.h>

#include "Types.h"

/* ====== DEFINES ====== */
#ifndef PI
#define PI 3.1415926535f
#endif

/* ====== STRUCTURES ====== */
struct SVertex2
{
    s32 x, y;
};

struct FVertex2
{
    f32 x, y;
};

struct Polygon2
{
    s32 state;
    s32 vertexCount;
    f32 x, y;
    f32 vx, vy;
    s32 color;
    FVertex2* aVertex;

    Polygon2() : aVertex(NULL) {}
};

struct Mat33
{
    f32 c[3][3];
};

struct Mat13
{
    f32 c[3];
};

struct Mat32
{
    f32 c[3][2];
};

struct Mat12
{
    f32 c[2];
};

// Static class
class Math
{
    static f32 m_sinLook[361];
    static f32 m_cosLook[361];
public:
    static b32 StartUp();
    static void ShutDown();

    static f32 SinLook(s32 angle) { return m_sinLook[angle]; }
    static f32 CosLook(s32 angle) { return m_cosLook[angle]; }

    static void TranslatePolygon2(Polygon2* poly, f32 dx, f32 dy);
    static void RotatePolygon2(Polygon2* poly, s32 angle);
    static void ScalePolygon2(Polygon2* poly, f32 scaleX, f32 scaleY);

    static void MulMat33(const Mat33& m1, const Mat33& m2, Mat33& mr);
    static void MulMat13x33(const Mat13& m1, const Mat33& m2, Mat13& mr);
    static void MulMat12x32(const Mat12& m1, const Mat32& m2, Mat12& mr);
};

#endif // MATH_H_