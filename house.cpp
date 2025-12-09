#include "headers.h"
#include "geometry.h"
#include "house.h"
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

    //房子四面墙壁
    glPushMatrix();
    applyWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    //房子后墙
    cons2(320, 10, 100, 80, 160, 10);
    build2();
    cons2(480, 10, 100, -80, 160, 10);
    build2();

    //房子前墙
    cons4(320, 90, 385, 90, 385, 155, 260, 10);
    build4();
    cons4(480, 90, 415, 90, 415, 155, 260, 10);
    build4();
    cons4(385, 155, 415, 155, 400, 170, 260, 10);
    build4();
    cons1(385, 90, 260, 30, 10, 10);
    build();
    cons1(385, 130, 260, 30, 25, 10);
    build();
    cons1(320, 10, 260, 20, 80, 10);
    build();
    cons1(460, 10, 260, 20, 80, 10);
    build();
    cons1(340, 10, 260, 30, 40, 10);
    build();
    cons1(430, 10, 260, 35, 40, 10);
    build();
    cons1(370, 10, 260, 15, 70, 10);
    build();
    cons1(415, 10, 260, 15, 70, 10);
    build();
    cons1(340, 80, 260, 140, 10, 10);
    build();
    cons1(385, 70, 260, 30, 10, 10);
    build();

    //房子左墙
    cons2(320, 80, 100, 10, 20, 160);
    build2();
    cons1(320, 10, 100, 10, 70, 70);
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

    //绘制大门
    glPushMatrix();
    applyDarkWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(385, 20, 260, 30, 50, 5);
    build();
    disableTexture();
    //绘制门上的装饰
    glColor3f(GRAY);
    cons1(390, 25, 265, 20, 2, 0.5);
    build();
    cons1(390, 65, 265, 20, -2, 0.5);
    build();
    cons1(390, 44, 265, 20, 2, 0.5);
    build();
    cons1(390, 25, 265, 2, 40, 0.5);
    build();
    cons1(408, 25, 265, 2, 40, 0.5);
    build();
    //绘制门前的棚子
    glPushMatrix();
    applyBrickTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons3(400, 95, 400, 85, 370, 65, 370, 55, 270, 70);
    build3();
    cons3(400, 95, 400, 85, 430, 65, 430, 55, 270, 70);
    build3();
    cons1(375, 10, 330, 4, 57, 4);
    build();
    cons1(425, 10, 330, -4, 57, 4);
    build();
    disableTexture();
    glPopMatrix();


    applyMarbleTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    //绘制入门走廊
    cons1(385, 10, 260, 30, 10, 60);
    build();
    cons1(375, 10, 270, 10, 10, 50);
    build();
    cons1(415, 10, 270, 10, 10, 50);
    build();
    //绘制左边的围栏
    for (int i = 0; i < 6; i++)
    {
        cons1(378, 20, 270 + i * 10, 4, 30, 4);
        build();
    }
    cons1(378, 50, 270, 4, 4, 58);
    build();
    //绘制右边围栏
    for (int i = 0; i < 6; i++)
    {
        cons1(418, 20, 270 + i * 10, 4, 30, 4);
        build();
    }
    cons1(418, 50, 270, 4, 4, 58);
    build();
    //绘制台阶
    cons1(375, 10, 320, 50, 10, 10);
    build();
    cons1(385, 10, 330, 30, 5, 10);
    build();
    //绘制房子地板
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
    
    //绘制房子内部装饰
    //绘制床铺床板
    glPushMatrix();
    applyWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(40, 40, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(330, 20, 110, 30, 20, 3);
    build();
    cons1(330, 20, 113, 30, 10, 60);
    build();
    disableTexture();
    glPopMatrix();
    //绘制被褥
    glColor3f(WHITE);
    cons1(330, 30, 113, 30, 10, 15);
    build();
    glColor3f(RED);
    cons1(330, 30, 128, 30, 10, 45);
    build();

    //绘制沙发
    //绘制垫子
    cons1(470, 30, 150, -30, 10, 60);
    build();
    cons1(470, 40, 150, -5, 12, 60);
    build();
    //绘制木制底座
    glPushMatrix();
    applyWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(40, 40, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(470, 20, 140, -30, 30, 10);
    build();
    cons1(470, 20, 210, -30, 30, 10);
    build();
    cons1(470, 20, 140, -30, 10, 80);
    build();
    disableTexture();
    glPopMatrix();
    //绘制柜子
    glPushMatrix();
    applyDarkWoodTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    cons1(470, 20, 220, -30, 2, 40);
    build();
    cons1(470, 40, 220, -30, 2, 40);
    build();
    cons1(470, 60, 220, -30, 2, 40);
    build();
    cons1(470, 20, 220, -30, 42, 2);
    build();
    cons1(470, 20, 258, -30, 42, 2);
    build();
    disableTexture();
    glPopMatrix();

    //绘制地毯
    glPushMatrix();
    applyCarpetTexture();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(10, 10, 1.0);
    glMatrixMode(GL_MODELVIEW);
    cons1(340,20,120,130,1,130);
    build();
    disableTexture();
    glPopMatrix();
    
    
    //绘制窗户玻璃
    glColor4f(WHITE, 0.2);
    //正面窗户1
    cons1(320, 50, 170, 10, 30, 30);
    build();
    //正面窗户2
    cons1(480, 50, 170, -10, 30, 30);
    build();
    //正面窗户3
    cons1(385, 100, 260, 30, 30, 10);
    build();
    //左面窗户
    cons1(340, 50, 260, 30, 30, 10);
    build();
    //右面窗户
    cons1(430, 50, 260, 30, 30, 10);
    build();

    glPopMatrix();
    //绘制窗户边框
    glColor3f(BROWN);
    //正面窗户边框1
    cons1(380, 95, 270, 40, 5, 1);
    build();
    cons1(380, 130, 270, 40, 5, 1);
    build();
    cons1(380, 100, 270, 5, 30, 1);
    build();
    cons1(415, 100, 270, 5, 30, 1);
    build();
    cons1(399, 100, 270, 2, 30, 1);
    build();
    //正面窗户边框2
    cons1(335, 45, 270, 40, 5, 1);
    build();
    cons1(335, 80, 270, 40, 5, 1);
    build();
    cons1(335, 50, 270, 5, 30, 1);
    build();
    cons1(370, 50, 270, 5, 30, 1);
    build();
    cons1(354, 50, 270, 2, 30, 1);
    build();
    //正面窗户边框3
    cons1(425, 45, 270, 40, 5, 1);
    build();
    cons1(425, 80, 270, 40, 5, 1);
    build();
    cons1(425, 50, 270, 5, 30, 1);
    build();
    cons1(460, 50, 270, 5, 30, 1);
    build();
    cons1(444, 50, 270, 2, 30, 1);
    build();
    //左面窗户边框
    cons1(320, 45, 165, -1, 5, 40);
    build();
    cons1(320, 85, 165, -1, -5, 40);
    build();
    cons1(320, 50, 170, -1, 30, -5);
    build();
    cons1(320, 50, 205, -1, 30, -5);
    build();
    cons1(320, 50, 184, -1, 30, 2);
    build();
    //右面窗户边框
    cons1(481, 45, 165, -1, 5, 40);
    build();
    cons1(481, 85, 165, -1, -5, 40);
    build();
    cons1(481, 50, 170, -1, 30, -5);
    build();
    cons1(481, 50, 205, -1, 30, -5);
    build();
    cons1(481, 50, 184, -1, 30, 2);
    build();

    glFlush();
    glutSwapBuffers();

}


