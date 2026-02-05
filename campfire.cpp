#include "campfire.h"
#include "player.h"
#include "shader.h"
#include "terrain.h"
#include <iostream>
#include <cmath>

CampfireSystem campfireSystem;

CampfireSystem::CampfireSystem()
    : posX(0), posY(0), posZ(0), playerWoodCount(0), isLit(false), burnTime(0), flicker(0) {
}

void CampfireSystem::init(float x, float y, float z) {
    posX = x; posY = y; posZ = z;
}

void CampfireSystem::update(float deltaTime) {
    if (isLit) {
        burnTime -= deltaTime;
        if (burnTime <= 0) { burnTime = 0; isLit = false; isCooking = false; }
        flicker = 1.5f + sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f) * 0.25f;

        // 烹饪逻辑
        if (isCooking) {
            cookingTimer += deltaTime;
            if (cookingTimer >= 5.0f) { // 5秒烤熟一条
                isCooking = false;
                cookingTimer = 0.0f;
                // 注意：这里需要外部逻辑或直接操作 player 对象
                extern Player player;
                if (player.rawFishCount > 0) {
                    player.rawFishCount--;
                    player.cookedFishCount++;
                    std::cout << "[Campfire] Fish is cooked! Raw: " << player.rawFishCount
                        << " Cooked: " << player.cookedFishCount << std::endl;
                }
            }
        }
    }
    else {
        flicker = 0.0f;
        isCooking = false;
    }
}

void CampfireSystem::interact(Player& player) {
    float dx = player.x - posX;
    float dz = player.z - posZ;
    float dist = sqrt(dx * dx + dz * dz);

    // 1. 火堆旁的逻辑 (添柴/烤鱼/吃鱼)
    if (dist < 60.0f) {
        if (player.cookedFishCount > 0) {
            player.cookedFishCount--;
            player.hunger = std::min(100.0f, player.hunger + 30.0f);
            player.health = std::min(100.0f, player.health + 5.0f);
            std::cout << "[Action] Ate cooked fish." << std::endl;
        }
        else if (!isCooking && player.rawFishCount > 0 && isLit) {
            isCooking = true;
            cookingTimer = 0.0f;
            std::cout << "[Campfire] Start cooking fish..." << std::endl;
        }
        else if (playerWoodCount > 0) {
            playerWoodCount--;
            isLit = true;
            burnTime += 60.0f;
            player.warmth += 10.0f;
            std::cout << "[Campfire] Added wood." << std::endl;
        }
    }
    // 2. 森林里的逻辑 (只有在树木周围按 E 才能捡木头)
    else if (dist > 100.0f) {
        // 调用地形系统的树木检测函数
        // 这里的检测半径设为 15.0f，比物理碰撞半径稍大一点，方便玩家交互
        extern class TerrainSystem terrainSystem;
        bool isNearTree = terrainSystem.checkTreeCollision(player.x, player.z, 15.0f);

        if (isNearTree) {
            if (player.hunger > 5.0f) {
                player.hunger -= 2.0f; // 采集木材消耗少量体力
                playerWoodCount++;
                std::cout << "[Inventory] Picked up a log from the tree. Total: " << playerWoodCount << std::endl;
            }
            else {
                std::cout << "[Status] Too hungry to gather wood!" << std::endl;
            }
        }
        else {
            std::cout << "[Action] You need to be closer to a tree to get wood." << std::endl;
        }
    }
}

void CampfireSystem::draw() {
    // 1. 强制关闭当前的 Shader，回到固定管线绘制火堆
    glUseProgram(0);

    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    glDisable(GL_TEXTURE_2D);

    // 绘制石头和木头（需要光照才有立体感）
    glEnable(GL_LIGHTING);
    drawStones();
    drawLogs();

    // 绘制火焰（不需要光照，它是自发光的）
    if (isLit) {
        glDisable(GL_LIGHTING);
        drawFlame();
        glEnable(GL_LIGHTING);
    }

    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void CampfireSystem::drawStones() {
    // 强制给石头一个深灰色材质
    GLfloat stone_diffuse[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    GLfloat stone_ambient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, stone_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, stone_diffuse);
    glColor3f(0.35f, 0.35f, 0.35f);

    // 增加石头数量到 14 块，围成一个更大的圆圈
    for (int i = 0; i < 14; i++) {
        glPushMatrix();
        glRotatef(i * (360.0f / 14.0f), 0, 1, 0); // 均匀分布

        // 1. 加大环绕半径：从 18 加大到 35
        glTranslatef(35, 0, 0);

        // 2. 添加随机感（利用 i 的余数模拟）
        float scale = 1.0f + (i % 3) * 0.2f; // 不同石头大小略有不同
        glScalef(scale, scale * 0.8f, scale); // 让石头稍微扁平一点，更像散落的岩石

        // 3. 加大石头尺寸：半径从 5 加大到 12
        glutSolidSphere(12, 10, 10);

        glPopMatrix();
    }
}

void CampfireSystem::drawLogs() {
    glColor3f(0.25f, 0.15f, 0.05f);
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glRotatef(i * 60, 0, 1, 0);
        glRotatef(35, 1, 0, 0);
        // 尺寸加大：从 4,4,18 加大到 6,6,30
        glScalef(6, 6, 30);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
}

void CampfireSystem::drawFlame() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // 加色混合，产生发光效果

    // 【关键】关闭深度缓冲区写入，防止火焰球体挡住里面的木头
    glDepthMask(GL_FALSE);

    float time = glutGet(GLUT_ELAPSED_TIME) * 0.002f;
    float s = flicker * 1.5f; // 缩放系数

    // 1. 最外层：大范围黄色光晕
    // 使用极低的 alpha (0.1) 避免形成明显的“蛋壳”
    glColor4f(1.0f, 0.8f, 0.0f, 0.1f);
    glPushMatrix();
    glScalef(s * 1.8f, s * 2.5f, s * 1.8f);
    glutSolidSphere(8, 15, 15);
    glPopMatrix();

    // 2. 中间层：明亮的动态火苗
    glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        float offset = sin(time * 4.0f + i) * 1.5f;
        glTranslatef(offset, 0, cos(time * 3.0f + i) * 1.5f);
        glScalef(s, s * 3.5f, s);
        glutSolidSphere(5, 10, 10);
        glPopMatrix();
    }

    // 3. 核心：亮白色/强黄核心
    glColor4f(1.0f, 1.0f, 0.8f, 0.8f);
    glPushMatrix();
    glScalef(s * 0.5f, s * 1.5f, s * 0.5f);
    glutSolidSphere(4, 10, 10);
    glPopMatrix();

    // 恢复深度写入和混合状态
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void CampfireSystem::setShaderUniforms(Shader* shader) {
    shader->use();
    shader->setVec3("campfirePos", posX, posY + 10.0f, posZ); // 光源点稍微调高一点
    shader->setVec3("campfireColor", 1.0f, 0.8f, 0.2f);     // 颜色调得更偏黄白（高温感）
    shader->setFloat("campfireIntensity", flicker * 2.0f);  // 强度倍增
}

float CampfireSystem::getDistToPlayer(Player& player) {
    float dx = player.x - posX;
    float dz = player.z - posZ;
    return sqrt(dx * dx + dz * dz);
}
