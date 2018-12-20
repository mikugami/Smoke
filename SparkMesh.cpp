#include "SparkMesh.h"
 
SparkMesh* CreateSparkMesh() {
    vector<float> pos {
        -0.03, 0.0,   0, 1,
         0.00, 0.0,   0, 1,
    };
 
    vector<float> norm {
        0, 0, 1, 1,
        0, 0, 1, 1,
    };
 
    vector<float> texc {
        0, 1,
        0, 0,
    };
 
    vector<uint32_t> ind {
        0, 1
    };
 
    return new SparkMesh(pos, norm, texc, ind, -1, "SparkLine");
}
 
void DrawSpark() {
    static std::unique_ptr<SparkMesh> mesh(CreateSparkMesh());
    mesh->Draw();
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
 
static void ResetParticle(int index)
{
    int base_number = rand() % 6;
 
    // movement options
    sparks[index].velocity = -4.0;
    sparks[index].angle = min_angles[base_number % 3] + ((double)rand() / 
            (double)RAND_MAX) * (max_angles[base_number % 3] - min_angles[base_number % 3]);
 
    sparks[index].y_angle = (-1.0 + ((double)rand() / (double)RAND_MAX) * 2) * Y_RANGE;
    sparks[index].time_to_appear = 0.1 + ((double)rand() / (double)RAND_MAX) * 1.0;
    sparks[index].fade = GetNormalSample() * X_BORDER;
 
 
    // init position options
    sparks[index].position = base_points[base_number];
    if (base_number >= 3) {
        sparks[index].angle *= -1;
    }
    sparks[index].position += float3(
        ((double)rand() / (double)RAND_MAX) * X_SHIFT,
        ((double)rand() / (double)RAND_MAX) * Y_SHIFT,
        ((double)rand() / (double)RAND_MAX) * Z_SHIFT
    );
 
    sparks_offset[index] = mul(rotate_Y_4x4(sparks[index].angle),
            transpose(translate4x4(sparks[index].position)));
}
 
static void InitSparks()
{
    for (int i = 0; i < MAX_SPARKS; ++i) {
        ResetParticle(i);
    }
}
 
void SparkMesh::UpdateParticles(float deltaTime)
{
    for (int i = 0; i < MAX_SPARKS; ++i) {
        if (sparks[i].time_to_appear > 0) {
            sparks[i].time_to_appear -= deltaTime;
            continue;
        } else {
            sparks[i].position.x += sparks[i].velocity * cos(sparks[i].angle) * deltaTime;
            sparks[i].position.z += sparks[i].velocity * sin(sparks[i].angle) * deltaTime;
            sparks[i].position.y += sparks[i].velocity * sin(sparks[i].y_angle) * deltaTime;
        }
 
        if (sparks[i].position.x < sparks[i].fade) {
            ResetParticle(i);
        } else {
            sparks_offset[i] = mul(rotate_Y_4x4(sparks[i].angle),
                    transpose(translate4x4(sparks[i].position)));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboOffset); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, sizeof(sparks_offset), sparks_offset, GL_STREAM_DRAW); GL_CHECK_ERRORS;
}
 
SparkMesh::SparkMesh(const vector<float>    &positions,
           const vector<float>    &normals  ,
           const vector<float>    &texcoords,
           const vector<uint32_t> &indices  ,
           size_t mat_id,
           string n) {

    InitSparks(); GL_CHECK_ERRORS;
 
 
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(sparks_offset), sparks_offset, GL_STREAM_DRAW);
 
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
 
string SparkMesh::GetName() {
    return name;
}
 
void SparkMesh::Draw() {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElements(GL_LINES, ind_num, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}
 
void SparkMesh::DrawInstanced(GLsizei prim_count) {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElementsInstanced(GL_LINES, ind_num, GL_UNSIGNED_INT, nullptr, prim_count); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}
 
SparkMesh::~SparkMesh() {
    glDeleteVertexArrays(1, &vao);
}