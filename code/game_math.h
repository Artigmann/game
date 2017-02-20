#if !defined(GAME_MATH_H)
#define GAME_MATH_H

#include <math.h>

inline v2 operator*(real32 a, v2 b)
{
    v2 result;
    result.x = a*b.x;
    result.y = a*b.y;

    return result;
}

inline v2 operator*(v2 b, real32 a)
{
    v2 result = a*b;
    return result;
}

inline v2 & operator*=(v2 &b, real32 a)
{
    b = a*b;
    return b;
}

inline v2 operator-(v2 a)
{
    v2 result;
    result.x = -a.x;
    result.y = -a.y;

    return result;
}

inline v2 operator+(v2 a, v2 b)
{
    v2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

inline v2 & operator+=(v2 &a, v2 b)
{
    a = a + b;
    return a;
}

inline v2 operator-(v2 a, v2 b)
{
    v2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

inline v2 & operator-=(v2 &a, v2 b)
{
    a = a - b;
    return a;
}


inline v3 operator*(real32 a, v3 b)
{
    v3 result;
    result.x = a*b.x;
    result.y = a*b.y;
    result.z = a*b.z;

    return result;
}

inline v3 operator*(v3 b, real32 a)
{
    v3 result = a*b;
    return result;
}

inline v3 & operator*=(v3 &b, real32 a)
{
    b = a*b;
    return b;
}

inline v3 operator-(v3 a)
{
    v3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}

inline v3 operator+(v3 a, v3 b)
{
    v3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline v3 & operator+=(v3 &a, v3 b)
{
    a = a + b;
    return a;
}

inline v3 operator-(v3 a, v3 b)
{
    v3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

inline v3 & operator-=(v3 &a, v3 b)
{
    a = a - b;
    return a;
}

static inline real32 dotV3(v3 a, v3 b)
{
    float result = 0.0f;

    result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    
    return result;
}

static inline real32 lengthSquaredV3(v3 a)
{
    real32 result = 0.0f;
    result = dotV3(a, a);

    return result;
}

static inline real32 lengthV3(v3 a)
{
    real32 result = 0.0f;

    result = sqrtf(lengthSquaredV3(a));
    
    return result;

}

static inline v3 normalizeV3(v3 a)    
{
    v3 result = {};

    real32 vectorLength = lengthV3(a);
    
    result.x = a.x * (1.0f / vectorLength);
    result.y = a.y * (1.0f / vectorLength);
    result.z = a.z * (1.0f / vectorLength);
    
    return result;
}

static inline real32 toRadians(real32 degrees)
{
    real32 result = 0.0f;

    result = degrees * (pi32 / 180.0f);
    return result;
}

static m4x4 operator*(m4x4 A, m4x4 B)
{
    // NOTE(casey): This is written to be instructive, not optimal!
    
    m4x4 result = {};
    
    for(int r = 0; r <= 3; ++r) // NOTE(casey): Rows (of A)
    {
        for(int c = 0; c <= 3; ++c) // NOTE(casey): Column (of B)
        {
            for(int i = 0; i <= 3; ++i) // NOTE(casey): Columns of A, rows of B!
            {
                result.E[r][c] += A.E[r][i]*B.E[i][c];
            }
        }
    }
    
    return result;
}

static inline m4x4 identity(void)
{
    m4x4  result = 
    {
        {{1, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1}},
    };
    
    return result;
}

static inline m4x4 orthographicProjection(real32 aspectWidthOverHeight)
{
    real32 a = 1.0f;
    real32 b = aspectWidthOverHeight;
    
    m4x4 result =
    {
        {{a,  0,  0,  0},
         {0,  b,  0,  0},
         {0,  0,  1,  0},
         {0,  0,  0,  1}}
    };
    
    return result;
}

static inline m4x4 translate(m4x4 a, v3 t)
{
    m4x4 result = a;
    
    result.E[0][3] += t.x;
    result.E[1][3] += t.y;
    result.E[2][3] += t.z;
    
    return result;
}

static inline m4x4 scale(m4x4 a, v3 t)
{
    m4x4 result = a;
    
    result.E[0][0] *= t.x;
    result.E[1][1] *= t.y;
    result.E[2][2] *= t.y;

    return result;
}

static inline m4x4 zRotation(real32 angle)
{
    real32 c = cosf(angle);
    real32 s = sinf(angle);

    m4x4 result = 
    {
        {{c,-s, 0, 0},
         {s, c, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1}},
    };
    
    return result;
}

//NOTE understand this shit 
static inline m4x4 rotate(m4x4 a, real32 angle, v3 axis)
{
    m4x4 result = identity();
    
    float sinTheta = sinf(toRadians(angle));
    float cosTheta = cosf(toRadians(angle));
    float cosValue = 1.0f - cosTheta;

    axis = normalizeV3(axis);
    
    result.E[0][0] = (axis.x * axis.x * cosValue) + cosTheta;
    result.E[0][1] = (axis.x * axis.y * cosValue) + (axis.z * sinTheta);
    result.E[0][2] = (axis.x * axis.z * cosValue) - (axis.y * sinTheta);
    
    result.E[1][0] = (axis.y * axis.x * cosValue) - (axis.z * sinTheta);
    result.E[1][1] = (axis.y * axis.y * cosValue) + cosTheta;
    result.E[1][2] = (axis.y * axis.z * cosValue) + (axis.x * sinTheta);
    
    result.E[2][0] = (axis.z * axis.x * cosValue) + (axis.y * sinTheta);
    result.E[2][1] = (axis.z * axis.y * cosValue) - (axis.x * sinTheta);
    result.E[2][2] = (axis.z * axis.z * cosValue) + cosTheta;

    a = a * result;
    
    return a;
}

#endif
