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

namespace Math
{
    b32 StartUp();
    void ShutDown();

    f32 SinLook(s32 angle);
    f32 CosLook(s32 angle);

    // Fast distance functions return distance between zero point and xy/xyz point
    s32 FastDist2(s32 x, s32 y);
    f32 FastDist3(f32 x, f32 y, f32 z);

    void TranslatePolygon2(Polygon2* poly, f32 dx, f32 dy);
    void RotatePolygon2(Polygon2* poly, s32 angle);
    void ScalePolygon2(Polygon2* poly, f32 scaleX, f32 scaleY);

    b32 FindBoxPoly2(Polygon2* poly, f32 minX, f32 minY, f32 maxX, f32 maxY);

    void MulMat33(const Mat33& m1, const Mat33& m2, Mat33& mr);
    void MulMat13x33(const Mat13& m1, const Mat33& m2, Mat13& mr);
    void MulMat12x32(const Mat12& m1, const Mat32& m2, Mat12& mr);
};

#endif // MATH_H_