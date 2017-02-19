#version 330 core
layout (location = 0) in vec3 position;

out vec4 ourColor;

uniform mat4 projection;
uniform mat4 transform;

void main()
{
    gl_Position = projection * transform * vec4(position, 1.0f);
    ourColor = vec4(0.5f, 0.3f, 0.2f, 1.0f);
} 