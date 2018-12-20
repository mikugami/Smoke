#pragma once

#include "ShaderProgram.h"
#include "Camera.h"
#include "mesh.h"
#include "CloudMesh.h"
#include "RainMesh.h"
#include "SparkMesh.h"
#include "SmokeMesh.h"

void DrawSimpleTriangle(ShaderProgram &debug_program,
                        Camera &camera,
                        uint32_t width,
                        uint32_t height);

void DrawClouds(ShaderProgram &program,
        Camera &camera,
        CloudMesh *mesh,
        uint32_t width,
        uint32_t height,
        float deltaTime);

void DrawRain(ShaderProgram &program,
    Camera &camera,
    RainMesh *mesh,
    uint32_t width,
    uint32_t height,
    float deltaTime);

void DrawSparks(ShaderProgram &program,
    Camera &camera,
    SparkMesh *mesh,
    uint32_t width,
    uint32_t height,
    float deltaTime);

void DrawMesh(ShaderProgram &program,
        Camera &camera,
        std::unique_ptr<Mesh> &mesh,
        uint32_t width,
        uint32_t height,
        float deltaTime);

void DrawSmoke(ShaderProgram &program,
    Camera &camera,
    SmokeMesh *mesh,
    uint32_t width,
    uint32_t height,
    float deltaTime);