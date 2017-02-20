#include <windows.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "game_platform.h"
#include "win32_game.h"
#include "game_math.h"
/*
  1280×720 (HD, 720p)
  1920×1080 (FHD, Full HD, 2K 1080p)
  2560×1440 (QHD, WQHD, Quad HD, 1440p)
*/

static int windowWidth = 1280;
static int windowHeight = 720;

static void readEntireShaderFromFile(struct shaderData *shader, GLenum shaderType)
{
    DWORD bytesRead = 0;
    char *fileData;
    shader->fileHandle = CreateFile(shader->filePath, GENERIC_READ,
                                          FILE_SHARE_READ|FILE_SHARE_WRITE,
                                          NULL, OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL, NULL);
    GetFileTime(shader->fileHandle, NULL, NULL, &shader->lastWriteTime);
    GetFileSizeEx(shader->fileHandle, &shader->fileSize);

    fileData = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, shader->fileSize.QuadPart);
    assert(fileData);
    ReadFile(shader->fileHandle, fileData, (DWORD)shader->fileSize.QuadPart, &bytesRead, NULL);
    shader->fileLoaded = true;
        
    shader->shader = glCreateShader(shaderType);

    GLchar *shaderCode = (GLchar*)HeapAlloc(GetProcessHeap(),
                                            HEAP_ZERO_MEMORY, shader->fileSize.QuadPart);
    assert(shaderCode);
    CopyMemory((void*)shaderCode, (void*)fileData, shader->fileSize.QuadPart);
    glShaderSource(shader->shader, 1, &shaderCode, NULL);
    glCompileShader(shader->shader);

    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader->shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader->shader, 1024, NULL, infoLog);
        OutputDebugString("Shader Compilation ERROR: \n");
        OutputDebugString(infoLog);
        OutputDebugString("\n");
    }
    else
    {
        //NOTE don't load new shader if it didn't compile!!
        // ONLY SUPPORTS MAX_SHADER_CODE_SIZE bytes large shaders
        ZeroMemory(shader->code, MAX_SHADER_CODE_SIZE);
        CopyMemory(shader->code, fileData, MAX_SHADER_CODE_SIZE);
        shader->isModified = true;
    }

    HeapFree(GetProcessHeap(), 0, fileData);
    HeapFree(GetProcessHeap(), 0, shaderCode);
}

//TODO make code size dynamic, not static
static void hotLoadShaderFromFile(struct shaderData *vertexShader, struct shaderData *fragmentShader, GLuint *shaderProgram)
{
    DWORD bytesRead = 0;
    
    if (!vertexShader->fileLoaded)
    {
        readEntireShaderFromFile(vertexShader, GL_VERTEX_SHADER);
    }
    else
    {        
        FILETIME lastWriteTime = vertexShader->lastWriteTime;
        GetFileTime(vertexShader->fileHandle, NULL, NULL, &vertexShader->lastWriteTime);
        if (CompareFileTime(&lastWriteTime, &vertexShader->lastWriteTime) != 0)
        {
            readEntireShaderFromFile(vertexShader, GL_VERTEX_SHADER);
        }
    }

    if (!fragmentShader->fileLoaded)
    {
        readEntireShaderFromFile(fragmentShader, GL_FRAGMENT_SHADER);
    }
    else
    {
        FILETIME lastWriteTime = fragmentShader->lastWriteTime;
        GetFileTime(fragmentShader->fileHandle, NULL, NULL, &fragmentShader->lastWriteTime);
        if (CompareFileTime(&lastWriteTime, &fragmentShader->lastWriteTime) != 0)
        {
            readEntireShaderFromFile(fragmentShader, GL_FRAGMENT_SHADER);
        }
    }

    if (vertexShader->isModified || fragmentShader->isModified)
    {
        glDeleteProgram(*shaderProgram);

        *shaderProgram = glCreateProgram();
        glAttachShader(*shaderProgram, vertexShader->shader);
        glAttachShader(*shaderProgram, fragmentShader->shader);
        glLinkProgram(*shaderProgram);

        
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(*shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(*shaderProgram, 1024, NULL, infoLog);
            OutputDebugString("Shader Link ERROR: \n");
            OutputDebugString(infoLog);
            OutputDebugString("\n");
        }
        
        glDeleteShader(vertexShader->shader);
        glDeleteShader(fragmentShader->shader);

        vertexShader->isModified = false;
        fragmentShader->isModified = false;
    }
} 

struct quad
{
    GLuint VAO;
};

static void initQuad(struct quad *quad)
{
    GLuint VBO;
    GLfloat vertices[] =
    {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    glGenVertexArrays(1, &quad->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quad->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void drawGameObject(struct gameObject *object, struct quad *quad, GLuint shaderProgram)
{
    
    glUseProgram(shaderProgram);
#if 1
    m4x4 model = identity();
    model = translate(model, {object->position.x, object->position.y, 0.0f});

    model = scale(model, {object->size.x, object->size.y, 1.0f});
    
    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, model.E[0]);

    
    m4x4 projection = orthographicProjection(0.0f, (real32)windowWidth, (real32)windowHeight,
                                             0.0f, -1.0f, 1.0f);
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.E[0]);
    
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform3fv(colorLoc, 1, &object->color.E[0]);
#else

    glm::mat4 model;
    model = glm::translate(model, glm::vec3(object->glmPos, 0.0f));
    model = glm::scale(model, glm::vec3(object->glmSize, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 
                                      static_cast<GLfloat>(windowHeight), 0.0f, -1.0f, 1.0f);
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                    
    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform3fv(colorLoc, 1, glm::value_ptr(object->glmColor));
#endif    
    glBindVertexArray(quad->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void drawLevel(struct gameLevel *level, struct quad *quad, GLuint shaderProgram)
{
    for (int tileIndex = 0; tileIndex < LEVEL_HEIGHT*LEVEL_WIDTH; tileIndex++)
    {
        drawGameObject(&level->tiles[tileIndex], quad, shaderProgram);
    }
}

static void renderGame(struct game *gameState, struct quad *quad, GLuint shaderProgram)
{
    drawLevel(&gameState->level, quad, shaderProgram);
}

static void initGameObject(struct gameObject *object)
{
    object->position = {0.0f, 0.0f};
    object->size = {1.0f, 1.0f};
    object->velocity = {};
    object->color = {1.0f, 1.0f, 0.0f};
    object->rotation = 0.0f;
}

static void initLevel(struct gameLevel *level)
{
    int tileData[LEVEL_HEIGHT][LEVEL_WIDTH] =
    {
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,},
    };

    GLuint height = LEVEL_HEIGHT;
    GLuint width = LEVEL_WIDTH;    
    real32 tileWidth = windowWidth/(real32)width;
    real32 tileHeight = (windowHeight)/(real32)height;

    int tileIndex = 0;
    
    for (int y = 0; y < LEVEL_HEIGHT; y++)
    {
        for (int x = 0; x < LEVEL_WIDTH; x++)
        {
            initGameObject(&level->tiles[tileIndex]);
            if (tileData[y][x] == 1)
            {
                level->tiles[tileIndex].solid = true;
                level->tiles[tileIndex].position = { tileWidth * x, tileHeight * y };
                level->tiles[tileIndex].size = { tileWidth, tileHeight };
                level->tiles[tileIndex].color = { 0.0f, 0.0f, 0.0f };

                glm::vec2 pos(tileWidth * x, tileHeight * y);
                glm::vec2 size(tileWidth, tileHeight);
                glm::vec3 color(0.0f, 0.0f, 0.0f);
                level->tiles[tileIndex].glmPos = pos;
                level->tiles[tileIndex].glmSize = size;
                level->tiles[tileIndex].glmColor = color;

                
                tileIndex++;
            }
            else
            {
                level->tiles[tileIndex].solid = false;
                level->tiles[tileIndex].position = { tileWidth*x, tileHeight*y };
                level->tiles[tileIndex].size = { tileWidth, tileHeight };
                level->tiles[tileIndex].color = { 1.0f, 1.0f, 1.0f };

                
                glm::vec2 pos(tileWidth * x, tileHeight * y);
                glm::vec2 size(tileWidth, tileHeight);
                glm::vec3 color(1.0f, 1.0f, 1.0f);
                level->tiles[tileIndex].glmPos = pos;
                level->tiles[tileIndex].glmSize = size;
                level->tiles[tileIndex].glmColor = color;

                tileIndex++;
            }
        }
    }
}

static void initGame(struct game *gameState)
{
    initGameObject(&gameState->player);
    initLevel(&gameState->level);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {

        GLint polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);

        if (polygonMode == GL_FILL)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCode)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "game", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    glViewport(0, 0, windowWidth, windowHeight);

    struct quad quad;
    initQuad(&quad);
    struct game gameState = {};
    initGame(&gameState);
    
    struct shaderData vertexShader = {};
    struct shaderData fragmentShader = {};

    strcpy(vertexShader.filePath, "vertex.glsl");
    strcpy(fragmentShader.filePath, "fragment.glsl");

    GLuint shaderProgram = 0;
    hotLoadShaderFromFile(&vertexShader, &fragmentShader, &shaderProgram);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // game process input

        // game update
#if 0
        m4x4 transform = identity();

        transform = scale(transform, { 0.5f, 0.5f, 0.0f, });
        transform = rotate(transform, (GLfloat)glfwGetTime() * 50.0f, {0.0f, 0.0f, 1.0f, });

        
        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.E[0]);
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.E[0]);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        glBindVertexArray(quad.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

#elif 0
        m4x4 transform = identity();

        transform = translate(transform, {0.0f, 0.0f, 0.0f});
        transform = scale(transform, { 85.0f, 45.0f, 0.0f, });
//        transform = rotate(transform, (GLfloat)glfwGetTime() * 50.0f, {0.0f, 0.0f, 1.0f, });

        
        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.E[0]);
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.E[0]);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        glBindVertexArray(quad.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

#else        
        // game render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderGame(&gameState, &quad, shaderProgram);
#endif        
        glfwSwapBuffers(window);
    }
    
    return 0;
}
