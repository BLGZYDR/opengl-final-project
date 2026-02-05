#include "chunk.h"
#include "terrain.h" // 需要用到 getHeight

TerrainChunk::TerrainChunk(int cx, int cz, float s, int res)
    : chunkX(cx), chunkZ(cz), size(s), resolution(res), VAO(0), VBO(0), EBO(0) {
    generateMesh();
}

TerrainChunk::~TerrainChunk() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

// 顶点结构体
struct Vertex {
    float x, y, z;    // 位置
    float nx, ny, nz; // 法线
};

void TerrainChunk::generateMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float startX = chunkX * size;
    float startZ = chunkZ * size;
    float step = size / resolution;

    // 1. 生成顶点
    for (int z = 0; z <= resolution; ++z) {
        for (int x = 0; x <= resolution; ++x) {
            float posX = startX + x * step;
            float posZ = startZ + z * step;
            float posY = terrainSystem.getHeight(posX, posZ);

            // 计算法线 (简单差分法)
            float hL = terrainSystem.getHeight(posX - 1, posZ);
            float hR = terrainSystem.getHeight(posX + 1, posZ);
            float hD = terrainSystem.getHeight(posX, posZ - 1);
            float hU = terrainSystem.getHeight(posX, posZ + 1);
            float nx = hL - hR;
            float ny = 2.0f;
            float nz = hD - hU;
            // 归一化法线
            float len = sqrt(nx * nx + ny * ny + nz * nz);
            nx /= len; ny /= len; nz /= len;

            vertices.push_back({ posX, posY, posZ, nx, ny, nz });
        }
    }

    // 2. 生成索引 (三角形)
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            int topLeft = z * (resolution + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (resolution + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    indexCount = indices.size();

    // 3. 配置 OpenGL 缓冲 (VAO/VBO/EBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 顶点位置 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // 法线 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TerrainChunk::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
