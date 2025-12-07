#include "headers.h"
#include "texture.h"
#include "platform.h"
#include <cmath>

// 确保纹理变量在这里定义
GLuint groundTexture, riverTexture;

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

// 绘制森林地面
void drawForestGround() {
    applyGroundTexture();

    // 设置纹理缩放
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glScalef(10, 10, 1.0);  // 缩放纹理以适应800x800平台

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // 平台在y=10平面上，从(0,10,0)到(800,10,800)
    glBegin(GL_QUADS);

    // 设置法线向上
    glNormal3f(0.0, 1.0, 0.0);

    // 顶部（森林地面）
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 10.0, 0.0);
    glTexCoord2f(10.0, 0.0); glVertex3f(800.0, 10.0, 0.0);
    glTexCoord2f(10.0, 10.0); glVertex3f(800.0, 10.0, 800.0);
    glTexCoord2f(0.0, 10.0); glVertex3f(0.0, 10.0, 800.0);

    glEnd();

    // 绘制平台侧面（土坡效果）
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.55f, 0.27f, 0.07f);  // 土棕色

    // 前面
    glBegin(GL_QUADS);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(800.0, 0.0, 0.0);
    glVertex3f(800.0, 10.0, 0.0);
    glVertex3f(0.0, 10.0, 0.0);
    glEnd();

    // 后面
    glBegin(GL_QUADS);
    glVertex3f(0.0, 0.0, 800.0);
    glVertex3f(800.0, 0.0, 800.0);
    glVertex3f(800.0, 10.0, 800.0);
    glVertex3f(0.0, 10.0, 800.0);
    glEnd();

    // 左面
    glBegin(GL_QUADS);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 800.0);
    glVertex3f(0.0, 10.0, 800.0);
    glVertex3f(0.0, 10.0, 0.0);
    glEnd();

    // 右面
    glBegin(GL_QUADS);
    glVertex3f(800.0, 0.0, 0.0);
    glVertex3f(800.0, 0.0, 800.0);
    glVertex3f(800.0, 10.0, 800.0);
    glVertex3f(800.0, 10.0, 0.0);
    glEnd();

    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
}

// 生成蜿蜒的河流路径（使用贝塞尔曲线）
void generateRiverPath(float x, float& z) {
    // 使用贝塞尔曲线参数计算
    static bool initialized = false;
    static float controlPointsX[5], controlPointsZ[5];

    if (!initialized) {
        controlPointsX[0] = 0.0f;    controlPointsZ[0] = 475.0f;
        controlPointsX[1] = 150.0f;  controlPointsZ[1] = 650.0f;  // 向上
        controlPointsX[2] = 300.0f;  controlPointsZ[2] = 300.0f;  // 向下
        controlPointsX[3] = 550.0f;  controlPointsZ[3] = 620.0f;  // 向上
        controlPointsX[4] = 800.0f;  controlPointsZ[4] = 500.0f;  // 终点
        initialized = true;
    }

    // 计算贝塞尔曲线上的点
    float t = x / 800.0f;  // x在0-800之间，t在0-1之间

    // 三次贝塞尔曲线公式: B(t) = (1-t)^3*P0 + 3*(1-t)^2*t*P1 + 3*(1-t)*t^2*P2 + t^3*P3
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    // 计算z坐标
    z = uuu * controlPointsZ[0] +           // (1-t)^3 * P0
        3 * uu * t * controlPointsZ[1] +    // 3*(1-t)^2*t * P1
        3 * u * tt * controlPointsZ[2] +    // 3*(1-t)*t^2 * P2
        ttt * controlPointsZ[3];            // t^3 * P3

    // 添加一些随机变化使河流更自然
    static std::vector<float> noise;
    static bool noiseInitialized = false;

    if (!noiseInitialized) {
        srand(time(NULL));
        for (int i = 0; i <= 800; i += 10) {
            noise.push_back((rand() % 40 - 20) / 100.0f);  // -0.2到0.2的小随机变化
        }
        noiseInitialized = true;
    }

    // 添加噪声
    int index = x / 10;
    if (index < noise.size()) {
        z += noise[index] * 20.0f;  // 添加±4的随机变化
    }

    // 确保河流在z=350-600范围内
    if (z < 350.0f) z = 350.0f;
    if (z > 600.0f) z = 600.0f;
}


// 计算贝塞尔曲线在t处的切线（用于计算河流边缘）
void getRiverTangent(float t, float& tangentX, float& tangentZ) {
    // 贝塞尔曲线控制点（与generateRiverPath中的相同）
    static float controlPointsX[4] = { 0.0f, 200.0f, 400.0f, 800.0f };
    static float controlPointsZ[4] = { 475.0f, 550.0f, 400.0f, 525.0f };

    // 贝塞尔曲线的一阶导数公式：B'(t) = 3*(1-t)^2*(P1-P0) + 6*(1-t)*t*(P2-P1) + 3*t^2*(P3-P2)
    float u = 1.0f - t;
    float uu = u * u;
    float tt = t * t;

    // 计算导数
    tangentX = 3 * uu * (controlPointsX[1] - controlPointsX[0]) +
        6 * u * t * (controlPointsX[2] - controlPointsX[1]) +
        3 * tt * (controlPointsX[3] - controlPointsX[2]);

    tangentZ = 3 * uu * (controlPointsZ[1] - controlPointsZ[0]) +
        6 * u * t * (controlPointsZ[2] - controlPointsZ[1]) +
        3 * tt * (controlPointsZ[3] - controlPointsZ[2]);

    // 归一化
    float length = sqrt(tangentX * tangentX + tangentZ * tangentZ);
    if (length > 0.0001f) {
        tangentX /= length;
        tangentZ /= length;
    }
}

// 绘制蜿蜒的河流 - 改进版
void drawRiver() {
    applyRiverTexture();

    // 设置纹理矩阵用于水流动画效果
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();

    // 添加时间依赖的纹理偏移创建水流动画效果
    static float textureOffset = 0.0f;
    textureOffset += 0.003f;  // 更慢的水流速度
    if (textureOffset > 1.0f) textureOffset -= 1.0f;
    glTranslatef(textureOffset, 0.0f, 0.0f);
    glScalef(0.03f, 0.15f, 1.0f);  // 调整纹理缩放

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    float riverWidth = 80.0f;  // 河流宽度约80

    // 使用四边形绘制河流 - 更自然的外观
    glBegin(GL_QUAD_STRIP);

    // 生成更密集的点用于平滑的河流
    for (float x = 0.0f; x <= 800.0f; x += 5.0f) {  // 使用更小的步长
        float zCenter;
        generateRiverPath(x, zCenter);

        // 使用贝塞尔曲线的切线计算法线
        float t = x / 800.0f;
        float tangentX, tangentZ;
        getRiverTangent(t, tangentX, tangentZ);

        // 计算垂直于切线的法线（旋转90度）
        float normalX = -tangentZ;
        float normalZ = tangentX;

        // 计算左右边缘点
        float leftZ = zCenter + (riverWidth / 2.0f) * normalZ;
        float rightZ = zCenter - (riverWidth / 2.0f) * normalZ;

        // 调整河流宽度，使弯曲处变窄，直线处变宽（更自然）
        float widthVariation = 1.0f;

        // 计算曲率
        float curvature = fabs(tangentZ);  // 切线在z方向的变化表示曲率

        // 曲率大（转弯处）时宽度变小
        if (curvature > 0.3f) {
            widthVariation = 0.8f;  // 转弯处宽度减少20%
        }

        // 重新计算调整后的边缘点
        leftZ = zCenter + (riverWidth / 2.0f) * widthVariation * normalZ;
        rightZ = zCenter - (riverWidth / 2.0f) * widthVariation * normalZ;

        // 纹理坐标
        float texS = x / 800.0f * 3.0f;  // 重复纹理3次
        float texTLeft = 0.0f;
        float texTRight = 1.0f;

        // 左侧边缘点（水面）
        glTexCoord2f(texS, texTLeft);
        glVertex3f(x, 10.1f, leftZ);  // 比地面高0.1单位

        // 右侧边缘点
        glTexCoord2f(texS, texTRight);
        glVertex3f(x, 10.1f, rightZ);
    }

    glEnd();

    // 绘制河岸 - 让河流与地面过渡更自然
    glDisable(GL_TEXTURE_2D);

    
    // 绘制河岸 - 使用泥土纹理
    applyDirtTexture();  // 启用纹理并绑定dirtTexture

    // 设置纹理缩放
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glScalef(0.02f, 0.02f, 1.0f);  // 调整这个缩放值直到纹理看起来合适

    // 左河岸
    glBegin(GL_QUAD_STRIP);
    for (float x = 0.0f; x <= 800.0f; x += 20.0f) {
        float zCenter;
        generateRiverPath(x, zCenter);

        float t = x / 800.0f;
        float tangentX, tangentZ;
        getRiverTangent(t, tangentX, tangentZ);

        float normalX = -tangentZ;
        float normalZ = tangentX;

        // 河流边缘
        float riverEdgeZ = zCenter + 40.0f * normalZ;  // 河流宽度一半

        // 河岸边坡（从河流边缘向外延伸20单位）
        float bankEdgeZ = zCenter + 60.0f * normalZ;

        // 设置纹理坐标和顶点
        glTexCoord2f(x / 50.0f, 0.0f); glVertex3f(x, 10.1f, riverEdgeZ);   // 水面边缘
        glTexCoord2f(x / 50.0f, 1.0f); glVertex3f(x, 10.0f, bankEdgeZ);    // 地面边缘
    }
    glEnd();

    // 右河岸
    glBegin(GL_QUAD_STRIP);
    for (float x = 0.0f; x <= 800.0f; x += 20.0f) {
        float zCenter;
        generateRiverPath(x, zCenter);

        float t = x / 800.0f;
        float tangentX, tangentZ;
        getRiverTangent(t, tangentX, tangentZ);

        float normalX = -tangentZ;
        float normalZ = tangentX;

        // 河流边缘
        float riverEdgeZ = zCenter - 40.0f * normalZ;  // 河流宽度一半

        // 河岸边坡（从河流边缘向外延伸20单位）
        float bankEdgeZ = zCenter - 60.0f * normalZ;

        // 设置纹理坐标和顶点
        glTexCoord2f(x / 50.0f, 0.0f); glVertex3f(x, 10.1f, riverEdgeZ);   // 水面边缘
        glTexCoord2f(x / 50.0f, 1.0f); glVertex3f(x, 10.0f, bankEdgeZ);    // 地面边缘
    }
    glEnd();

    // 恢复纹理矩阵
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);



    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
}

// 初始化平台和河流纹理
void initPlatformTextures() {
    generateGroundTexture();
    generateRiverTexture();
}