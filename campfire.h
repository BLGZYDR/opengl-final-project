#ifndef CAMPFIRE_H
#define CAMPFIRE_H

#include "headers.h"

class CampfireSystem {
public:
    CampfireSystem();

    // 初始化位置
    void init(float x, float y, float z);

    // 每帧更新：处理燃烧时间、光照抖动计算
    void update(float deltaTime);

    // 渲染火堆物体（石头、木柴、火焰视觉效果）
    void draw();

    // 交互：玩家尝试添柴或捡木头
    void interact(class Player& player);

    // 将篝火光照数据传给 Shader
    void setShaderUniforms(class Shader* shader);

    // 获取玩家到火堆的距离（供寒冷滤镜使用）
    float getDistToPlayer(class Player& player);

    int getWoodCount() const { return playerWoodCount; }
    bool getIsLit() const { return isLit; }

    bool getIsCooking() const { return isCooking; }
    float getCookingProgress() const { return cookingTimer / 5.0f; } // 假设5秒烤好

private:
    float posX, posY, posZ;
    int playerWoodCount;
    bool isLit;
    float burnTime;    // 剩余燃烧秒数
    float flicker;     // 当前光照强度抖动值

    bool isCooking = false;
    float cookingTimer = 0.0f;

    void drawStones();
    void drawLogs();
    void drawFlame();
};

extern CampfireSystem campfireSystem;

#endif#pragma once
