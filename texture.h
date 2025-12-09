#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glut.h>

// 纹理ID声明
extern GLuint woodTexture, brickTexture, darkWoodTexture,
marbleTexture, dirtTexture, groundTexture, riverTexture, carpetTexture;

// 纹理加载和生成函数
void loadTextures();
void generateWoodTexture();
void generateBrickTexture();
void generateDarkWoodTexture();
void generateMarbleTexture();
void generateDirtTexture();
void generateGroundTexture();
void generateRiverTexture();
void generateCarpetTexture();

// 纹理应用函数
void applyWoodTexture();
void applyBrickTexture();
void applyDarkWoodTexture();
void applyMarbleTexture();
void applyDirtTexture();
void applyGroundTexture();
void applyRiverTexture();
void applyCarpetTexture();
void disableTexture();

#endif