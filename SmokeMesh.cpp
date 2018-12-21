#include "SmokeMesh.h"

typedef struct SmokeParticle {
    // Position/direction
    float3 position;
 
    float time_to_appear;
    float angle;    /* 15 - 45 */
    float y_angle;
    float velocity;
    float fade;
    float dist_to_cam;
} Smoke;
  
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
    0.1,
    0.1
};

static double max_angles[3] = {
    0.2,
    0.2,
    0.2
};
 
static Smoke smoke_particles[MAX_SMOKE_PARTICLES];
static float4x4 smoke_offset[MAX_SMOKE_PARTICLES];

SmokeMesh* CreateSmokeMesh() {
    vector<float> pos {
         0.03,  0.03,  0.0, 1,
         0.03, -0.03,  0.0, 1,
        -0.03, -0.03,  0.0, 1,
        -0.03,  0.03,  0.0, 1
    };

    vector<float> norm {
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1
    };

    vector<float> texc {
        1, 1,
        0, 1,
        0, 0,
        1, 0 
    };

    vector<uint32_t> ind {
        0, 1, 3,
        1, 2, 3
    };

    return new SmokeMesh(pos, norm, texc, ind, -1, "SmokeParticle");
}

static double GetNormalSample()
{
    double u = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double v = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1) {
        return GetNormalSample();
    }
    double c = sqrt(-2 * log(r) / r);
    return u * c;
}
 
static void ResetSmokeParticle(int index)
{
    int base_number = rand() % 6;
 
    // movement options
    smoke_particles[index].velocity = -1.5;
    smoke_particles[index].angle = min_angles[base_number % 3] + ((double)rand() / 
            (double)RAND_MAX) * (max_angles[base_number % 3] - min_angles[base_number % 3]);

    smoke_particles[index].y_angle = (-1.0 + ((double)rand() / (double)RAND_MAX) * 2) * Y_RANGE;
    smoke_particles[index].time_to_appear = 0.1 + ((double)rand() / (double)RAND_MAX) * 1.0;
    smoke_particles[index].fade = GetNormalSample() * X_BORDER;
 
 
    // init position options
    smoke_particles[index].position = base_points[base_number];
    if (base_number >= 3) {
        smoke_particles[index].angle *= -1;
    }
    smoke_particles[index].position += float3(
        ((double)rand() / (double)RAND_MAX) * X_SHIFT,
        ((double)rand() / (double)RAND_MAX) * Y_SHIFT,
        ((double)rand() / (double)RAND_MAX) * Z_SHIFT
    );
 
    smoke_offset[index] = mul(rotate_Y_4x4(smoke_particles[index].angle),
            transpose(translate4x4(smoke_particles[index].position)));
}
 
static void InitSmoke()
{
    for (int i = 0; i < MAX_SMOKE_PARTICLES; ++i) {
        ResetSmokeParticle(i);
    }
}

int cam_dist_compare(const void *a, const void *b)
{
    return ((Smoke *)b)->dist_to_cam > ((Smoke *)a)->dist_to_cam - ((Smoke *)b)->dist_to_cam < ((Smoke *)a)->dist_to_cam;
}
 
void SmokeMesh::UpdateParticles(float deltaTime, Camera &camera)
{
    /*
    map<float, Smoke> sorted;
    for (int i = 0; i < MAX_SMOKE_PARTICLES; ++i) {
        if (smoke_particles[i].time_to_appear > 0) {
            smoke_particles[i].time_to_appear -= deltaTime;
            continue;
        } else {
            smoke_particles[i].position.x += smoke_particles[i].velocity * cos(smoke_particles[i].angle) * deltaTime;
            smoke_particles[i].position.z += smoke_particles[i].velocity * sin(smoke_particles[i].angle) * deltaTime;
            smoke_particles[i].position.y += smoke_particles[i].velocity * sin(smoke_particles[i].y_angle) * deltaTime;
        }
        float distance = length(camera.pos - smoke_particles[i].position);
        sorted[distance] = smoke_particles[i];
    }
    int i = 0;
    for (map<float, Smoke>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it, ++i) {
        smoke_particles[i] = it->second;
        if ((it->second).position.x < (it->second).fade) {
            ResetSmokeParticle(i);
        } else {
            smoke_offset[i] = mul(rotate_Y_4x4((it->second).angle),
                    transpose(translate4x4((it->second).position)));
        }
    }
    */
    for (int i = 0; i < MAX_SMOKE_PARTICLES; ++i) {
        if (smoke_particles[i].time_to_appear > 0) {
            smoke_particles[i].time_to_appear -= deltaTime;
            continue;
        } else {
            smoke_particles[i].position.x += smoke_particles[i].velocity * cos(smoke_particles[i].angle) * deltaTime;
            smoke_particles[i].position.z += smoke_particles[i].velocity * sin(smoke_particles[i].angle) * deltaTime;
            smoke_particles[i].position.y += smoke_particles[i].velocity * sin(smoke_particles[i].y_angle) * deltaTime;
        }
        //smoke_particles[i].dist_to_cam = length(camera.pos - smoke_particles[i].position);
        smoke_particles[i].dist_to_cam = (camera.pos - smoke_particles[i].position).x * (camera.pos - smoke_particles[i].position).x +
                (camera.pos - smoke_particles[i].position).y * (camera.pos - smoke_particles[i].position).y +
                (camera.pos - smoke_particles[i].position).z * (camera.pos - smoke_particles[i].position).z;
    }
    qsort(smoke_particles, MAX_SMOKE_PARTICLES, sizeof(Smoke), cam_dist_compare);
    for (int i = 0; i < MAX_SMOKE_PARTICLES; ++i) {
        if (smoke_particles[i].position.x < smoke_particles[i].fade) {
            ResetSmokeParticle(i);
        } else {
            smoke_offset[i] = mul(rotate_Y_4x4(smoke_particles[i].angle),
                    transpose(translate4x4(smoke_particles[i].position)));
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboOffset); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, sizeof(smoke_offset), smoke_offset, GL_STREAM_DRAW); GL_CHECK_ERRORS;
}
 
SmokeMesh::SmokeMesh(const vector<float>    &positions,
           const vector<float>    &normals  ,
           const vector<float>    &texcoords,
           const vector<uint32_t> &indices  ,
           size_t mat_id,
           string n) {

    InitSmoke(); GL_CHECK_ERRORS;
 
 
    name = n;
 
    ind_num = indices.size();
 
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboVertices);
    glGenBuffers(1, &vboIndices);
    glGenBuffers(1, &vboNormals);
    glGenBuffers(1, &vboTexCoords);
    glGenBuffers(1, &vboOffset);
 
 
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    {
        //передаем в шейдерную программу атрибут координат вершин
        glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GL_FLOAT), positions.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(0); GL_CHECK_ERRORS;
 
        //передаем в шейдерную программу атрибут нормалей
        glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GL_FLOAT), normals.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(1); GL_CHECK_ERRORS;
 
        //передаем в шейдерную программу атрибут текстурных координат
        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GL_FLOAT), texcoords.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(2); GL_CHECK_ERRORS;
 
        glBindBuffer(GL_ARRAY_BUFFER, vboOffset);
        glBufferData(GL_ARRAY_BUFFER, sizeof(smoke_offset), smoke_offset, GL_STREAM_DRAW);
 
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(0)); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(3); GL_CHECK_ERRORS;
 
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(sizeof(float4)));
        glEnableVertexAttribArray(4);
 
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(2 * sizeof(float4)));
        glEnableVertexAttribArray(5);
 
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(3 * sizeof(float4)));
        glEnableVertexAttribArray(6);
 
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1); 
 
        //передаем в шейдерную программу индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0); GL_CHECK_ERRORS;
 
    glBindVertexArray(0); GL_CHECK_ERRORS;
 
    material_id = mat_id;
}
 
string SmokeMesh::GetName() {
    return name;
}
 
void SmokeMesh::Draw() {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElements(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}
 
void SmokeMesh::DrawInstanced(GLsizei prim_count) {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElementsInstanced(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr, prim_count); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}
 
SmokeMesh::~SmokeMesh() {
    glDeleteVertexArrays(1, &vao);
}