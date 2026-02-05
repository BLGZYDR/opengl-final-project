#pragma once
#include "headers.h"

class Skybox {
public:
    void init();
    // 绘制天空盒，需要传入相机当前位置，保证天空盒永远包围相机
    void draw(float camX, float camY, float camZ);
};

extern Skybox skybox;
