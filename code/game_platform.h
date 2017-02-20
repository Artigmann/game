#if !defined(GAME_PLATFORM_H)
#define GAME_PLATFORM_H

#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int32_t bool32;

typedef float real32;
typedef double real64;


#define pi32 3.14159265359f

union v2
{
    struct
    {
        real32 x, y;
    };
    real32 E[2];
};

union v3
{
    struct
    {
        real32 x, y, z;
    };
    real32 E[3];
};

union v4
{
    struct
    {
        union
        {
            v3 xyz;
            struct
            {
                real32 x, y, z;
            };
        };
         real32 w;
    };
};

struct m4x4
{
    // NOTE(casey): These are stored ROW MAJOR - E[ROW][COLUMN]!!!
    real32 E[4][4];
};


#endif
