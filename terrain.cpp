#include "terrain.h"
#include "lighting.h" 
#include "platform.h" 
#include "weather.h" 
#include "model.h"
#include <algorithm>
#include <ctime> 
#include <fstream>
#include <vector>

extern GLuint shadowMap;
extern float lightSpaceMatrixSimple[16];

template<typename T>
T mix(T a, T b, float t) {
    return a * (1.0f - t) + b * t;
}

std::vector<TreeInstance> treeInstances;
TerrainSystem terrainSystem;
int globalSeed = 0;
std::vector<Model*> treeModels;

float TerrainSystem::getHeight(float x, float z) {
    float riverZ;
    extern void generateRiverPath(float x, float& z);
    generateRiverPath(x, riverZ);
    float dRiver = fabs(z - riverZ);

    // 基础地形噪声逻辑 (保持不变)
    float noise = 60.0f * sin(x / 300.0f) * cos(z / 300.0f) +
        20.0f * sin(x / 100.0f + z / 150.0f) +
        5.0f * sin(x / 20.0f) * sin(z / 20.0f);
    float naturalH = noise + 10.0f;

    // 房屋平台逻辑 (保持不变)
    const float hX = 350.0f; const float hZ = -120.0f;
    const float pH = 50.0f; const float fR = 125.0f; const float tR = 350.0f;
    float dist = sqrt(pow(x - hX, 2) + pow(z - hZ, 2));
    float baseH;
    if (dist < fR) baseH = pH;
    else if (dist < tR) {
        float t = (dist - fR) / (tR - fR);
        baseH = mix(naturalH + 15.0f, pH, 1.0f - (t * t * (3.0f - 2.0f * t)));
    }
    else baseH = naturalH + 15.0f;

    // --- 【关键修改：河道与河堤对齐】 ---
    // 假设水宽度为 80 (半径40)，水面 y = -2.0

    if (dRiver < 50.0f) {
        // 1. 拓宽河床底部：半径 50 以内全部下沉，确保水（半径40）完全在坑里
        return -15.0f;
    }
    else if (dRiver < 80.0f) {
        // 2. 陡峭河岸：从 -15.0 急剧上升到河堤顶点 (baseH + 15)
        // 这一段起始于 50，远大于水的半径 40，所以水绝对不会切入坡面
        float t = (dRiver - 50.0f) / 30.0f;
        float smoothT = t * t * (3.0f - 2.0f * t);
        return mix(-15.0f, baseH + 15.0f, smoothT);
    }
    else if (dRiver < 150.0f) {
        // 3. 缓坡回落：从河堤顶点回到正常地面
        float t = (dRiver - 80.0f) / 70.0f;
        float smoothT = t * t * (3.0f - 2.0f * t);
        return mix(baseH + 15.0f, baseH, smoothT);
    }

    return baseH;
}

void TerrainSystem::init() {
    srand(static_cast<unsigned int>(time(0)));
    globalSeed = rand();

    extern const char* terrainVS;
    extern const char* terrainFS;
    terrainShader = new Shader(terrainVS, terrainFS);

    initVBO();

    treeModels.clear();
    // 确保路径正确
    if (std::ifstream("model/AshTree.obj")) treeModels.push_back(new Model("model/AshTree.obj"));
    if (std::ifstream("model/Maple.obj")) treeModels.push_back(new Model("model/Maple.obj"));

    generateVegetationData(0, 0);

    terrainList = glGenLists(1);
    glNewList(terrainList, GL_COMPILE);
    drawRocks();
    glEndList();
}

void TerrainSystem::generateVegetationData(float centerX, float centerZ) {
    treeInstances.clear();
    const float GRID_SIZE = 400.0f;
    const int VIEW_RADIUS = 6;

    int currentGridX = static_cast<int>(floor(centerX / GRID_SIZE));
    int currentGridZ = static_cast<int>(floor(centerZ / GRID_SIZE));

    for (int gx = currentGridX - VIEW_RADIUS; gx <= currentGridX + VIEW_RADIUS; ++gx) {
        for (int gz = currentGridZ - VIEW_RADIUS; gz <= currentGridZ + VIEW_RADIUS; ++gz) {
            unsigned int seed = gx * 7234569 + gz * 9876543 + globalSeed;
            srand(seed);
            int treesPerChunk = 6;

            for (int i = 0; i < treesPerChunk; i++) {
                float localX = static_cast<float>(rand() % (int)GRID_SIZE);
                float localZ = static_cast<float>(rand() % (int)GRID_SIZE);
                float tx = gx * GRID_SIZE + localX;
                float tz = gz * GRID_SIZE + localZ;

                // --- 【修改点 1】显著扩大房屋禁止树木生成的范围 ---
                // X范围从原本的 240~460 扩大到 150~550
                // Z范围从原本的 -220~30 扩大到 -350~150
                if (tx > 150.0f && tx < 550.0f && tz > -350.0f && tz < 150.0f) continue;

                float distToCampfire = sqrt(pow(tx - 350.0f, 2) + pow(tz - 550.0f, 2));
                if (distToCampfire < 150.0f) continue;

                // --- 【修改点 2】扩大河流禁止生成的范围 ---
                float riverZ; extern void generateRiverPath(float x, float& z);
                generateRiverPath(tx, riverZ);
                // 避让距离从 110 增加到 160，确保两岸有足够空地
                if (abs(tz - riverZ) < 160.0f) continue;

                float ty = getHeight(tx, tz);
                if (ty < 5.0f) continue;

                TreeInstance t;
                t.x = tx; t.z = tz;
                t.y = ty; // 直接使用地面高度，依靠 model.cpp 的归一化保证不悬空
                t.rotation = rand() % 360;
                t.scale = 0.6f + (rand() % 8) / 10.0f;

                if (!treeModels.empty()) t.modelIndex = rand() % treeModels.size();
                else t.modelIndex = -1;

                t.isPine = (rand() % 10 < 7);
                treeInstances.push_back(t);
            }
        }
    }
    lastGenX = centerX; lastGenZ = centerZ;
}

// 其余 drawTerrainVBO, drawTrees 等函数保持原样，无需修改
void TerrainSystem::draw(float camX, float camZ) {
    float dist = sqrt(pow(camX - lastGenX, 2) + pow(camZ - lastGenZ, 2));
    if (dist > 400.0f) generateVegetationData(camX, camZ);
    drawTerrainVBO(camX, camZ);
    glCallList(terrainList);
    drawTrees(camX, camZ);
}

void TerrainSystem::initVBO() {
    std::vector<TerrainVertex> vertices;
    float range = 4000.0f; float step = 80.0f;
    for (float x = -range; x < range; x += step) {
        for (float z = -range; z < range; z += step) {
            vertices.push_back({ x, z }); vertices.push_back({ x + step, z }); vertices.push_back({ x, z + step });
            vertices.push_back({ x + step, z }); vertices.push_back({ x + step, z + step }); vertices.push_back({ x, z + step });
        }
    }
    indexCount = static_cast<int>(vertices.size());
    glGenVertexArrays(1, &terrainVAO); glGenBuffers(1, &terrainVBO);
    glBindVertexArray(terrainVAO); glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
    glEnableVertexAttribArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0); glBindVertexArray(0);
}

void TerrainSystem::drawTerrainVBO(float camX, float camZ) {
    terrainShader->use();
    float snow = weatherSystem.getSnowAccumulation() / 8.0f;
    if (snow > 1.0f) snow = 1.0f;
    terrainShader->setFloat("snowFactor", snow);
    terrainShader->setVec3("viewPos", camX, 500.0f, camZ);
    float step = 80.0f;
    float offsetX = floor(camX / step) * step;
    float offsetZ = floor(camZ / step) * step;
    glUniform2f(glGetUniformLocation(terrainShader->Program, "offset"), offsetX, offsetZ);

    if (isShadowPass) {
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0); glActiveTexture(GL_TEXTURE0);
    }
    else {
        const GLfloat* sunPos = lightingSystem.getSunPosition();
        terrainShader->setVec3("lightPos", sunPos[0], sunPos[1], sunPos[2]);
        const GLfloat* sunColor = lightingSystem.getSunColor();
        terrainShader->setVec3("lightColor", sunColor[0], sunColor[1], sunColor[2]);
        const GLfloat* ambientColor = lightingSystem.getGlobalAmbient();
        terrainShader->setVec3("ambientColor", ambientColor[0], ambientColor[1], ambientColor[2]);
        terrainShader->setVec3("skyColorTop", 0.1f, 0.4f, 0.8f);
        terrainShader->setVec3("skyColorBottom", 0.4f, 0.35f, 0.3f);
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, shadowMap);
        glUniform1i(glGetUniformLocation(terrainShader->Program, "shadowMap"), 1);
        glActiveTexture(GL_TEXTURE0);
        glUniformMatrix4fv(glGetUniformLocation(terrainShader->Program, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrixSimple);
    }
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_TRIANGLES, 0, indexCount);
    glBindVertexArray(0);
    glUseProgram(0);
}

void TerrainSystem::drawTrees(float camX, float camZ) {
    float snowFactor = weatherSystem.getSnowAccumulation() / 8.0f;
    if (snowFactor > 1.0f) snowFactor = 1.0f;
    glPushAttrib(GL_ALL_ATTRIB_BITS); glEnable(GL_TEXTURE_2D); glDisable(GL_BLEND); glEnable(GL_ALPHA_TEST); glAlphaFunc(GL_GREATER, 0.5f); glDisable(GL_CULL_FACE); glEnable(GL_DEPTH_TEST);
    float amb = 0.6f + snowFactor * 0.4f; GLfloat mat_ambient[] = { amb, amb, amb, 1.0f }; glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient); glColor3f(1.0f, 1.0f, 1.0f);
    float cullDistance = 5000.0f; float cullDistanceSq = cullDistance * cullDistance;
    for (const auto& t : treeInstances) {
        float dx = camX - t.x; float dz = camZ - t.z; float distSq = dx * dx + dz * dz;
        if (distSq > cullDistanceSq) continue;
        glPushMatrix();
        glTranslatef(t.x, t.y, t.z);
        glRotatef(t.rotation, 0, 1, 0);
        float s = t.scale * 14.0f; glScalef(s, s, s);
        if (t.modelIndex >= 0 && t.modelIndex < treeModels.size()) { treeModels[t.modelIndex]->draw(); }
        glPopMatrix();
    }
    glPopAttrib();
}

void TerrainSystem::drawRocks() {
    srand(67890);
    for (int i = 0; i < 150; i++) {
        float rx = (rand() % 4000) - 2000.0f;
        float rz = (rand() % 4000) - 2000.0f;

        // 1. 【同步修改】房屋周围避让：与 generateVegetationData 保持一致
        // 范围从原本的 250~450 扩大到 150~550，确保地基平台干净
        if (rx > 150.0f && rx < 550.0f && rz > -350.0f && rz < 150.0f) continue;

        // 2. 【核心修改】河流/河谷避让
        float riverZ;
        extern void generateRiverPath(float x, float& z);
        generateRiverPath(rx, riverZ);

        // 之前的 80.0f 会让石头生在斜坡上。
        // 现在河谷影响范围是 150.0f，我们将避让半径设为 160.0f，确保石头只出现在平地上
        if (abs(rz - riverZ) < 160.0f) continue;

        float ry = getHeight(rx, rz);

        // 3. 额外保险：如果高度在河床高度附近（水下），不生成石头
        if (ry < 0.0f) continue;

        glPushMatrix();
        glTranslatef(rx, ry, rz);
        // 让石头大小随机化
        glScalef(1.5f + (rand() % 10) / 10.0f, 1.0f, 1.5f + (rand() % 10) / 10.0f);
        glRotatef(rand() % 360, 0, 1, 0);
        glColor3f(0.5f, 0.5f, 0.55f);
        glutSolidDodecahedron();
        glPopMatrix();
    }
}
bool TerrainSystem::checkTreeCollision(float x, float z, float collisionRadius) { for (const auto& t : treeInstances) { float dx = x - t.x; float dz = z - t.z; float distance = sqrt(dx * dx + dz * dz); float treeRadius = 8.0f * t.scale; if (distance < (treeRadius + collisionRadius)) return true; } return false; }
