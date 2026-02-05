#pragma once
#include "headers.h"

class TerrainChunk {
public:
    int chunkX, chunkZ; // 区块索引
    float size;         // 物理大小
    int resolution;     // 网格精度

    TerrainChunk(int cx, int cz, float s = 100.0f, int res = 32);
    ~TerrainChunk();
    
    void draw();

private:
    GLuint VAO, VBO, EBO;
    int indexCount;
    void generateMesh();
};
