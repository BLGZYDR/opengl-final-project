#include "headers.h"
#include "house.h"
#include "geometry.h"
#include "texture.h"
#include "weather.h"
#include "lighting.h" 
#include <iostream>

void drawHouse() {
    GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    // 关闭剔除面，确保内外墙都可见
    glDisable(GL_CULL_FACE);

    // 设置高光材质
    GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

    // ==================
    // 房子主体结构
    // ==================
    glPushMatrix();
    applyWoodTexture(); // 这里的 apply 函数现在会自动设置 Shader 的 useTexture = 1.0

    // 纹理矩阵设置 (保持不变)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // 后墙
    cons2(270, 50, -60, 80, 160, 10); build2();
    cons2(430, 50, -60, -80, 160, 10); build2();

    // 前墙
    cons4(270, 130, 335, 130, 335, 195, 100, 10); build4();
    cons4(430, 130, 365, 130, 365, 195, 100, 10); build4();
    cons4(335, 195, 365, 195, 350, 210, 100, 10); build4();
    cons1(335, 130, 100, 30, 10, 10); build();
    cons1(335, 170, 100, 30, 25, 10); build();
    cons1(270, 50, 100, 20, 80, 10); build();
    cons1(410, 50, 100, 20, 80, 10); build();
    cons1(290, 50, 100, 30, 40, 10); build();
    cons1(380, 50, 100, 35, 40, 10); build();
    cons1(320, 50, 100, 15, 70, 10); build();
    cons1(365, 50, 100, 15, 70, 10); build();
    cons1(290, 120, 100, 140, 10, 10); build();
    cons1(335, 110, 100, 30, 10, 10); build();

    // 左墙
    cons2(270, 120, -60, 10, 20, 160); build2();
    cons1(270, 50, -60, 10, 70, 70); build();
    cons1(270, 50, 40, 10, 70, 60); build();
    cons1(270, 50, 10, 10, 40, 30); build();

    // 右墙
    cons2(430, 120, -50, -10, 20, 150); build2();
    cons1(430, 50, -50, -10, 70, 60); build();
    cons1(430, 50, 40, -10, 70, 60); build();
    cons1(430, 50, 10, -10, 40, 30); build();

    // 关闭木纹
    disableTexture(); // 自动设置 Shader useTexture = 0.0
    glPopMatrix();

    // ==================
    // 房顶 (砖块纹理)
    // ==================
    glPushMatrix();
    applyBrickTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);

    cons3(350, 210, 350, 220, 260, 120, 260, 130, -65, 180); build3();
    cons3(350, 210, 350, 220, 440, 120, 440, 130, -65, 180); build3();

    // 烟囱 (无纹理)
    cons2(300, 180, 10, -20, -40, 20); build2();
    disableTexture();
    glPopMatrix();

    // ==================
    // 大门 (深色木纹)
    // ==================
    glPushMatrix();
    applyDarkWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(335, 60, 100, 30, 50, 5); build();
    disableTexture();

    // 门装饰 (灰色，无纹理)
    glColor3f(GRAY);
    cons1(340, 65, 105, 20, 2, 0.5); build();
    cons1(340, 105, 105, 20, -2, 0.5); build();
    cons1(340, 84, 105, 20, 2, 0.5); build();
    cons1(340, 65, 105, 2, 40, 0.5); build();
    cons1(358, 65, 105, 2, 40, 0.5); build();

    // ==================
    // 棚子 (砖块纹理)
    // ==================
    glPushMatrix();
    applyBrickTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons3(350, 135, 350, 125, 320, 105, 320, 95, 110, 70); build3();
    cons3(350, 135, 350, 125, 380, 105, 380, 95, 110, 70); build3();
    cons1(325, 50, 170, 4, 57, 4); build();
    cons1(375, 50, 170, -4, 57, 4); build();
    disableTexture();
    glPopMatrix();

    // ==================
    // 大理石部分 (走道、围栏)
    // ==================
    applyMarbleTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // 走道
    cons1(335, 50, 100, 30, 10, 60); build();
    cons1(325, 50, 110, 10, 10, 50); build();
    cons1(365, 50, 110, 10, 10, 50); build();
    cons1(325, 40, 110, 50, 10, 80); build();

    // 围栏
    for (int i = 0; i < 6; i++) {
        cons1(328, 60, 110 + i * 10, 4, 30, 4); build();
    }
    cons1(328, 90, 110, 4, 4, 58); build();
    for (int i = 0; i < 6; i++) {
        cons1(368, 60, 110 + i * 10, 4, 30, 4); build();
    }
    cons1(368, 90, 110, 4, 4, 58); build();

    // 台阶
    cons1(325, 50, 160, 50, 10, 10); build();
    cons1(335, 50, 170, 30, 5, 10); build();

    // 地板
    cons1(275, 50, -55, 150, 10, 150); build();

    // 外围边缘
    cons1(265, 40, -65, 170, 23, 5); build();
    cons1(265, 40, -60, 5, 23, 175); build();
    cons1(435, 40, -60, -5, 23, 175); build();
    cons1(270, 40, 110, 65, 23, 5); build();
    cons1(430, 40, 110, -65, 23, 5); build();

    // 烟囱顶部
    cons1(305, 185, 5, -30, -10, 5); build();
    cons1(305, 185, 30, -30, -10, 5); build();
    cons1(305, 185, 10, -5, -10, 20); build();
    cons1(280, 185, 10, -5, -10, 20); build();

    disableTexture();
    glPopMatrix();

    // ==================
    // 内部装饰
    // ==================

    // 床板
    glPushMatrix();
    applyWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(40, 40, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(280, 60, -50, 30, 20, 3); build();
    cons1(280, 60, -47, 30, 10, 60); build();
    disableTexture();
    glPopMatrix();

    // 被子
    glColor3f(WHITE);
    cons1(280, 70, -47, 30, 10, 15); build();
    glColor3f(RED);
    cons1(280, 70, -32, 30, 10, 45); build();

    // 沙发垫子
    glColor3f(WHITE);
    cons1(420, 70, -10, -30, 10, 60); build();
    cons1(420, 80, -10, -5, 12, 60); build();

    // 沙发底座
    glPushMatrix();
    applyWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(40, 40, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(420, 60, -20, -30, 30, 10); build();
    cons1(420, 60, 50, -30, 30, 10); build();
    cons1(420, 60, -20, -30, 10, 80); build();
    disableTexture();
    glPopMatrix();

    // 架子 (深色木纹)
    glPushMatrix();
    applyDarkWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(10, 10, 1.0);
    cons1(420, 60, 60, -30, 2, 40); build();
    cons1(420, 80, 60, -30, 2, 40); build();
    cons1(420, 100, 60, -30, 2, 40); build();
    cons1(420, 60, 60, -30, 42, 2); build();
    cons1(420, 60, 98, -30, 42, 2); build();
    disableTexture();
    glPopMatrix();

    // 地毯
    glPushMatrix();
    applyCarpetTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE); glLoadIdentity(); glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(290, 60, -40, 130, 1, 130); build();
    disableTexture();
    glPopMatrix();

    // 窗户边框 (BROWN)
    glColor3f(BROWN);
    // 正面窗户
    cons1(330, 135, 110, 40, 5, 1); build();
    cons1(330, 170, 110, 40, 5, 1); build();
    cons1(330, 140, 110, 5, 30, 1); build();
    cons1(365, 140, 110, 5, 30, 1); build();
    cons1(349, 140, 110, 2, 30, 1); build();
    cons1(285, 85, 110, 40, 5, 1); build();
    cons1(285, 120, 110, 40, 5, 1); build();
    cons1(285, 90, 110, 5, 30, 1); build();
    cons1(320, 90, 110, 5, 30, 1); build();
    cons1(304, 90, 110, 2, 30, 1); build();
    cons1(375, 85, 110, 40, 5, 1); build();
    cons1(375, 120, 110, 40, 5, 1); build();
    cons1(375, 90, 110, 5, 30, 1); build();
    cons1(410, 90, 110, 5, 30, 1); build();
    cons1(394, 90, 110, 2, 30, 1); build();

    // 左面窗户
    cons1(270, 85, 5, -1, 5, 40); build();
    cons1(270, 125, 5, -1, -5, 40); build();
    cons1(270, 90, 10, -1, 30, -5); build();
    cons1(270, 90, 45, -1, 30, -5); build();
    cons1(270, 90, 24, -1, 30, 2); build();

    // 右面窗户
    cons1(431, 85, 5, -1, 5, 40); build();
    cons1(431, 125, 5, -1, -5, 40); build();
    cons1(431, 90, 10, -1, 30, -5); build();
    cons1(431, 90, 45, -1, 30, -5); build();
    cons1(431, 90, 24, -1, 30, 2); build();

    // ==================
    // 窗户玻璃 (半透明)
    // ==================
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    bool isNight = !lightingSystem.isDaytime();

    if (isNight) {
        // 仅在晚上给窗户设置微弱的暖色发光
        GLfloat window_emit[] = { 0.5f, 0.35f, 0.1f, 1.0f }; // 暗淡的暖橙色
        glMaterialfv(GL_FRONT, GL_EMISSION, window_emit);
        glColor4f(0.6f, 0.4f, 0.2f, 0.8f); // 玻璃颜色也设为暗橙色
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
        glColor4f(0.8f, 0.9f, 1.0f, 0.3f); // 白天普通的半透明玻璃
    }

    // 左面窗户玻璃
    cons1(270, 90, 10, 1, 30, 30); build();
    // 右面窗户玻璃
    cons1(430, 90, 10, -1, 30, 30); build();
    // 正面窗户玻璃
    cons1(335, 140, 110, 30, 30, 1); build();
    cons1(290, 90, 110, 30, 30, 1); build();
    cons1(380, 90, 110, 30, 30, 1); build();

    // 3. 【关键】绘制完玻璃后立即重置发光
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
    glDisable(GL_BLEND);
}
