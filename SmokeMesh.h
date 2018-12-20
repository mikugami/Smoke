#pragma once
 
#include "common.h"
#include "LiteMath.h"
#include "Camera.h"
 
#include <vector>
#include <string>
#include <map>
 
using LiteMath::float4x4;
using LiteMath::float3;
using LiteMath::float4;
using LiteMath::rotate_Y_4x4;
using LiteMath::mul;
 
using std::vector;
using std::string;
using std::map;
 
 
class SmokeMesh {
public:
    float4x4 model;
    uint32_t material_id;
 
    SmokeMesh(const vector<float>   &positions,
             const vector<float>    &normals  ,
             const vector<float>    &texcoords,
             const vector<uint32_t> &indices  ,
             size_t mat_id,
             string n);
 
    string GetName();
 
    void Draw();
 
    void DrawInstanced(GLsizei prim_count);
 
    void UpdateParticles(float deltaTime, Camera &camera);
 
    ~SmokeMesh();
 
protected:
    string name;
    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords, vboOffset, vao;
    size_t ind_num;
};
 
SmokeMesh* CreateSmokeMesh();
 
static void ResetSmokeParticle(int index);
static void InitSmoke();

const uint64_t MAX_SMOKE_PARTICLES = 10000;
