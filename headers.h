#ifndef HEADERS_H
#define HEADERS_H

// ==========================================
// 1. OpenGL 核心库
// ==========================================
#include <GL/glew.h> 
#define FREEGLUT_STATIC
#include <GL/freeglut.h>

// 在 headers.h 的 #include 区域添加
#include <mmsystem.h>
#include <mciapi.h>
#pragma comment(lib, "winmm.lib")

// ==========================================
// 2. C++ 标准库
// ==========================================
#include <iostream>
#include <vector>
#include <cmath>    
#include <ctime>    
#include <cstdlib>  
#include <algorithm>
#include <map>      
#include <string>   

using std::cout;
using std::endl;

// ==========================================
// 3. 全局变量声明
// ==========================================
extern GLfloat w;
extern GLfloat h;

// 【新增】全局 Standard Shader 指针声明
// 使用 class 前置声明，避免头文件循环依赖
extern class Shader* standardShader;

// 【新增】第二阶段：阴影相关全局变量
extern GLuint shadowFBO;      // 阴影帧缓冲
extern GLuint shadowMap;      // 阴影深度纹理
extern const int SHADOW_WIDTH;  // 分辨率
extern const int SHADOW_HEIGHT;
extern float lightSpaceMatrix[16]; // 太阳视角的投影矩阵
extern float viewMatrix[16];       // 玩家相机的观察矩阵

// 【新增】第三阶段：后处理与 SSAO 相关变量
extern GLuint mainFBO;       // 主场景帧缓冲
extern GLuint sceneColorMap; // 场景颜色纹理
extern GLuint sceneDepthMap; // 场景深度纹理 (用于计算 SSAO)
extern class Shader* screenShader; // 后处理 Shader
extern bool isShadowPass;

extern int playerWoodCount;      // 玩家拥有的木材数量
extern bool isCampfireLit;       // 篝火是否点燃
extern float campfireHeat;       // 篝火剩余热量（随时间减少）
extern float campfirePos[3];     // 篝火位置

// ==========================================
// 4. 颜色宏定义 (保持不变)
// ==========================================
#define WHITE        1.0, 1.0, 1.0
#define GREEN        0.0, 0.502, 0.0
#define RED          0.6, 0.1, 0.1
#define DARK_RED     0.6, 0.078, 0.235
#define GRAY         0.502, 0.502, 0.502
#define HGRAY        0.117, 0.180, 0.227
#define BLUE         0.0, 0.0, 1.0
#define GOLD         1.0, 215.0/255.0, 0.0
#define HGREEN       0.0, 100.0/255.0, 0.0
#define BROWN        73.0/255.0, 57.0/255.0, 45.0/255.0
#define MEN          244.0/255.0, 164.0/255.0, 96.0/255.0
#define MENBA        139.0/255.0, 69.0/255.0, 19.0/255.0
#define DARK_GREEN   0.0, 0.4, 0.0     
#define FOREST_GREEN 0.133, 0.545, 0.133  
#define WATER_BLUE   0.0, 0.5, 1.0     

#define PI 3.14159265f

#endif
