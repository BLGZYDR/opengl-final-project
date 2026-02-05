#ifndef TERRAIN_H
#define TERRAIN_H

#include "headers.h"
#include "shader.h"
#include "model.h" 
#include <vector>
#include <cmath>

struct TreeInstance {
    float x, y, z;
    float scale;
    float rotation;
    bool isPine;
    int modelIndex;
};

struct TerrainVertex {
    float x, z;
};

class TerrainSystem {
public:
    void init();
    void draw(float camX, float camZ);
    float getHeight(float x, float z);
    bool checkTreeCollision(float x, float z, float collisionRadius);

private:
    GLuint terrainVAO, terrainVBO;
    Shader* terrainShader;
    int indexCount;

    std::vector<TreeInstance> treeInstances;
    std::vector<Model*> treeModels;

    float lastGenX = 0.0f;
    float lastGenZ = 0.0f;

    void initVBO();
    void drawTerrainMesh(float camX, float camZ);
    void drawTerrainVBO(float camX, float camZ);

    void generateVegetationData(float centerX, float centerZ);

    // 【修改】增加参数，用于距离剔除
    void drawTrees(float camX, float camZ);

    void drawRocks();
    void drawPineTree(float snowFactor);
    void drawRoundTree(float snowFactor);

    GLuint terrainList;
    void setNormalForPoint(float x, float z);
};

extern TerrainSystem terrainSystem;

#endif
