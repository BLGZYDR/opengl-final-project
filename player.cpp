#include "headers.h" 
#include "player.h"
#include "terrain.h"
#include "campfire.h"
#include "weather.h"
#include "lighting.h" 
#include <cmath>
#include <algorithm> // 用于 std::max, std::min

Player player;

Player::Player() {
    // 调整位置：让你出生在房子前面，面向房子
    x = 350.0f;
    z = 150.0f;  // 房子在 z=-120，你站在 150 处看向负 Z 方向
    y = 60.0f;
    yaw = 0.0f;  // 面向北方（负Z）
    pitch = 0.0f;

    velocityY = 0.0f;
    vx = 0.0f; vz = 0.0f;
    isGrounded = false;

    // --- 初始化数值，状态条才会有颜色 ---
    health = 100.0f;
    hunger = 100.0f;
    warmth = 100.0f;
}

void Player::update(float deltaTime, bool keyW, bool keyS, bool keyA, bool keyD, bool keySpace) {
    // 1. 计算输入方向 (目标加速度方向)
    float inputX = 0.0f;
    float inputZ = 0.0f;
    float rad = yaw * PI / 180.0f;

    if (keyW) { inputX += sin(rad); inputZ -= cos(rad); }
    if (keyS) { inputX -= sin(rad); inputZ += cos(rad); }
    if (keyA) { inputX -= cos(rad); inputZ -= sin(rad); }
    if (keyD) { inputX += cos(rad); inputZ += sin(rad); }

    // 归一化输入向量
    if (inputX != 0 || inputZ != 0) {
        float len = sqrt(inputX * inputX + inputZ * inputZ);
        inputX /= len; inputZ /= len;
    }

    // 2. 应用加速度 (F = ma)
    if (inputX != 0 || inputZ != 0) {
        vx += inputX * ACCELERATION * deltaTime;
        vz += inputZ * ACCELERATION * deltaTime;
    }

    // 3. 应用摩擦力
    vx -= vx * FRICTION * deltaTime;
    vz -= vz * FRICTION * deltaTime;

    // 4. 限制最大速度
    float currentSpeed = sqrt(vx * vx + vz * vz);
    if (currentSpeed > MAX_SPEED) {
        float scale = MAX_SPEED / currentSpeed;
        vx *= scale;
        vz *= scale;
    }

    // 如果速度极小，强制归零，防止抖动
    if (currentSpeed < 0.1f && inputX == 0 && inputZ == 0) {
        vx = 0; vz = 0;
    }

    // 5. 计算位移并进行碰撞检测
    float dx = vx * deltaTime;
    float dz = vz * deltaTime;

    // X轴尝试移动
    if (!checkCollisions(x + dx, z)) {
        x += dx;
    }
    else {
        vx = 0; // 撞墙后 X 方向速度归零
    }

    // Z轴尝试移动
    if (!checkCollisions(x, z + dz)) {
        z += dz;
    }
    else {
        vz = 0; // 撞墙后 Z 方向速度归零
    }

    // 6. 处理跳跃 (垂直物理)
    if (keySpace && isGrounded) {
        velocityY = JUMP_FORCE;
        isGrounded = false;
    }

    velocityY += GRAVITY; // 应用重力
    y += velocityY;

    // 7. 地形贴合
    float terrainHeight = terrainSystem.getHeight(x, z);
    float groundLevel = terrainHeight + PLAYER_HEIGHT;

    if (y < groundLevel) {
        y = groundLevel;
        velocityY = 0.0f;
        isGrounded = true;
    }
    else {
        isGrounded = false;
    }
}

// 【更新】新的房子碰撞区域 (x-50, z-60)
bool Player::checkCollisions(float newX, float newZ) {
    // 新房子碰撞区域：x: 255~445, z: -75~115
    if (newX > 255.0f && newX < 445.0f && newZ > -185.0f && newZ < 65.0f) return true;

    // 树木碰撞检测保持不变
    if (terrainSystem.checkTreeCollision(newX, newZ, COLLISION_RADIUS)) return true;
    
    return false;
}

void Player::setCamera() {
    float radYaw = yaw * PI / 180.0f;
    float radPitch = pitch * PI / 180.0f;
    float lookX = x + sin(radYaw) * cos(radPitch);
    float lookY = y + sin(radPitch);
    float lookZ = z - cos(radYaw) * cos(radPitch);
    gluLookAt(x, y, z, lookX, lookY, lookZ, 0.0f, 1.0f, 0.0f);
}

void Player::updateSurvivalStats(float deltaTime) {
    // 1. 饱食度自然下降 (每10分钟掉光)
    hunger -= 0.15f * deltaTime;
    if (hunger < 0) hunger = 0;

    // 2. 体温逻辑
    float distToFire = campfireSystem.getDistToPlayer(*this);
    bool isNearFire = (distToFire < 100.0f && campfireSystem.getIsLit());

    if (isNearFire) {
        warmth += 5.0f * deltaTime; // 靠近火堆快速回暖
    }
    else {
        // 环境寒冷扣除
        float coldRate = 0.5f;
        if (!lightingSystem.isDaytime()) coldRate += 0.5f; // 夜晚更冷
        if (weatherSystem.isSnowing) coldRate += 1.5f;    // 下雪极冷
        warmth -= coldRate * deltaTime;
    }
    warmth = std::max(0.0f, std::min(100.0f, warmth));

    // 3. 生命值逻辑
    if (hunger <= 0 || warmth <= 0) {
        health -= 1.0f * deltaTime; // 饥饿或寒冷扣血
    }
    else if (hunger > 70 && warmth > 70) {
        health += 0.5f * deltaTime; // 状态良好回血
    }
    health = std::max(0.0f, std::min(100.0f, health));
}
