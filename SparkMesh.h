#pragma once
 
#include "common.h"
#include "LiteMath.h"
 
#include <vector>
#include <string>
 
using LiteMath::float4x4;
using LiteMath::float3;
using LiteMath::float4;
using LiteMath::rotate_Y_4x4;
using LiteMath::mul;
 
using std::vector;
using std::string;
 
 
class SparkMesh {
public:
    float4x4 model;
    uint32_t material_id;
 
    SparkMesh(const vector<float>   &positions,
             const vector<float>    &normals  ,
             const vector<float>    &texcoords,
             const vector<uint32_t> &indices  ,
             size_t mat_id,
             string n);
 
    string GetName();
 
    void Draw();
 
    void DrawInstanced(GLsizei prim_count);
 
    void UpdateParticles(float deltaTime);
 
    ~SparkMesh();
 
protected:
    string name;
    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords, vboOffset, vao;
    size_t ind_num;
};
 
void DrawSpark();
SparkMesh* CreateSparkMesh();
 
 
// rain particles
static void ResetParticle(int index);
static void InitSparks();
 
 
typedef struct Spark {
    // Position/direction
    float3 position;
 
    float time_to_appear;
    float angle;    /* 15 - 45 */
    float y_angle;
    float velocity;
    float fade;
} Spark;
 
 
const uint64_t MAX_SPARKS = 500;
 
static float X_BORDER = -0.3;
 
static float X_SHIFT = 0.04;
static float Y_SHIFT = 0.03;
static float Z_SHIFT = 0.04;
 
static float Y_RANGE = 0.2;
 
static float3 base_points[6] = {
    float3(1.1, 0.35, -0.55),
    float3(1.5, 0.35, -0.55),
    float3(1.9, 0.35, -0.47),
    float3(1.1, 0.35,  0.50),
    float3(1.5, 0.35,  0.50),
    float3(1.9, 0.35,  0.45)
};
 
static double min_angles[3] = {
    0.1,
    0.2,
    0.3
};
static double max_angles[3] = {
    0.2,
    0.3,
    0.5
};
 
static Spark sparks[MAX_SPARKS];
static float4x4 sparks_offset[MAX_SPARKS];