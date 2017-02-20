#if !defined(WIN32_GAME_H)
#define WIN32_GAME_H



#define arrayLength(array) sizeof(array)/sizeof(array[0])

#define MAX_SHADER_CODE_SIZE 1024
//NOTE make one struct for all shaders??
struct shaderData
{
    GLuint shader;
    BOOL fileLoaded;
    BOOL isModified;
    char filePath[512];
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    LARGE_INTEGER fileSize;
    FILETIME lastWriteTime;
    char code[MAX_SHADER_CODE_SIZE];    
};
//NOTE like this??
struct shader
{
    GLuint shaderProgram;
    struct shaderData geometry;
    struct shaderData vertex;
    struct shaderData fragmentShader;
};

struct gameObject
{
    v2 position, size, velocity;
    v3 color;

    glm::vec2 glmPos, glmSize;
    glm::vec3 glmColor;
    
    real32 rotation;
    bool32 solid;
};

#define LEVEL_WIDTH 15
#define LEVEL_HEIGHT 8

struct gameLevel
{
    struct gameObject tiles[LEVEL_HEIGHT*LEVEL_WIDTH];
};

struct game
{
    struct gameLevel level;
    struct gameObject player;
};

#endif
