#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 model;

uniform mat4 projection;
uniform mat4 view;

out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragPos;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0f);
    vNormal = mat3(transpose(inverse(model))) * normal;
    vFragPos = vec3(model * vec4(vertex, 1.0f));
    vTexCoords = texCoords;
}