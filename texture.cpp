#include "texture.h"
#include <cmath>
#include "headers.h"
#include <cstdlib>
#include <ctime>

GLuint woodTexture, brickTexture, darkWoodTexture, marbleTexture, dirtTexture;

// BMP文件头结构
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
// 生成木板纹理
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

// 生成砖墙纹理
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

// 生成深色木头纹理
void generateDarkWoodTexture() {
    unsigned char darkWoodData[64][64][3];

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // 深色木纹
            int baseR = 73;
            int baseG = 57;
            int baseB = 45;

            // 木纹效果
            int variation = sin(i * 0.2 + j * 0.1) * 15;
            darkWoodData[i][j][0] = baseR + variation;
            darkWoodData[i][j][1] = baseG + variation;
            darkWoodData[i][j][2] = baseB + variation;

            // 添加深色条纹
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

void loadTextures() {
    srand(time(NULL));
    generateWoodTexture();
    generateBrickTexture();
    generateDarkWoodTexture();
    generateMarbleTexture();
    generateDirtTexture();
}


void applyWoodTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glColor3f(1.0, 1.0, 1.0);
}

void applyBrickTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glColor3f(1.0, 1.0, 1.0);
}

void applyDarkWoodTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, darkWoodTexture);
    glColor3f(1.0, 1.0, 1.0);
}
void applyDirtTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, dirtTexture);
    glColor3f(1.0, 1.0, 1.0);
}
void applyMarbleTexture() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glColor3f(1.0, 1.0, 1.0);
}

void disableTexture() {
    glDisable(GL_TEXTURE_2D);

}
