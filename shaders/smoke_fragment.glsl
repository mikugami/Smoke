#version 330 core

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

uniform sampler2D smoke_texture;

layout (location = 0) out vec4 fragColor;

void main()
{

    vec4 smoke_color = texture(smoke_texture, vTexCoords);
    fragColor = smoke_color;
}