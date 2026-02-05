#include "skybox.h"

Skybox skybox;

void Skybox::init() {
    // 这里不需要加载纹理，因为我们要用纯代码画渐变色
}

void Skybox::draw(float camX, float camY, float camZ) {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST); // 关闭深度测试，确保护天空永远在最背景
    glDepthMask(GL_FALSE);

    glPushMatrix();
    // 【关键】天空盒必须跟随相机的 X, Y, Z，否则你飞高了就穿帮了
    glTranslatef(camX, camY, camZ);

    // 【关键】放大天空盒！之前是 2000，现在地形是 4000，所以天空盒至少要 5000
    float size = 5000.0f;

    // 颜色调整 (让天空更蓝一点，去除底部的死黑)
    float topR = 0.0f, topG = 0.3f, topB = 0.7f;
    float horR = 0.6f, horG = 0.8f, horB = 0.9f;
    float botR = 0.6f, botG = 0.8f, botB = 0.9f; // 底部也改成淡蓝色，防止露馅

    glBegin(GL_QUADS);
    // ... (绘制6个面，代码太长省略，只需把原来的 glVertex3f 中的 size 逻辑保留即可)
    // 确保你的 draw 代码里用的变量是这个新的 size

    // 前面
    glColor3f(horR, horG, horB); glVertex3f(-size, -size, -size);
    glColor3f(horR, horG, horB); glVertex3f(size, -size, -size);
    glColor3f(topR, topG, topB); glVertex3f(size, size, -size);
    glColor3f(topR, topG, topB); glVertex3f(-size, size, -size);
    // 后面
    glColor3f(horR, horG, horB); glVertex3f(size, -size, size);
    glColor3f(horR, horG, horB); glVertex3f(-size, -size, size);
    glColor3f(topR, topG, topB); glVertex3f(-size, size, size);
    glColor3f(topR, topG, topB); glVertex3f(size, size, size);
    // 左面
    glColor3f(horR, horG, horB); glVertex3f(-size, -size, size);
    glColor3f(horR, horG, horB); glVertex3f(-size, -size, -size);
    glColor3f(topR, topG, topB); glVertex3f(-size, size, -size);
    glColor3f(topR, topG, topB); glVertex3f(-size, size, size);
    // 右面
    glColor3f(horR, horG, horB); glVertex3f(size, -size, -size);
    glColor3f(horR, horG, horB); glVertex3f(size, -size, size);
    glColor3f(topR, topG, topB); glVertex3f(size, size, size);
    glColor3f(topR, topG, topB); glVertex3f(size, size, -size);
    // 顶面
    glColor3f(topR, topG, topB); glVertex3f(-size, size, size);
    glColor3f(topR, topG, topB); glVertex3f(size, size, size);
    glColor3f(topR, topG, topB); glVertex3f(size, size, -size);
    glColor3f(topR, topG, topB); glVertex3f(-size, size, -size);
    // 底面
    glColor3f(botR, botG, botB); glVertex3f(-size, -size, -size);
    glColor3f(botR, botG, botB); glVertex3f(size, -size, -size);
    glColor3f(botR, botG, botB); glVertex3f(size, -size, size);
    glColor3f(botR, botG, botB); glVertex3f(-size, -size, size);
    glEnd();

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST); // 恢复深度测试
    glEnable(GL_LIGHTING);
}
