struct quad
{
    GLuint shaderProgram;
    GLuint VAO;
};


static void GL_drawQuad(GLuint quadVAO, GLuint shaderProgram,
                        v2 position, v2 size, real32 rotation,
                        v3 color)
{
    
    glUseProgram(shaderProgram);

    m4x4 model = identity();
    model = translate(model, {position.x, position.y, 0.0f});

    model = scale(model, {size.x, size.y, 1.0f});
    
    GLuint transformLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, model.E[0]);
   
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "quadColor");
    glUniform3fv(colorLoc, 1, &color.E[0]);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void GL_setShaderProgramProjectionMatrix(GLuint shaderProgram, m4x4 projection)
{
    glUseProgram(shaderProgram);
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.E[0]);
}

static void initQuadVAO(struct quad *quad)
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
