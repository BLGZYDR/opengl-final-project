#include "scene.h"
#include "house.h"
#include "weather.h"
#include "campfire.h"
#include "platform.h"
#include "terrain.h" 
#include <iostream>

// 引用全局相机变量
extern float camX, camZ;

void drawSceneContent() {
    terrainSystem.draw(camX, camZ);

    // 1. 河流高度微调
    glPushMatrix();
    // 将河流平面稍微降低一点点，避免与绝对平整的河床发生深度冲突
    glTranslatef(0, -2.0f, 0);
    drawRiver();
    glPopMatrix();

    // 2. 绘制房子 - 【关键修改】
    // 因为地基在 y=50，而房屋模型地板也在 50，所以不需要 y 轴偏移了
    glPushMatrix();
    glTranslatef(0, 0, -120.0f);
    drawHouse();
    glUseProgram(0);
    weatherSystem.drawSmoke();
    glPopMatrix();

    // 3. 篝火高度动态获取 (保持之前的逻辑)
    campfireSystem.draw();

    weatherSystem.draw();
    weatherSystem.renderSnowAccumulation();
}
