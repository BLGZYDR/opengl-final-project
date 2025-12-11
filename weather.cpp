#include "weather.h"
#include"geometry.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// 全局天气系统实例
WeatherSystem weatherSystem;

// 构造函数
WeatherSystem::WeatherSystem() : isRaining(false), isSnowing(false), snowAccumulation(0.0f) {
    srand(static_cast<unsigned>(time(0)));
}

// 析构函数
WeatherSystem::~WeatherSystem() {
    particles.clear();
}

// 初始化系统
void WeatherSystem::init(int maxParticles) {
    particles.reserve(maxParticles);
    isRaining = false;
    isSnowing = false;
    snowAccumulation = 0.0f;
}

// 创建新的雨滴粒子
void WeatherSystem::createRaindrop() {
    if (particles.size() >= particles.capacity()) {
        return;
    }

    WeatherParticle p;
    p.x = static_cast<float>(rand() % 800);
    p.y = 500.0f + static_cast<float>(rand() % 300);
    p.z = static_cast<float>(rand() % 800);

    p.vx = 0.0f;
    p.vy = -300.0f - static_cast<float>(rand() % 100);
    p.vz = 0.0f;

    p.life = 2.0f;
    p.size = 1.5f + static_cast<float>(rand() % 5) / 10.0f;
    p.active = true;
    p.type = 0;

    particles.push_back(p);
}

// 创建新的雪花粒子 - 修复：确保雪花能落到地面
void WeatherSystem::createSnowflake() {
    if (particles.size() >= particles.capacity()) {
        return;
    }

    WeatherParticle p;
    // 雪花在场景上方更宽的范围生成
    p.x = static_cast<float>(rand() % 1200) - 200.0f;  // x: -200 到 1000
    p.y = 600.0f + static_cast<float>(rand() % 300);   // 提高起始高度：800-1400
    p.z = static_cast<float>(rand() % 1200) - 200.0f;  // z: -200 到 1000

    // 雪花缓慢飘落，有轻微的水平飘动
    p.vx = (static_cast<float>(rand() % 100) - 50.0f) * 0.3f;  // 减小水平速度
    p.vy = -80.0f - static_cast<float>(rand() % 30);           // 减慢下落速度
    p.vz = (static_cast<float>(rand() % 100) - 50.0f) * 0.3f;  // 减小水平速度

    p.life = 20.0f;  // 大大延长雪花生命周期，确保能落到地面
    p.size = 2.5f + static_cast<float>(rand() % 20) / 10.0f;  // 雪花大小：2.5-4.5
    p.active = true;
    p.type = 2;  // 雪花类型

    particles.push_back(p);
}

// 创建水花粒子
void WeatherSystem::createSplash(float x, float y, float z) {
    int splashCount = 6 + rand() % 4;

    for (int i = 0; i < splashCount; i++) {
        if (particles.size() >= particles.capacity()) {
            break;
        }

        WeatherParticle p;
        p.x = x + (static_cast<float>(rand() % 20) - 10.0f);
        p.y = y + 0.3f;
        p.z = z + (static_cast<float>(rand() % 20) - 10.0f);

        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        float speed = 8.0f + static_cast<float>(rand() % 15);

        p.vx = cos(angle) * speed;
        p.vy = 15.0f + static_cast<float>(rand() % 20);
        p.vz = sin(angle) * speed;

        p.life = 0.5f + static_cast<float>(rand() % 20) / 100.0f;
        p.size = 1.8f + static_cast<float>(rand() % 8) / 10.0f;
        p.active = true;
        p.type = 1;

        particles.push_back(p);
    }
}

// 更新粒子系统
void WeatherSystem::update(float deltaTime) {
    // 更新积雪厚度
    if (isSnowing) {
        // 缓慢增加积雪厚度，最大到10个单位
        snowAccumulation += deltaTime * 0.5f;
        if (snowAccumulation > 10.0f) snowAccumulation = 10.0f;
    }
    else if (snowAccumulation > 0.0f) {
        // 不下雪时积雪缓慢融化
        snowAccumulation -= deltaTime * 0.1f;
        if (snowAccumulation < 0.0f) snowAccumulation = 0.0f;
    }

    // 如果正在下雨，创建新的雨滴
    if (isRaining) {
        static float rainTimer = 0.0f;
        rainTimer += deltaTime;

        if (rainTimer > 0.002f) {
            for (int i = 0; i < 3; i++) {
                createRaindrop();
            }
            rainTimer = 0.0f;
        }
    }

    // 如果正在下雪，创建新的雪花
    if (isSnowing) {
        static float snowTimer = 0.0f;
        snowTimer += deltaTime;

        if (snowTimer > 0.1f) {  // 雪花生成频率较低
            for (int i = 0; i < 2; i++) {
                createSnowflake();
            }
            snowTimer = 0.0f;
        }
    }

    // 更新所有粒子
    for (auto& p : particles) {
        if (!p.active) continue;

        // 更新位置
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
        p.z += p.vz * deltaTime;

        // 更新生命周期
        p.life -= deltaTime;

        // 重力影响
        if (p.type == 1) {  // 水花粒子
            p.vy -= 120.0f * deltaTime;
        }

        // 雪花轻微旋转飘动效果
        if (p.type == 2) {
            p.vx += (static_cast<float>(rand() % 100) - 50.0f) * 0.01f;
            p.vz += (static_cast<float>(rand() % 100) - 50.0f) * 0.01f;
        }

        // 检查雨滴是否落到地面
        if (p.type == 0 && p.y <= 15.0f) {
            p.active = false;

            if (rand() % 100 < 70) {
                createSplash(p.x, 15.0f, p.z);
            }
        }

        // 检查雪花是否落到地面或屋顶
        if (p.type == 2 && p.y <= 15.0f) {
            p.active = false;
        }

        // 检查粒子是否死亡
        if (p.life <= 0.0f || p.y < -50.0f) {
            p.active = false;
        }

        // 限制在场景范围内
        if (p.x < -200 || p.x > 1000 || p.z < -200 || p.z > 1000) {
            p.active = false;
        }
    }

    // 移除不活跃的粒子
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const WeatherParticle& p) { return !p.active; }),
        particles.end()
    );
}

// 绘制粒子
void WeatherSystem::renderRainAccumulation() {
    if ((!isRaining && !isSnowing) && particles.empty()) {
        return;
    }

    // 保存当前矩阵状态
    glPushMatrix();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    // 绘制雨滴
    if (isRaining || (!isSnowing && particles.size() > 0)) {
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (const auto& p : particles) {
            if (!p.active || p.type != 0) continue;

            float alpha = p.life;
            if (alpha > 1.0f) alpha = 1.0f;
            if (alpha < 0.1f) alpha = 0.1f;

            glColor4f(0.6f, 0.8f, 0.9f, alpha * 0.8f);

            float startX = p.x;
            float startY = p.y;
            float startZ = p.z;

            float endX = p.x;
            float endY = p.y - p.size * 25.0f;
            float endZ = p.z;

            glVertex3f(startX, startY, startZ);
            glVertex3f(endX, endY, endZ);
        }
        glEnd();
    }

    // 绘制水花
    glPointSize(2.5f);
    glBegin(GL_POINTS);
    for (const auto& p : particles) {
        if (!p.active || p.type != 1) continue;

        float alpha = p.life;
        if (alpha > 1.0f) alpha = 1.0f;
        if (alpha < 0.05f) alpha = 0.05f;

        float colorAlpha = alpha * 0.4f;
        glColor4f(0.9f, 0.95f, 1.0f, colorAlpha);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();

    // 绘制雪花
    if (isSnowing || (!isRaining && particles.size() > 0)) {
        glPointSize(4.0f);  // 雪花使用更大的点
        glBegin(GL_POINTS);
        for (const auto& p : particles) {
            if (!p.active || p.type != 2) continue;

            float alpha = p.life;
            if (alpha > 1.0f) alpha = 1.0f;
            if (alpha < 0.2f) alpha = 0.2f;

            // 雪花颜色：纯白色带轻微透明度
            glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.9f);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();

        // 为较大的雪花添加星形效果
        glLineWidth(0.8f);
        glBegin(GL_LINES);
        for (const auto& p : particles) {
            if (!p.active || p.type != 2) continue;

            if (p.size > 3.5f) {
                float alpha = p.life * 0.7f;
                glColor4f(1.0f, 1.0f, 1.0f, alpha);

                // 绘制雪花的星形图案
                for (int i = 0; i < 6; i++) {
                    float angle = static_cast<float>(i) * 3.14159f / 3.0f;
                    float length = p.size * 1.5f;

                    float endX = p.x + cos(angle) * length;
                    float endY = p.y;
                    float endZ = p.z + sin(angle) * length;

                    glVertex3f(p.x, p.y, p.z);
                    glVertex3f(endX, endY, endZ);
                }
            }
        }
        glEnd();
    }

    // 只对少数水花添加短线效果
    glLineWidth(0.5f);
    glBegin(GL_LINES);
    for (const auto& p : particles) {
        if (!p.active || p.type != 1) continue;

        if (p.size > 2.2f && p.life > 0.3f) {
            float alpha = p.life * 0.3f;
            glColor4f(0.85f, 0.9f, 1.0f, alpha);

            float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
            float length = p.size * 1.5f;

            float endX = p.x + cos(angle) * length;
            float endY = p.y + sin(angle) * length * 0.3f;
            float endZ = p.z + sin(angle) * length;

            glVertex3f(p.x, p.y, p.z);
            glVertex3f(endX, endY, endZ);
        }
    }
    glEnd();

    // 恢复状态
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // 恢复矩阵
    glPopMatrix();
}

// 绘制积雪效果
void WeatherSystem::renderSnowAccumulation() {
    if (snowAccumulation <= 0.0f) {
        return;
    }

    // 保存当前矩阵状态
    glPushMatrix();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 绘制森林地面的积雪
    glColor4f(0.95f, 0.95f, 1.0f, 0.9f);
    cons1(0, 10, 0, 800, 1, 800);
    build();

    // 绘制房屋地基上的积雪
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 10.0f + snowAccumulation * 0.1f, -1.0f);
    glVertex3f(801.0f, 10.0f + snowAccumulation * 0.1f, -1.0f);
    glVertex3f(801.0f, 10.0f + snowAccumulation * 0.1f, 801.0f);
    glVertex3f(-1.0f, 10.0f + snowAccumulation * 0.1f, 801.0f);
    glEnd();

    // 绘制房屋屋顶的积雪（三角形屋顶）
    glColor4f(0.98f, 0.98f, 1.0f, 0.95f);
    // 屋顶左侧
    cons3(400, 185, 400, 180, 310, 95, 310, 90, 95, 180);
    build3();
    // 屋顶右侧
    cons3(400, 185, 400, 180, 490, 95, 490, 90, 95, 180);
    build3();

    // 绘制烟囱顶部的积雪
    glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
    // 烟囱顶部
    cons1(355, 145, 165, -30, 5, 30);
    build();
    //绘制棚子上的积雪
    cons3(400, 95, 400, 100, 370, 65, 370, 70, 270, 70);
    build3();
    cons3(400, 95, 400, 100, 430, 65, 430, 70, 270, 70);
    build3();

    

    // 恢复状态
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // 恢复矩阵
    glPopMatrix();
}

// 切换下雨状态
void WeatherSystem::toggleRain() {
    isRaining = !isRaining;
    if (isRaining) {
        isSnowing = false;  // 下雨时停止下雪
        clearSnow();        // 清除积雪
    }
    std::cout << "Rain toggled. Now isRaining: " << isRaining << std::endl;
}

// 切换下雪状态
void WeatherSystem::toggleSnow() {
    isSnowing = !isSnowing;
    if (isSnowing) {
        isRaining = false;  // 下雪时停止下雨
        clear();            // 清除雨滴粒子
    }
    else {
        clearSnow();        // 停止下雪时清除积雪
    }
    std::cout << "Snow toggled. Now isSnowing: " << isSnowing << std::endl;
}

// 清除所有粒子
void WeatherSystem::clear() {
    particles.clear();
}

// 清除积雪
void WeatherSystem::clearSnow() {
    snowAccumulation = 0.0f;
}
