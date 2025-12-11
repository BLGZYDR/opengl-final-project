#include "scene.h"
#include "house.h"
#include "weather.h"
#include "platform.h"
#include <iostream>

GLfloat w = 700;
GLfloat h = 700;
double rotate_x = 0.0;
double rotate_y = 0.0;

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);
    glTranslatef(-600, -200, -600);
    glScalef(1.5, 1.5, 1.5);

    glPushMatrix();

    // 启用上下雪系统
    weatherSystem.renderSnowAccumulation();

    // 绘制森林地面平台
    drawForestGround();

    // 绘制溪流河流
    drawRiver();

    glPopMatrix();

    // 绘制房子
    drawHouse();

    // 加上下雨系统
    weatherSystem.renderRainAccumulation();

    glFlush();
    glutSwapBuffers();
}