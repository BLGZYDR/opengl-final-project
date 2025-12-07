#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

extern double fang[8][3];
extern double san[8][3];

// 原有函数声明
void drawSphere(double r, int lats, int longs, double x, double y, double z);
void cons2(double x, double y, double z, double x1, double y1, double z1);
void build2();
void cons1(double x, double y, double z, double x1, double y1, double z1);
void build();
void cons3(double x1, double y1, double x2, double y2,double x3, double y3, double x4, double y4,double z_base, double height);
void build3();
// 河流绘制函数
void drawRiver(float startX, float startZ, float endX, float endZ, float y, float width, float x1, float x2, int segments);
#endif