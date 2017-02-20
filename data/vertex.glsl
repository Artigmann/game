#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 transform;

void main()
{
    gl_Position = projection * transform * vec4(position.xy, 0.0f, 1.0f);
} 