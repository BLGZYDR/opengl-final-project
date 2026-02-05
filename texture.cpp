#include "texture.h"
#include "shader.h" // 需要引用 Shader 类定义
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm> // for std::min, std::max

// 引用在 main.cpp 或 scene.cpp 中定义的全局 Standard Shader
extern Shader* standardShader;

GLuint woodTexture, brickTexture, darkWoodTexture, marbleTexture, dirtTexture, groundTexture, riverTexture, carpetTexture, snowTexture;

// BMP文件结构体 (保持原样)
#pragma pack(push, 1)
struct BMPHeader {
    unsigned char signature[2];
    unsigned int fileSize;
    unsigned int reserved;
    unsigned int dataOffset;
    unsigned int headerSize;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    unsigned int xPixelsPerMeter;
    unsigned int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int importantColors;
};
#pragma pack(pop)

// ==========================================
// 纹理生成函数 (保持原样，省略重复的算法细节，逻辑不变)
// ==========================================

void generateSnowTexture() {
    unsigned char snowData[64][64][3];
    srand(time(NULL));
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 255; int baseG = 255; int baseB = 255;
            int blueTint = 10; baseB = std::min(baseB + blueTint, 255);
            float noise = (sin(i * 0.15) * cos(j * 0.15) + 1.0) * 0.5;
            int variation = noise * 30;
            int spotChance = (i * j * 13) % 100;
            if (spotChance < 15) {
                snowData[i][j][0] = baseR - 40 + variation;
                snowData[i][j][1] = baseG - 40 + variation;
                snowData[i][j][2] = baseB - 20 + variation;
            }
            else if (spotChance > 85 && spotChance < 95) {
                snowData[i][j][0] = std::min(baseR + 20 + variation, 255);
                snowData[i][j][1] = std::min(baseG + 20 + variation, 255);
                snowData[i][j][2] = std::min(baseB + 10 + variation, 255);
            }
            else {
                snowData[i][j][0] = baseR - variation;
                snowData[i][j][1] = baseG - variation;
                snowData[i][j][2] = baseB - variation;
            }
            if (sin(i * 0.2) * cos(j * 0.2) > 0.6) {
                snowData[i][j][0] = std::max(snowData[i][j][0] - 15, 200);
                snowData[i][j][1] = std::max(snowData[i][j][1] - 15, 200);
                snowData[i][j][2] = std::max(snowData[i][j][2] - 10, 210);
            }
            if ((i * j * 7) % 100 < 5) {
                snowData[i][j][0] = 240; snowData[i][j][1] = 245; snowData[i][j][2] = 255;
            }
        }
    }
    glGenTextures(1, &snowTexture);
    glBindTexture(GL_TEXTURE_2D, snowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, snowData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateDirtTexture() {
    unsigned char dirtData[64][64][3];
    int baseR = 101; int baseG = 67; int baseB = 33;
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int variation = (int)(sin(i * 0.5 + j * 0.3) * 15);
            int randomVar = (i * j * 13) % 10 - 5;
            int spotChance = (i * j * 17) % 100;
            if (spotChance < 10) {
                dirtData[i][j][0] = baseR - 30 + variation + randomVar;
                dirtData[i][j][1] = baseG - 20 + variation + randomVar;
                dirtData[i][j][2] = baseB - 10 + variation + randomVar;
            }
            else if (spotChance > 85 && spotChance < 95) {
                dirtData[i][j][0] = baseR + 20 + variation + randomVar;
                dirtData[i][j][1] = baseG + 15 + variation + randomVar;
                dirtData[i][j][2] = baseB + 10 + variation + randomVar;
            }
            else {
                dirtData[i][j][0] = baseR + variation + randomVar;
                dirtData[i][j][1] = baseG + variation + randomVar;
                dirtData[i][j][2] = baseB + variation + randomVar;
            }
            if ((i - 20) * (i - 20) + (j - 20) * (j - 20) < 16) {
                dirtData[i][j][0] -= 10; dirtData[i][j][1] -= 8; dirtData[i][j][2] -= 5;
            }
        }
    }
    glGenTextures(1, &dirtTexture);
    glBindTexture(GL_TEXTURE_2D, dirtTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, dirtData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateWoodTexture() {
    unsigned char woodData[64][64][3];
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 139; int baseG = 90; int baseB = 43;
            int variation = sin(i * 0.3) * 20;
            woodData[i][j][0] = baseR + variation;
            woodData[i][j][1] = baseG + variation;
            woodData[i][j][2] = baseB + variation;
            if ((i - 32) * (i - 32) + (j - 32) * (j - 32) < 25) {
                woodData[i][j][0] = 100; woodData[i][j][1] = 60; woodData[i][j][2] = 30;
            }
        }
    }
    glGenTextures(1, &woodTexture);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, woodData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateBrickTexture() {
    unsigned char brickData[64][64][3];
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int brickR = 138; int brickG = 34; int brickB = 34;
            if (i % 8 == 0 || j % 8 == 0) {
                brickData[i][j][0] = 100; brickData[i][j][1] = 100; brickData[i][j][2] = 100;
            }
            else {
                int variation = rand() % 20;
                brickData[i][j][0] = brickR + variation;
                brickData[i][j][1] = brickG + variation;
                brickData[i][j][2] = brickB + variation;
            }
        }
    }
    glGenTextures(1, &brickTexture);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, brickData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateDarkWoodTexture() {
    unsigned char darkWoodData[64][64][3];
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 73; int baseG = 57; int baseB = 45;
            int variation = sin(i * 0.2 + j * 0.1) * 15;
            darkWoodData[i][j][0] = baseR + variation;
            darkWoodData[i][j][1] = baseG + variation;
            darkWoodData[i][j][2] = baseB + variation;
            if (i % 16 < 2) {
                darkWoodData[i][j][0] = 70; darkWoodData[i][j][1] = 45; darkWoodData[i][j][2] = 20;
            }
        }
    }
    glGenTextures(1, &darkWoodTexture);
    glBindTexture(GL_TEXTURE_2D, darkWoodTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, darkWoodData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateMarbleTexture() {
    unsigned char marbleData[64][64][3];
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 230; int baseG = 230; int baseB = 230;
            float noise = (sin(i * 0.1) * cos(j * 0.1) + 1.0) * 0.5;
            int variation = noise * 40;
            marbleData[i][j][0] = baseR - variation;
            marbleData[i][j][1] = baseG - variation;
            marbleData[i][j][2] = baseB - variation;
            if (abs(sin(i * 0.3) + cos(j * 0.3)) > 1.8) {
                marbleData[i][j][0] = 180; marbleData[i][j][1] = 180; marbleData[i][j][2] = 180;
            }
        }
    }
    glGenTextures(1, &marbleTexture);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, marbleData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateGroundTexture() {
    unsigned char groundData[64][64][3];
    srand(time(NULL));
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 34; int baseG = 69; int baseB = 34;
            int variation = (rand() % 40) - 20;
            groundData[i][j][0] = baseR + variation;
            groundData[i][j][1] = baseG + variation;
            groundData[i][j][2] = baseB + variation;
            if (sin(i * 0.2) * cos(j * 0.2) > 0.7) {
                groundData[i][j][0] -= 20; groundData[i][j][1] -= 20; groundData[i][j][2] -= 10;
            }
        }
    }
    glGenTextures(1, &groundTexture);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, groundData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateRiverTexture() {
    unsigned char riverData[64][64][3];
    srand(time(NULL));
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 25; int baseG = 130; int baseB = 245;
            float flowDirection = 0.7; float flowIntensity = 0.4;
            float flowCoord = i * 0.1 + j * flowDirection * 0.15;
            float wave = sin(flowCoord) * flowIntensity;
            float turbulence = sin(i * 0.3) * cos(j * 0.2) * 0.2;
            float combinedEffect = wave + turbulence;
            int colorVariation = combinedEffect * 40 + 20;
            int finalR = std::min(std::max(baseR + colorVariation, 20), 70);
            int finalG = std::min(std::max(baseG + colorVariation, 100), 180);
            int finalB = std::min(std::max(baseB + colorVariation, 200), 255);
            riverData[i][j][0] = finalR; riverData[i][j][1] = finalG; riverData[i][j][2] = finalB;
            float foamPattern1 = sin(i * 0.5 + j * 0.3);
            float foamPattern2 = cos(i * 0.2 - j * 0.4);
            float foamPattern3 = sin(i * 0.15 + j * 0.25) * cos(j * 0.1);
            float foamValue = (foamPattern1 + foamPattern2 + foamPattern3) / 3.0;
            if (foamValue > 0.65) {
                riverData[i][j][0] = 200; riverData[i][j][1] = 230; riverData[i][j][2] = 255;
            }
            if (fabs(sin(i * 0.25 + j * 0.1)) > 0.8) {
                riverData[i][j][0] = std::max(riverData[i][j][0] - 15, 10);
                riverData[i][j][1] = std::max(riverData[i][j][1] - 20, 80);
                riverData[i][j][2] = std::max(riverData[i][j][2] - 10, 190);
            }
        }
    }
    glGenTextures(1, &riverTexture);
    glBindTexture(GL_TEXTURE_2D, riverTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, riverData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void generateCarpetTexture() {
    unsigned char carpetData[64][64][3];
    srand(time(NULL));
    int redR = 200; int redG = 50; int redB = 50;
    int whiteR = 255; int whiteG = 240; int whiteB = 220;
    int gridSize = 16;
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int gridX = i / gridSize; int gridY = j / gridSize;
            if ((gridX + gridY) % 2 == 0) {
                carpetData[i][j][0] = redR; carpetData[i][j][1] = redG; carpetData[i][j][2] = redB;
                if (sin(i * 0.2) * cos(j * 0.15) > 0.3) {
                    carpetData[i][j][0] = std::min(redR + 20, 255);
                    carpetData[i][j][1] = std::min(redG + 10, 255);
                    carpetData[i][j][2] = std::min(redB + 10, 255);
                }
                if (i % 4 == 0 || j % 4 == 0) {
                    carpetData[i][j][0] = std::max(redR - 30, 0);
                    carpetData[i][j][1] = std::max(redG - 20, 0);
                    carpetData[i][j][2] = std::max(redB - 20, 0);
                }
            }
            else {
                carpetData[i][j][0] = whiteR; carpetData[i][j][1] = whiteG; carpetData[i][j][2] = whiteB;
                float noise = (sin(i * 0.15) * cos(j * 0.1) + 1.0) * 0.5;
                int variation = noise * 25;
                carpetData[i][j][0] = std::min(whiteR - variation, 255);
                carpetData[i][j][1] = std::min(whiteG - variation, 255);
                carpetData[i][j][2] = std::min(whiteB - variation, 255);
                if (i % 4 == 0 || j % 4 == 0) {
                    carpetData[i][j][0] = std::min(whiteR + 20, 255);
                    carpetData[i][j][1] = std::min(whiteG + 20, 255);
                    carpetData[i][j][2] = std::min(whiteB + 20, 255);
                }
            }
            int cellX = i % gridSize; int cellY = j % gridSize;
            if (cellX < 2 || cellY < 2 || cellX > gridSize - 3 || cellY > gridSize - 3) {
                float blendFactor = 0.7;
                if ((gridX + gridY) % 2 == 0) {
                    carpetData[i][j][0] = static_cast<unsigned char>(redR * blendFactor + whiteR * (1.0 - blendFactor));
                    carpetData[i][j][1] = static_cast<unsigned char>(redG * blendFactor + whiteG * (1.0 - blendFactor));
                    carpetData[i][j][2] = static_cast<unsigned char>(redB * blendFactor + whiteB * (1.0 - blendFactor));
                }
                else {
                    carpetData[i][j][0] = static_cast<unsigned char>(whiteR * blendFactor + redR * (1.0 - blendFactor));
                    carpetData[i][j][1] = static_cast<unsigned char>(whiteG * blendFactor + redG * (1.0 - blendFactor));
                    carpetData[i][j][2] = static_cast<unsigned char>(whiteB * blendFactor + redB * (1.0 - blendFactor));
                }
            }
            int randomNoise = (rand() % 6) - 3;
            carpetData[i][j][0] = std::min(std::max(carpetData[i][j][0] + randomNoise, 0), 255);
            carpetData[i][j][1] = std::min(std::max(carpetData[i][j][1] + randomNoise, 0), 255);
            carpetData[i][j][2] = std::min(std::max(carpetData[i][j][2] + randomNoise, 0), 255);
        }
    }
    glGenTextures(1, &carpetTexture);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, carpetData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void loadTextures() {
    srand(time(NULL));
    generateWoodTexture();
    generateBrickTexture();
    generateDarkWoodTexture();
    generateMarbleTexture();
    generateDirtTexture();
    generateGroundTexture();
    generateRiverTexture();
    generateCarpetTexture();
    generateSnowTexture();
}

// =========================================================
// 纹理应用函数 (修改版：支持 Shader Uniform)
// =========================================================

// 禁用纹理
void disableTexture() {
    glDisable(GL_TEXTURE_2D);
    // 【关键修改】通知 Shader 关闭纹理混合
    if (standardShader) standardShader->setFloat("useTexture", 0.0f);
}

// 应用积雪纹理
void applySnowTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, snowTexture);
    glColor3f(1.0, 1.0, 1.0);
    // 【关键修改】通知 Shader 开启纹理混合
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用木材纹理
void applyWoodTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用砖块纹理
void applyBrickTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用深色木材纹理
void applyDarkWoodTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, darkWoodTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用泥土纹理
void applyDirtTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dirtTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用大理石纹理
void applyMarbleTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用地面纹理
void applyGroundTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用河流纹理
void applyRiverTexture() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, riverTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}

// 应用地毯纹理
void applyCarpetTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);
    glColor3f(1.0, 1.0, 1.0);
    if (standardShader) standardShader->setFloat("useTexture", 1.0f);
}
