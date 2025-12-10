#ifndef HEADERS_H
#define HEADERS_H

// 使用 freeglut
#define FREEGLUT_STATIC
#include<GL/freeglut.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<vector>
#include<iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <algorithm>  // 添加算法头文件，用于std::remove_if

using std::cout;

extern GLfloat w;
extern GLfloat h;

// 颜色定义
#define WHITE      1.0, 1.0, 1.0
#define GREEN      0.0, 0.502, 0.0
#define RED        0.6, 0.1, 0.1
#define DARK_RED    0.6, 0.078, 0.235
#define GRAY       0.502, 0.502, 0.502
#define HGRAY      0.117, 0.180, 0.227
#define BLUE       0.0, 0.0, 1.0
#define GOLD       1.0, 215.0/255.0, 0.0
#define HGREEN     0.0, 100.0/255.0, 0.0
#define BROWN      73/255.0, 57/255.0, 45/255.0
#define MEN        244.0/255.0, 164.0/255.0, 96.0/255.0
#define MENBA      139.0/255.0, 69.0/255.0, 19.0/255.0
#define DARK_GREEN 0.0, 0.4, 0.0     
#define FOREST_GREEN 0.133, 0.545, 0.133  
#define WATER_BLUE 0.0, 0.5, 1.0     

#define PI 3.14159265f

#endif
