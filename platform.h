#pragma once
#ifndef PLATFORM_H
#define PLATFORM_H

// 纹理声明
extern GLuint groundTexture, riverTexture;

// 平台和河流绘制函数
void drawRiver();
void drawForestGround();

// 纹理生成函数
void generateGroundTexture();
void generateRiverTexture();
void getRiverTangent();

// 纹理应用函数
void applyGroundTexture();
void applyRiverTexture();

// 初始化纹理函数 - 添加这行
void initPlatformTextures();

#endif