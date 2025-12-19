#include "texture.h"
#include <cmath>
#include<iostream>
#include <cstdlib>
#include <ctime>

GLuint woodTexture, brickTexture, darkWoodTexture, marbleTexture, dirtTexture, groundTexture, riverTexture, carpetTexture;

// BMP文件结构体
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

// 生成泥土纹理
void generateDirtTexture() {
    unsigned char dirtData[64][64][3];
    int baseR = 101;
    int baseG = 67;
    int baseB = 33;

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
                dirtData[i][j][0] -= 10;
                dirtData[i][j][1] -= 8;
                dirtData[i][j][2] -= 5;
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

// 生成木材纹理
void generateWoodTexture() {
    unsigned char woodData[64][64][3];

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 139;
            int baseG = 90;
            int baseB = 43;
            int variation = sin(i * 0.3) * 20;
            woodData[i][j][0] = baseR + variation;
            woodData[i][j][1] = baseG + variation;
            woodData[i][j][2] = baseB + variation;
            if ((i - 32) * (i - 32) + (j - 32) * (j - 32) < 25) {
                woodData[i][j][0] = 100;
                woodData[i][j][1] = 60;
                woodData[i][j][2] = 30;
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

// 生成砖块纹理
void generateBrickTexture() {
    unsigned char brickData[64][64][3];
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int brickR = 138;
            int brickG = 34;
            int brickB = 34;
            if (i % 8 == 0 || j % 8 == 0) {
                brickData[i][j][0] = 100;
                brickData[i][j][1] = 100;
                brickData[i][j][2] = 100;
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

// 生成深色木材纹理
void generateDarkWoodTexture() {
    unsigned char darkWoodData[64][64][3];

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // 深木色基色
            int baseR = 73;
            int baseG = 57;
            int baseB = 45;

            // 木纹变化
            int variation = sin(i * 0.2 + j * 0.1) * 15;
            darkWoodData[i][j][0] = baseR + variation;
            darkWoodData[i][j][1] = baseG + variation;
            darkWoodData[i][j][2] = baseB + variation;

            // 创建深色条纹
            if (i % 16 < 2) {
                darkWoodData[i][j][0] = 70;
                darkWoodData[i][j][1] = 45;
                darkWoodData[i][j][2] = 20;
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

// 生成大理石纹理
void generateMarbleTexture() {
    unsigned char marbleData[64][64][3];

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            int baseR = 230;
            int baseG = 230;
            int baseB = 230;

            float noise = (sin(i * 0.1) * cos(j * 0.1) + 1.0) * 0.5;
            int variation = noise * 40;

            marbleData[i][j][0] = baseR - variation;
            marbleData[i][j][1] = baseG - variation;
            marbleData[i][j][2] = baseB - variation;

            if (abs(sin(i * 0.3) + cos(j * 0.3)) > 1.8) {
                marbleData[i][j][0] = 180;
                marbleData[i][j][1] = 180;
                marbleData[i][j][2] = 180;
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

// 生成森林地面纹理
void generateGroundTexture() {
    unsigned char groundData[64][64][3];

    srand(time(NULL));

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // 基础森林绿色
            int baseR = 34;
            int baseG = 69;
            int baseB = 34;

            // 添加随机变化模拟自然地面
            int variation = (rand() % 40) - 20;
            groundData[i][j][0] = baseR + variation;
            groundData[i][j][1] = baseG + variation;
            groundData[i][j][2] = baseB + variation;

            // 添加深色区域模拟阴影
            if (sin(i * 0.2) * cos(j * 0.2) > 0.7) {
                groundData[i][j][0] -= 20;
                groundData[i][j][1] -= 20;
                groundData[i][j][2] -= 10;
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

// 生成河流纹理（带水流效果）
void generateRiverTexture() {
    unsigned char riverData[64][64][3];  // RGB格式

    srand(time(NULL));

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // 基础水颜色 - 使用蓝绿色调
            int baseR = 25;
            int baseG = 130;
            int baseB = 245;

            // 创建有方向性的水流效果
            // 水流方向（沿对角线方向）
            float flowDirection = 0.7;  // 控制流向
            float flowIntensity = 0.4;  // 控制强度

            // 计算沿水流方向的波浪
            float flowCoord = i * 0.1 + j * flowDirection * 0.15;
            float wave = sin(flowCoord) * flowIntensity;

            // 添加一些湍流/漩涡效果
            float turbulence = sin(i * 0.3) * cos(j * 0.2) * 0.2;

            // 综合效果
            float combinedEffect = wave + turbulence;

            // 颜色变化 - 根据水流强度调整
            int colorVariation = combinedEffect * 40 + 20;

            // 计算最终颜色 - 限制在合理范围内
            int finalR = std::min(std::max(baseR + colorVariation, 20), 70);
            int finalG = std::min(std::max(baseG + colorVariation, 100), 180);
            int finalB = std::min(std::max(baseB + colorVariation, 200), 255);

            riverData[i][j][0] = finalR;
            riverData[i][j][1] = finalG;
            riverData[i][j][2] = finalB;

            // 创建白色水花/泡沫效果 - 模拟水流撞击
            // 使用多个频率的正弦波来创建更自然的图案
            float foamPattern1 = sin(i * 0.5 + j * 0.3);
            float foamPattern2 = cos(i * 0.2 - j * 0.4);
            float foamPattern3 = sin(i * 0.15 + j * 0.25) * cos(j * 0.1);

            float foamValue = (foamPattern1 + foamPattern2 + foamPattern3) / 3.0;

            if (foamValue > 0.65) {
                // 水花/泡沫颜色 - 带点蓝色的白色
                riverData[i][j][0] = 200;
                riverData[i][j][1] = 230;
                riverData[i][j][2] = 255;
            }

            // 添加一些深色条纹模拟水流中的阴影
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
// 生成红白相间地毯纹理
void generateCarpetTexture() {
    unsigned char carpetData[64][64][3]; // 64x64 RGB纹理

    srand(time(NULL));

    // 红白相间暖色调参数
    int redR = 200;    // 暖红色
    int redG = 50;
    int redB = 50;

    int whiteR = 255;  // 暖白色（略带米色）
    int whiteG = 240;
    int whiteB = 220;

    // 定义网格大小 - 红白相间的格子
    int gridSize = 16; // 每个格子的像素大小

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // 计算在网格中的位置
            int gridX = i / gridSize;
            int gridY = j / gridSize;

            // 创建红白相间的棋盘格图案
            if ((gridX + gridY) % 2 == 0) {
                // 红色格子
                carpetData[i][j][0] = redR;
                carpetData[i][j][1] = redG;
                carpetData[i][j][2] = redB;

                // 在红色格子中添加一些纹理变化
                if (sin(i * 0.2) * cos(j * 0.15) > 0.3) {
                    carpetData[i][j][0] = std::min(redR + 20, 255);
                    carpetData[i][j][1] = std::min(redG + 10, 255);
                    carpetData[i][j][2] = std::min(redB + 10, 255);
                }

                // 添加深红色线条模拟织物质感
                if (i % 4 == 0 || j % 4 == 0) {
                    carpetData[i][j][0] = std::max(redR - 30, 0);
                    carpetData[i][j][1] = std::max(redG - 20, 0);
                    carpetData[i][j][2] = std::max(redB - 20, 0);
                }
            }
            else {
                // 白色格子
                carpetData[i][j][0] = whiteR;
                carpetData[i][j][1] = whiteG;
                carpetData[i][j][2] = whiteB;

                // 在白色格子中添加暖色调纹理
                float noise = (sin(i * 0.15) * cos(j * 0.1) + 1.0) * 0.5;
                int variation = noise * 25;

                carpetData[i][j][0] = std::min(whiteR - variation, 255);
                carpetData[i][j][1] = std::min(whiteG - variation, 255);
                carpetData[i][j][2] = std::min(whiteB - variation, 255);

                // 添加浅色线条模拟织物质感
                if (i % 4 == 0 || j % 4 == 0) {
                    carpetData[i][j][0] = std::min(whiteR + 20, 255);
                    carpetData[i][j][1] = std::min(whiteG + 20, 255);
                    carpetData[i][j][2] = std::min(whiteB + 20, 255);
                }
            }

            // 在格子边界添加柔和的过渡
            int cellX = i % gridSize;
            int cellY = j % gridSize;

            if (cellX < 2 || cellY < 2 || cellX > gridSize - 3 || cellY > gridSize - 3) {
                // 边界像素 - 创建柔和的混合效果
                float blendFactor = 0.7;
                if ((gridX + gridY) % 2 == 0) {
                    // 从红色向白色过渡
                    carpetData[i][j][0] = static_cast<unsigned char>(
                        redR * blendFactor + whiteR * (1.0 - blendFactor));
                    carpetData[i][j][1] = static_cast<unsigned char>(
                        redG * blendFactor + whiteG * (1.0 - blendFactor));
                    carpetData[i][j][2] = static_cast<unsigned char>(
                        redB * blendFactor + whiteB * (1.0 - blendFactor));
                }
                else {
                    // 从白色向红色过渡
                    carpetData[i][j][0] = static_cast<unsigned char>(
                        whiteR * blendFactor + redR * (1.0 - blendFactor));
                    carpetData[i][j][1] = static_cast<unsigned char>(
                        whiteG * blendFactor + redG * (1.0 - blendFactor));
                    carpetData[i][j][2] = static_cast<unsigned char>(
                        whiteB * blendFactor + redB * (1.0 - blendFactor));
                }
            }

            // 添加随机噪点模拟地毯的绒毛质感
            int randomNoise = (rand() % 6) - 3; // -3 到 +3 的随机变化
            carpetData[i][j][0] = std::min(std::max(carpetData[i][j][0] + randomNoise, 0), 255);
            carpetData[i][j][1] = std::min(std::max(carpetData[i][j][1] + randomNoise, 0), 255);
            carpetData[i][j][2] = std::min(std::max(carpetData[i][j][2] + randomNoise, 0), 255);
        }
    }

    // 生成OpenGL纹理
    glGenTextures(1, &carpetTexture);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, carpetData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
// 加载所有纹理
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
}

// 应用木材纹理
void applyWoodTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glColor3f(1.0, 1.0, 1.0);
}

// 应用砖块纹理
void applyBrickTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glColor3f(1.0, 1.0, 1.0);
}

// 应用深色木材纹理
void applyDarkWoodTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, darkWoodTexture);
    glColor3f(1.0, 1.0, 1.0);
}

// 应用泥土纹理
void applyDirtTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dirtTexture);
    glColor3f(1.0, 1.0, 1.0);
}

// 应用大理石纹理
void applyMarbleTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glColor3f(1.0, 1.0, 1.0);
}

// 应用地面纹理
void applyGroundTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glColor3f(1.0, 1.0, 1.0);
}

// 应用河流纹理
void applyRiverTexture() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, riverTexture);
    glColor3f(1.0, 1.0, 1.0);
}
// 应用地毯纹理
void applyCarpetTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);
    glColor3f(1.0, 1.0, 1.0);
}
// 禁用纹理
void disableTexture() {
    glDisable(GL_TEXTURE_2D);
}