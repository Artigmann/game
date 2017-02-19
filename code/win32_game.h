#if !defined(WIN32_GAME_H)
#define WIN32_GAME_H

#include <stdint.h>

#define arrayLength(array) sizeof(array)/sizeof(array[0])

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

#define MAX_SHADER_CODE_SIZE 1024
//NOTE make one struct for all shaders??
struct shaderData
{
    GLuint shader;
    BOOL fileLoaded;
    GLboolean isModified;
    char filePath[512];
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    LARGE_INTEGER fileSize;
    FILETIME lastWriteTime;
    char code[MAX_SHADER_CODE_SIZE];    
};
//NOTE like this??
struct shaders
{
    GLuint shaderProgram;
    struct shaderData vertex;
    struct shaderData fragmentShader;
};

struct player
{
    
};

#endif
