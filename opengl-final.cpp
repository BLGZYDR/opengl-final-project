#include "headers.h"
#include <direct.h> 
#include "texture.h"
#include "scene.h"
#include "house.h"
#include "platform.h"
#include "weather.h" 
#include "lighting.h" 
#include "terrain.h" 
#include "skybox.h" 
#include "player.h" 
#include "shader.h" 
#include "campfire.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

// ==========================================
// 1. 全局变量与状态
// ==========================================
float w = 800.0f; float h = 600.0f;
bool keyState[256] = { false };
float camX = 0.0f; float camZ = 0.0f;
bool isShadowPass = false;
bool debugShadowMap = false;

// Shader & FBO
Shader* standardShader = nullptr;
Shader* shadowShader = nullptr;
Shader* screenShader = nullptr;
GLuint shadowFBO, shadowMap;
const int SHADOW_WIDTH = 4096; const int SHADOW_HEIGHT = 4096;
GLuint mainFBO, sceneColorMap, sceneDepthMap;
GLuint quadVBO;

// 矩阵
float lightSpaceMatrix[16];
float lightSpaceMatrixSimple[16];

void drawSceneContent();

// --- opengl-final.cpp ---

// 1. 预加载所有音效 (在 initSystem 中调用)
void loadAllSounds() {
    std::cout << "[Sound] Pre-loading audio files..." << std::endl;
    // 强制先关闭，防止重复加载报错
    mciSendStringA("close all", NULL, 0, NULL);

    // 打开文件并起别名
    MCIERROR e1 = mciSendStringA("open \"bgm.wav\" alias forest_bgm", NULL, 0, NULL);
    MCIERROR e2 = mciSendStringA("open \"rain.wav\" alias rain_sfx", NULL, 0, NULL);
    MCIERROR e3 = mciSendStringA("open \"snow.wav\" alias snow_sfx", NULL, 0, NULL);

    if (e1 || e2 || e3) std::cout << "[Sound] Warning: Some audio files failed to load. Check file paths!" << std::endl;
}

// 2. 播放函数 (不再使用 repeat 关键字，改由每一帧检查)
void startSound(const std::string& alias) {
    std::string cmd = "play " + alias + " from 0";
    mciSendStringA(cmd.c_str(), NULL, 0, NULL);
}

// 3. 停止函数
void pauseSound(const std::string& alias) {
    mciSendStringA(("stop " + alias).c_str(), NULL, 0, NULL);
}

void updateVolumeAndLoop() {
}

// 兼容函数：将 main 里的 playSound 映射到 startSound
void playSound(const std::string& fileName, const std::string& alias, bool loop = true) {
    startSound(alias);
}

// ========================================================
// 2. HUD 渲染辅助函数
// ========================================================

void renderText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) glutBitmapCharacter(font, c);
}

void drawBar(float x, float y, float width, float height, float percentage, float r, float g, float b) {
    // 1. 强制关闭颜色材质混合
    glDisable(GL_COLOR_MATERIAL);

    // 2. 绘制深黑色背景 (更深，使进度条更显眼)
    glColor4f(0.05f, 0.05f, 0.05f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + width, y);
    glVertex2f(x + width, y + height); glVertex2f(x, y + height);
    glEnd();

    // 3. 绘制填充层 (使用传入的纯色)
    glColor3f(r, g, b);
    float fillWidth = width * (std::max(0.0f, std::min(100.0f, percentage)) / 100.0f);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + fillWidth, y);
    glVertex2f(x + fillWidth, y + height); glVertex2f(x, y + height);
    glEnd();

    // 4. 绘制一个细微的边框 (可选，增强清晰度)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y); glVertex2f(x + width, y);
    glVertex2f(x + width, y + height); glVertex2f(x, y + height);
    glEnd();
}

void drawSurvivalHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // --- 1. 强制重置渲染状态 ---
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- 2. 状态条 (左下角) ---
    float startY = 30.0f;
    float barW = 200.0f;
    float barH = 14.0f;

    // HP - 纯红
    drawBar(20, startY + 60, barW, barH, player.health, 1.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); renderText(20, startY + 76, "HP", GLUT_BITMAP_HELVETICA_12);

    // Hunger - 纯黄
    drawBar(20, startY + 30, barW, barH, player.hunger, 1.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); renderText(20, startY + 46, "HUNGER", GLUT_BITMAP_HELVETICA_12);

    // Warmth - 纯蓝
    drawBar(20, startY, barW, barH, player.warmth, 0.0f, 0.5f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); renderText(20, startY + 16, "WARMTH", GLUT_BITMAP_HELVETICA_12);

    // --- 3. 背包信息 (右上角) ---
    float infoX = w - 180;
    float infoY = h - 50;
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(infoX, infoY, "WOOD: " + std::to_string(campfireSystem.getWoodCount()));
    renderText(infoX, infoY - 25, "RAW FISH: " + std::to_string(player.rawFishCount));

    if (player.cookedFishCount > 0) glColor3f(0.0f, 1.0f, 0.0f); // 有熟鱼时显示绿色
    renderText(infoX, infoY - 50, "COOKED: " + std::to_string(player.cookedFishCount));

    // --- 4. 完善的交互提示逻辑 (屏幕中心下方) ---
    float riverZ; generateRiverPath(player.x, riverZ);
    float distToRiver = abs(player.z - riverZ);
    float distToFire = campfireSystem.getDistToPlayer(player);
    // 检测是否靠近树木 (判定距离设为 20.0f)
    bool isNearTree = terrainSystem.checkTreeCollision(player.x, player.z, 20.0f);

    float centerX = w / 2 - 110;
    float centerY = h / 2 - 80;

    if (distToFire < 80.0f) {
        // --- A. 篝火交互 ---
        if (player.cookedFishCount > 0) {
            glColor3f(0.0f, 1.0f, 1.0f); // 青色：进食提示
            renderText(centerX, centerY, "[E] EAT COOKED FISH (+30 Hunger)");
        }
        else if (player.rawFishCount > 0 && !campfireSystem.getIsCooking() && campfireSystem.getIsLit()) {
            glColor3f(1.0f, 0.5f, 0.0f); // 橙色：烹饪提示
            renderText(centerX, centerY, "[E] COOK RAW FISH (5 Seconds)");
        }
        else if (campfireSystem.getIsCooking()) {
            glColor3f(1.0f, 1.0f, 1.0f); // 白色：进度提示
            int prog = (int)(campfireSystem.getCookingProgress() * 100);
            renderText(centerX, centerY, "COOKING FISH... " + std::to_string(prog) + "%");
        }
        else if (campfireSystem.getWoodCount() > 0) {
            glColor3f(1.0f, 1.0f, 0.0f); // 黄色：维护提示
            renderText(centerX, centerY, "[E] ADD WOOD TO CAMPFIRE");
        }
        else {
            glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
            renderText(centerX, centerY, "CAMPFIRE");
        }
    }
    else if (isNearTree) {
        // --- B. 资源采集：伐木 ---
        glColor3f(0.0f, 1.0f, 0.0f); // 绿色：采集提示
        renderText(centerX, centerY, "[E] GATHER WOOD FROM TREE");
    }
    else if (distToRiver < 45.0f) {
        // --- C. 资源采集：捕鱼 ---
        glColor3f(0.3f, 0.3f, 1.0f); // 亮蓝色：捕鱼提示
        renderText(centerX, centerY, "[E] TRY FISHING IN RIVER");
    }
    else {
        // --- D. 状态引导提示 ---
        if (player.warmth < 30.0f) {
            glColor3f(1.0f, 0.3f, 0.3f);
            renderText(centerX, centerY, "GET BACK TO CAMPFIRE! YOU ARE FREEZING");
        }
        else if (player.hunger < 30.0f) {
            glColor3f(1.0f, 1.0f, 0.0f);
            renderText(centerX, centerY, "FIND A RIVER TO FISH OR TREES FOR WOOD");
        }
    }

    // --- 5. 准星 ---
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(w / 2 - 10, h / 2); glVertex2f(w / 2 + 10, h / 2);
    glVertex2f(w / 2, h / 2 - 10); glVertex2f(w / 2, h / 2 + 10);
    glEnd();

    // 状态还原
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}

// ==========================================
// 3. 矩阵运算辅助
// ==========================================
void mat4Mul(const float* A, const float* B, float* C) {
    float t[16];
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            t[c * 4 + r] = 0.0f;
            for (int k = 0; k < 4; k++) { t[c * 4 + r] += A[k * 4 + r] * B[c * 4 + k]; }
        }
    }
    std::copy(std::begin(t), std::end(t), C);
}

bool mat4Inverse(const float* m, float* out) {
    float inv[16], det; int i;
    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    if (det == 0) return false;
    det = 1.0f / det; for (i = 0; i < 16; i++) out[i] = inv[i] * det;
    return true;
}

// ==========================================
// 4. FBO与管线初始化
// ==========================================
void initShadowMap() {
    glGenFramebuffers(1, &shadowFBO); glGenTextures(1, &shadowMap); glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO); glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE); glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initMainFBO() {
    glGenFramebuffers(1, &mainFBO); glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glGenTextures(1, &sceneColorMap); glBindTexture(GL_TEXTURE_2D, sceneColorMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)w, (int)h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorMap, 0);
    glGenTextures(1, &sceneDepthMap); glBindTexture(GL_TEXTURE_2D, sceneDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)w, (int)h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sceneDepthMap, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initQuad() {
    float q[] = { -1,1,0,1, -1,-1,0,0, 1,-1,1,0, -1,1,0,1, 1,-1,1,0, 1,1,1,1 };
    glGenBuffers(1, &quadVBO); glBindBuffer(GL_ARRAY_BUFFER, quadVBO); glBufferData(GL_ARRAY_BUFFER, sizeof(q), &q, GL_STATIC_DRAW);
}

void drawQuad() {
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    GLint posLoc = glGetAttribLocation(screenShader->Program, "aPos");
    GLint texLoc = glGetAttribLocation(screenShader->Program, "aTexCoords");
    if (posLoc != -1) { glEnableVertexAttribArray(posLoc); glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); }
    if (texLoc != -1) { glEnableVertexAttribArray(texLoc); glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    if (posLoc != -1) glDisableVertexAttribArray(posLoc); if (texLoc != -1) glDisableVertexAttribArray(texLoc);
}

// ==========================================
// 5. 输入处理与日志
// ==========================================
void mouseMove(int x, int y) {
    int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
    if (x == centerX && y == centerY) return;

    float dx = (float)(x - centerX);
    float dy = (float)(y - centerY);

    player.yaw += dx * 0.15f;
    player.pitch -= dy * 0.15f;
    if (player.pitch > 89.0f) player.pitch = 89.0f;
    if (player.pitch < -89.0f) player.pitch = -89.0f;

    glutWarpPointer(centerX, centerY);
}

// opengl-final.cpp 中的键盘事件修改
void keyboardDown(unsigned char key, int x, int y) {
    keyState[key] = true;
    if (key == 27) exit(0);

    if (key == 'e' || key == 'E') {
        // 1. 检测是否在河边抓鱼
        float riverZ;
        extern void generateRiverPath(float x, float& z);
        generateRiverPath(player.x, riverZ);

        float distToRiver = abs(player.z - riverZ);
        float distToFire = campfireSystem.getDistToPlayer(player);

        if (distToRiver < 40.0f && distToFire > 100.0f) { // 在水里且不在火堆旁
            if (rand() % 100 < 40) { // 40% 几率抓到鱼
                player.rawFishCount++;
                std::cout << "[Action] Caught a fish! Raw fish: " << player.rawFishCount << std::endl;
            }
            else {
                std::cout << "[Action] The fish slipped away..." << std::endl;
            }
        }
        else {
            // 2. 否则执行火堆/捡木头逻辑
            campfireSystem.interact(player);
        }
    }

    // 天气日志
    if (key == 'r' || key == 'R') {
        weatherSystem.toggleRain();
        std::cout << "[Weather] 'R' pressed: Rain toggled " << (weatherSystem.isRaining ? "ON" : "OFF") << std::endl;
    }
    if (key == 't' || key == 'T') {
        weatherSystem.toggleSnow();
        std::cout << "[Weather] 'T' pressed: Snow toggled " << (weatherSystem.isSnowing ? "ON" : "OFF") << std::endl;
    }
    if (key == 'c' || key == 'C') {
        weatherSystem.clear(); weatherSystem.clearSnow();
        std::cout << "[Weather] 'C' pressed: Weather cleared." << std::endl;
    }

    // 调试日志
    if (key == 'b' || key == 'B') {
        debugShadowMap = !debugShadowMap;
        std::cout << "[Debug] 'B' pressed: Shadow Map View " << (debugShadowMap ? "ENABLED" : "DISABLED") << std::endl;
    }

    // 移动日志 (仅在初次按下时记录)
    if (key == 'w' || key == 'W') std::cout << "[Move] Forward" << std::endl;
    if (key == 's' || key == 'S') std::cout << "[Move] Backward" << std::endl;
}

void keyboardUp(unsigned char key, int x, int y) { keyState[key] = false; }



// ==========================================
// 6. 核心渲染函数 (display)
// ==========================================
void display(void) {
    static int lastT = 0;
    int nowT = glutGet(GLUT_ELAPSED_TIME);
    float dt = (nowT - lastT) / 1000.0f;
    lastT = nowT;

    // 0. 更新逻辑系统
    campfireSystem.update(dt);
    lightingSystem.update(dt); // 确保时间、太阳/月亮位置在此更新

    // --- 获取当前主光源 (太阳或月亮) ---
    bool isDay = lightingSystem.isDaytime();
    const GLfloat* sunPos = lightingSystem.getSunPosition();
    const GLfloat* moonPos = lightingSystem.getMoonPosition();
    const GLfloat* activeLightPos = isDay ? sunPos : moonPos;
    const GLfloat* activeLightColor = isDay ? lightingSystem.getSunColor() : lightingSystem.getMoonColor();

    // --- Pass 1: Shadow Pass (阴影相机跟随当前活跃光源) ---
    isShadowPass = true;
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // 计算阴影投影方向 (归一化光源向量)
    float dirX = activeLightPos[0], dirY = activeLightPos[1], dirZ = activeLightPos[2];
    float len = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    if (len < 0.1f) { dirX = 0; dirY = 1; dirZ = 0; }
    else { dirX /= len; dirY /= len; dirZ /= len; }

    // 阴影相机位置 (跟随玩家，从光源方向射入)
    float shadowDist = 1500.0f;
    float scX = player.x + dirX * shadowDist;
    float scY = player.y + dirY * shadowDist;
    float scZ = player.z + dirZ * shadowDist;
    if (scY < 400.0f) scY = 400.0f; // 确保阴影相机不进入地平线以下

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(-1500, 1500, -1500, 1500, 1.0, 5000.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    gluLookAt(scX, scY, scZ, player.x, player.y, player.z, 0, 1, 0);

    float lProj[16], lView[16];
    glGetFloatv(GL_PROJECTION_MATRIX, lProj);
    glGetFloatv(GL_MODELVIEW_MATRIX, lView);
    mat4Mul(lProj, lView, lightSpaceMatrixSimple);

    // 使用阴影Shader渲染
    Shader* ts = standardShader; standardShader = shadowShader;
    drawSceneContent();
    standardShader = ts;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    isShadowPass = false;

    // --- Pass 2: Scene Pass (渲染主场景) ---
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glViewport(0, 0, (int)w, (int)h);

    // 背景设为天空颜色
    const GLfloat* sky = lightingSystem.getSkyColor();
    glClearColor(sky[0], sky[1], sky[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(60.0f, w / h, 1.0f, 15000.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    player.setCamera();

    // 1. 绘制天空盒 (关闭着色器)
    glUseProgram(0);
    skybox.draw(player.x, player.y, player.z);

    // 2. 绘制太阳和月亮光球
    lightingSystem.drawLightOrbs();

    // 3. 准备 Standard Shader 并设置光照
    standardShader->use();

    // 设置当前主光源位置和颜色给 Shader
    standardShader->setVec3("lightPos", activeLightPos[0], activeLightPos[1], activeLightPos[2]);
    standardShader->setVec3("lightColor", activeLightColor[0], activeLightColor[1], activeLightColor[2]);

    // 阴影矩阵同步
    float cView[16], icView[16]; glGetFloatv(GL_MODELVIEW_MATRIX, cView);
    if (mat4Inverse(cView, icView)) mat4Mul(lightSpaceMatrixSimple, icView, lightSpaceMatrix);
    glUniformMatrix4fv(glGetUniformLocation(standardShader->Program, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix);

    // 绑定阴影图
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(standardShader->Program, "shadowMap"), 1);
    glActiveTexture(GL_TEXTURE0);

    // 设置篝火和环境光
    campfireSystem.setShaderUniforms(standardShader);
    const GLfloat* amb = lightingSystem.getGlobalAmbient();
    standardShader->setVec3("ambientColor", amb[0], amb[1], amb[2]);

    // 渲染物体 (内部 drawHouse 会根据 lightingSystem.isDaytime() 设置发光)
    drawSceneContent();

    // --- Pass 3: Post-Process ---
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    screenShader->use();
    screenShader->setFloat("distToFire", campfireSystem.getDistToPlayer(player));
    screenShader->setFloat("health", player.health);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, debugShadowMap ? shadowMap : sceneColorMap);
    glUniform1i(glGetUniformLocation(screenShader->Program, "sceneColor"), 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, sceneDepthMap);
    glUniform1i(glGetUniformLocation(screenShader->Program, "sceneDepth"), 1);
    drawQuad();

    // --- Pass 4: UI ---
    drawSurvivalHUD();

    glutSwapBuffers();
}

// ==========================================
// 7. 初始化与主循环
// ==========================================
void initSystem(void) {
    std::cout << "[System] Initializing OpenGL Components..." << std::endl;
    GLenum err = glewInit(); if (GLEW_OK != err) { std::cerr << "GLEW Init Failed!" << std::endl; exit(1); }

    lightingSystem.init(); std::cout << "[System] Lighting System Ready." << std::endl;
    skybox.init(); std::cout << "[System] Skybox Ready." << std::endl;

    standardShader = new Shader(standardVS, standardFS);
    shadowShader = new Shader(shadowVS, shadowFS);
    screenShader = new Shader(screenVS, screenFS);

    initShadowMap(); initMainFBO(); initQuad();
    terrainSystem.init(); std::cout << "[System] Terrain System Ready." << std::endl;

    float fX = 350.0f, fZ = 550.0f;
    float fY = terrainSystem.getHeight(fX, fZ);
    campfireSystem.init(fX, fY, fZ); // 传入正确的 Y 坐标
    std::cout << "[System] Campfire set at (350, 550)." << std::endl;

    loadTextures();
    weatherSystem.init(50000);

    // 1. 预加载
    loadAllSounds();
    // 2. 初始播放背景音乐
    startSound("forest_bgm");
    glutSetCursor(GLUT_CURSOR_NONE);
    std::cout << "[System] Initialization Complete. Game Loop Started." << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "--- Campfire Survival: Forest Edition ---" << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Campfire Survival - Pro Edition");

    initSystem();

    // 【新增】启动背景音乐
    playSound("bgm.wav", "forest_bgm", true);
    std::cout << "[Sound] Background Music started." << std::endl;

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMove);

    glutIdleFunc([]() {
        static int last = 0; int now = glutGet(GLUT_ELAPSED_TIME);
        if (now - last > 16) {
            float dt = (now - last) / 1000.0f;
            player.update(dt, keyState['w'], keyState['s'], keyState['a'], keyState['d'], keyState[' ']);

            updateVolumeAndLoop();

            // 坐标同步
            camX = player.x; camZ = player.z;

            // 生存属性更新
            player.hunger -= 0.15f * dt;
            float dist = campfireSystem.getDistToPlayer(player);
            if (dist < 120.0f && campfireSystem.getIsLit()) {
                player.warmth += 4.0f * dt;
            }
            else {
                player.warmth -= (weatherSystem.isSnowing ? 1.2f : 0.4f) * dt;
            }

            // 属性约束
            player.health = std::max(0.0f, std::min(100.0f, player.health));
            player.hunger = std::max(0.0f, std::min(100.0f, player.hunger));
            player.warmth = std::max(0.0f, std::min(100.0f, player.warmth));

            // 负面状态处理
            if (player.hunger <= 0.01f || player.warmth <= 0.01f) {
                player.health -= 2.0f * dt;
                static float lastLog = 0;
                if (now - lastLog > 5000) { // 每5秒提醒一次
                    std::cout << "[Warning] Player is starving or freezing! HP: " << (int)player.health << std::endl;
                    lastLog = (float)now;
                }
            }
            else if (player.hunger > 80 && player.warmth > 80) {
                player.health += 1.0f * dt;
            }

            weatherSystem.update(dt);
            lightingSystem.update(dt);

            last = now; glutPostRedisplay();
        }
        });

    glutMainLoop();
    return 0;
}
