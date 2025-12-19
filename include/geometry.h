#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

extern double fang[8][3];
extern double san[8][3];

// 原有函数声明
//绘制球体z
void drawSphere(double r, int lats, int longs, double x, double y, double z);
//绘制梯形柱体
void cons2(double x, double y, double z, double x1, double y1, double z1);
void build2();
//绘制立方体
void cons1(double x, double y, double z, double x1, double y1, double z1);
void build();
//绘制底面与x-y平面平行的平行四边形柱体
void cons3(double x1, double y1, double x2, double y2,double x3, double y3, double x4, double y4,double z_base, double height);
void build3();
//绘制底面与x-y平面平行的三角柱
void cons4(double x1, double y1, double x2, double y2, double x3, double y3, double z_base, double height);
void build4();

#endif