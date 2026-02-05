// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "headers.h"

class Player {
public:
    float x, y, z;
    float yaw, pitch;
    float vx, vz;
    float velocityY;
    bool isGrounded;

    // --- 必须在这里声明，红线才会消失 ---
    float health;
    float hunger;
    float warmth;

    int rawFishCount = 0;    // 生鱼数量
    int cookedFishCount = 0; // 熟鱼数量

    const float PLAYER_HEIGHT = 46.0f;
    const float MAX_SPEED = 700.0f;     // 建议恢复到一个合理值，原代码10000太快了
    const float ACCELERATION = 1500.0f;
    const float FRICTION = 8.0f;
    const float JUMP_FORCE = 8.0f;
    const float GRAVITY = -0.6f;
    const float COLLISION_RADIUS = 5.0f;

    Player();
    void updateSurvivalStats(float deltaTime);
    void update(float deltaTime, bool keyW, bool keyS, bool keyA, bool keyD, bool keySpace);
    void setCamera();

private:
    bool checkCollisions(float newX, float newZ);
};

extern Player player;

#endif
