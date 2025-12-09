#pragma once
#ifndef PLATFORM_H
#define PLATFORM_H

// 纹理声明（现在在texture.h中定义）
extern GLuint groundTexture, riverTexture;

// 森林和河流绘制函数
void drawRiver();
void drawForestGround();

// 河流路径生成函数
void generateRiverPath(float x, float& z);
void getRiverTangent(float t, float& tangentX, float& tangentZ);

#endif