#include "headers.h"
#include "geometry.h"
#include "scene.h"
#include "texture.h"
#include"platform.h"
#include <iostream>
using namespace std;

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

    //绘制森林地面平台
    drawForestGround();

    //绘制蜿蜒河流
    drawRiver();

    glPopMatrix();

    
    //绘制土地地基
    glPushMatrix();
    applyDirtTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(40, 40, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(-1, -10, -1, 802, 19, 802);
    build();
    disableTexture();
    glPopMatrix();

    //房子后墙
    glPushMatrix();
    applyWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);

    cons2(320, 10, 100, 80, 160, 10);
    build2();
    cons2(480, 10, 100, -80, 160, 10);
    build2();

    //房子前墙
    cons2(320, 25, 260, 65, 130, 10);
    build2();
    cons2(480, 25, 260, -65, 130, 10);
    build2();
    cons1(320, 10, 260, 65, 15, 10);
    build();
    cons1(480, 10, 260, -65, 15, 10);
    build();
    cons2(385, 140, 260, 15, 30, 10);
    build2();
    cons2(415, 140, 260, -15, 30, 10);
    build2();
    cons1(385, 120, 260, 30, 20, 10);
    build();
    cons1(385, 115, 260, 30, 15, 10);
    build();
    cons1(385, 70, 260, 30, 15, 10);
    build();

    //房子左墙
    cons2(320, 80, 110, 10, 20, 150);
    build2();
    cons1(320, 10, 110, 10, 70, 60);
    build();
    cons1(320, 10, 200, 10, 70, 60);
    build();
    cons1(320, 10, 170, 10, 40, 30);
    build();

    //房子右墙
    cons2(480, 80, 110, -10, 20, 150);
    build2();
    cons1(480, 10, 110, -10, 70, 60);
    build();
    cons1(480, 10, 200, -10, 70, 60);
    build();
    cons1(480, 10, 170, -10, 40, 30);
    build();

    disableTexture();
    glPopMatrix();

    //绘制房顶
    glPushMatrix();
    applyBrickTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons3(400, 170, 400, 180, 310, 80, 310, 90, 95, 180);
    build3();
    cons3(400, 170, 400, 180, 490, 80, 490, 90, 95, 180);
    build3();

    //绘制烟囱
    cons2(350, 140, 170, -20, -40, 20);
    build2();
    disableTexture();
    glPopMatrix();

    //绘制窗户
    glPushMatrix();
    glColor4f(BLUE, 0.35);
    disableTexture();
    cons1(385, 85, 260, 30, 30, 10);
    build();
    cons1(320, 50, 170, 10, 30, 30);
    build();
    cons1(480, 50, 170, -10, 30, 30);
    build();
    glPopMatrix();

    glPushMatrix();
    //绘制大门
    applyDarkWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(385, 20, 260, 30, 50, 5);
    build();
    disableTexture();

    applyMarbleTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    //绘制台阶
    cons1(385, 10, 260, 30, 10, 10);
    build();
    cons1(385, 10, 270, 30, 10, 10);
    build();
    cons1(385, 10, 280, 30, 5, 10);
    build();
    //绘制房子地面
    cons1(325, 10, 105, 150, 10, 150);
    build();
    //绘制外围边缘
    cons1(315, 10, 95, 170, 10, 5);
    build();
    cons1(315, 10, 100, 5, 10, 175);
    build();
    cons1(485, 10, 100, -5, 10, 175);
    build();
    cons1(320, 10, 270, 65, 10, 5);
    build();
    cons1(480, 10, 270, -65, 10, 5);
    build();
    //绘制烟囱顶部边缘
    cons1(355, 145, 165, -30, -10, 5);
    build();
    cons1(355, 145, 190, -30, -10, 5);
    build();
    cons1(355, 145, 170, -5, -10, 20);
    build();
    cons1(330, 145, 170, -5, -10, 20);
    build();
    disableTexture();
    glPopMatrix();

    //绘制门把手
    glColor3f(RED);
    glTranslatef(390, 45, 265);
    glutSolidSphere(2, 20, 20);


    glFlush();
    glutSwapBuffers();
}


