#version 330 core

in float vColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{   
    float color = vColor + 0.4;
    vec4 result = vec4(color, color, color, 0.8);
    fragColor = result;
    brightColor = vec4(0.0);
}