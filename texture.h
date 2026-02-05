#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"

// 纹理ID声明
extern GLuint woodTexture, brickTexture, darkWoodTexture,
marbleTexture, dirtTexture, groundTexture, riverTexture, carpetTexture, snowTexture;

// 纹理生成函数
void loadTextures();
void generateWoodTexture();
void generateBrickTexture();
void generateDarkWoodTexture();
void generateMarbleTexture();
void generateDirtTexture();
void generateGroundTexture();
void generateRiverTexture();
void generateCarpetTexture();
void generateSnowTexture();

// 纹理应用函数
void applyWoodTexture();
void applyBrickTexture();
void applyDarkWoodTexture();
void applyMarbleTexture();
void applyDirtTexture();
void applyGroundTexture();
void applyRiverTexture();
void applyCarpetTexture();
void applySnowTexture();
void disableTexture();

#endif
