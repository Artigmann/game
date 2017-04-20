#include "game.h"
#include <assert.h>

static void gameUpdateAndRender(struct input *gameInput)
{
    
#define TILE_MAP_COUNT_X 16
#define TILE_MAP_COUNT_Y 8

    uint32 tileMap[TILE_MAP_COUNT_Y][TILE_MAP_COUNT_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };

    real32 tileWidth = (real32)windowWidth/TILE_MAP_COUNT_X;
    real32 tileHeight = (real32)windowHeight/TILE_MAP_COUNT_Y;
    
    real32 dPlayerX = 0.0f;
    real32 dPlayerY = 0.0f;

    static real32 playerX = 60.0f;
    static real32 playerY = 30.0f;    

    real32 playerHeight = 0.8f*tileHeight;
    real32 playerWidth = 0.75f*playerHeight;
    
    if (gameInput->keys[GLFW_KEY_W])
    {
        dPlayerY -= 1.0f;
    }
    if (gameInput->keys[GLFW_KEY_S])
    {
        dPlayerY += 1.0f;
    }
    if (gameInput->keys[GLFW_KEY_A])
    {
        dPlayerX -= 1.0f;
    }
    if (gameInput->keys[GLFW_KEY_D])
    {
        dPlayerX += 1.0f;
    }

    dPlayerY *= 200.0f;
    dPlayerX *= 200.0f;

    playerX += gameInput->deltaTime*dPlayerX;
    playerY += gameInput->deltaTime*dPlayerY;

#if 0    
    playerX = dPlayerX;
    playerY = dPlayerY;
#endif
    
    for (int32 y = 0; y < TILE_MAP_COUNT_Y; y++)
    {
        for (int32 x = 0; x < TILE_MAP_COUNT_X; x++)
        {
            v3 color = {0.78f, 0.78f, 0.78f};

            if (tileMap[y][x] == 1)
            {
                color = {0.18f, 0.18f, 0.18f};
            }

            real32 tileX = x*tileWidth;
            real32 tileY = y*tileHeight;

            v2 tilePosition = {tileX, tileY};
            v2 tileSize = {tileWidth, tileHeight};
            real32 rotation = 0.0f;
            
            GL_drawQuad(quad.VAO, quad.shaderProgram, tilePosition, tileSize, rotation, color);
        }
    }

    v3 color = {0.84f, 0.85f, 0.33f};
    v2 playerPos = {playerX, playerY};
    v2 playerSize = {playerWidth, playerHeight};

    GL_drawQuad(quad.VAO, quad.shaderProgram, playerPos, playerSize, 0.0f, color);
    
}
