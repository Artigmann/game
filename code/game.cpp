static void gameUpdateAndRender(struct input *gameInput)
{
    
#define TILE_COUNT_X 16
#define TILE_COUNT_Y 8

    int tileMap[TILE_COUNT_Y][TILE_COUNT_X] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };


    real32 tileRelativeWidth = (real32)windowWidth/TILE_COUNT_X;
    real32 tileRelativeHeight = (real32)windowHeight/TILE_COUNT_Y;

    real32 playerWidth = 0.75f*40.0f;
    real32 playerHeight = 40.0f;
    static real32 playerX = (tileRelativeWidth*TILE_COUNT_X)/2;
    static real32 playerY = (tileRelativeHeight*TILE_COUNT_Y)/2;

    real32 dPlayerY = 0.0f;
    real32 dPlayerX = 0.0f;
    
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

    dPlayerY *= 2.0f;
    dPlayerX *= 2.0f;

    real32 newPlayerX = playerX + dPlayerX;
    real32 newPlayerY = playerY + dPlayerY;

    int32 playerTileX = (int32)newPlayerX/(int32)tileRelativeWidth;
    int32 playerTileY = (int32)newPlayerY/(int32)tileRelativeHeight;

    if (tileMap[playerTileY][playerTileX] == 0)
    {
        playerX = newPlayerX;
        playerY = newPlayerY;
    }
    
    for (int y = 0; y < TILE_COUNT_Y; y++)
    {
        for (int x = 0; x < TILE_COUNT_X; x++)
        {
            v3 color = {1.0f, 1.0f, 1.0f };
            if (tileMap[y][x] == 1)
            {
                color = {0.0f, 0.0f, 0.0f };
            }
            
            v2 position = { x * tileRelativeWidth, y * tileRelativeHeight };
            v2 size = { tileRelativeWidth, tileRelativeHeight };
            real32 rotation = 0.0f;

            GL_drawQuad(quad.VAO, quad.shaderProgram, position, size, rotation, color);
        }
    }


    v2 playerSize = { playerWidth, playerHeight };
    v2 playerPosition = { playerX, playerY };            
    GL_drawQuad(quad.VAO, quad.shaderProgram, playerPosition, playerSize, 0, { 1.0f, 0.0f, 0.0f });
}
