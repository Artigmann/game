#include <windows.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

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
        0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 
    };

    glGenVertexArrays(1, &quad->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quad->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
//    glfwSetKeyCallback(window, keyCallback);

    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    glViewport(0, 0, windowWidth, windowHeight);

    struct quad quad;
    initQuad(&quad);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    struct shaderData vertexShader = {};
    struct shaderData fragmentShader = {};

    strcpy(vertexShader.filePath, "vertex.glsl");
    strcpy(fragmentShader.filePath, "fragment.glsl");

    GLuint shaderProgram = 0;
    hotLoadShaderFromFile(&vertexShader, &fragmentShader, &shaderProgram);

    m4x4 projection = orthographicProjection((real32)windowWidth/(real32)windowHeight);
    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // game process input

        // game update

        m4x4 transform = identity();
        transform = scale(transform, { 1.0f, 1.0f, 1.0f, });
        transform = rotate(transform, (GLfloat)glfwGetTime() * 50.0f, {0.0f, 0.0f, 1.0f, });

        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.E[0]);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.E[0]);
        
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        glBindVertexArray(quad.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        // game render

        glfwSwapBuffers(window);
    }
    
    return 0;
}
