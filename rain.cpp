#include "rain.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// 全局下雨系统实例
RainSystem rainSystem;

// 构造函数
RainSystem::RainSystem() : isRaining(false) {
    srand(static_cast<unsigned>(time(0)));
}

// 析构函数
RainSystem::~RainSystem() {
    particles.clear();
}

// 初始化系统
void RainSystem::init(int maxParticles) {
    particles.reserve(maxParticles);
    isRaining = false;
}

// 创建新的雨滴粒子
void RainSystem::createRaindrop() {
    if (particles.size() >= particles.capacity()) {
        return;
    }

    RainParticle p;
    // 雨滴起始位置在场景上方
    p.x = static_cast<float>(rand() % 800);      // x: 0-800（场景坐标）
    p.y = 500.0f + static_cast<float>(rand() % 300);  // y: 500-800（更高处开始）
    p.z = static_cast<float>(rand() % 800);      // z: 0-800（场景坐标）

    // 基础下落速度 - 无风力影响
    p.vx = 0.0f;  // 无水平方向速度
    p.vy = -300.0f - static_cast<float>(rand() % 100);  // 下落速度到300-400
    p.vz = 0.0f;  // 无水平方向速度

    p.life = 2.0f;            // 延长生命周期
    p.size = 1.5f + static_cast<float>(rand() % 5) / 10.0f;  // 大小: 1.5-2.0
    p.active = true;
    p.type = 0;               // 雨滴类型

    particles.push_back(p);
}

// 创建水花粒子
void RainSystem::createSplash(float x, float y, float z) {
    // 控制水花粒子数量
    int splashCount = 8 + rand() % 4;  // 8-12个水花粒子

    for (int i = 0; i < splashCount; i++) {
        if (particles.size() >= particles.capacity()) {
            break;
        }

        RainParticle p;
        // 随机偏移范围设置
        p.x = x + (static_cast<float>(rand() % 20) - 10.0f); 
        p.y = y + 0.3f;  // 略高于地面
        p.z = z + (static_cast<float>(rand() % 20) - 10.0f);

        //水花溅射速度
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        float speed = 8.0f + static_cast<float>(rand() % 15);

        p.vx = cos(angle) * speed;
        p.vy = 15.0f + static_cast<float>(rand() % 20); //向上溅射速度
        p.vz = sin(angle) * speed;

        p.life = 0.5f + static_cast<float>(rand() % 20) / 100.0f;  //生命周期: 0.5-0.7秒
        p.size = 1.8f + static_cast<float>(rand() % 8) / 10.0f;  //减小大小: 1.8-2.6
        p.active = true;
        p.type = 1;           //水花类型

        particles.push_back(p);
    }
}

// 更新粒子系统
void RainSystem::update(float deltaTime) {
    if (!isRaining && particles.empty()) {
        return;
    }

    // 如果正在下雨，创建新的雨滴
    if (isRaining) {
        static float rainTimer = 0.0f;
        rainTimer += deltaTime;

        // 增加雨滴密度 - 更快地创建雨滴
        if (rainTimer > 0.002f) { 
            //一次创建多个雨滴来控制雨滴密度
            for (int i = 0; i < 5; i++) {  //每次创建5个雨滴
                createRaindrop();
            }
            rainTimer = 0.0f;
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
        p.life -= deltaTime * 1.0f;  // 正常速度衰减

        // 重力影响
        if (p.type == 1) {  //水花粒子受重力影响
            p.vy -= 120.0f * deltaTime;  //增加重力效果，使水花更快下落
        }

        // 检查雨滴是否落到地面
        if (p.type == 0 && p.y <= 15.0f) {  //地面高度为10，稍微提高检测点
            p.active = false;

            // 随机决定是否创建水花
            if (rand() % 100 < 70) {  //70%的几率创建水花
                createSplash(p.x, 15.0f, p.z);  //创建水花
            }
        }

        // 检查粒子是否死亡
        if (p.life <= 0.0f || p.y < -50.0f) {
            p.active = false;
        }

        // 限制在场景范围内
        if (p.x < -100 || p.x > 900 || p.z < -100 || p.z > 900) {
            p.active = false;
        }
    }

    // 移除不活跃的粒子
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const RainParticle& p) { return !p.active; }),
        particles.end()
    );
}

// 绘制粒子
void RainSystem::render() {
    if (!isRaining && particles.empty()) {
        return;
    }

    // 保存当前矩阵状态
    glPushMatrix();

    glDisable(GL_LIGHTING);      // 禁用光照
    glDisable(GL_TEXTURE_2D);    // 禁用纹理
    glEnable(GL_BLEND);          // 启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);       // 禁用深度写入

    // 设置更细的线宽用于绘制雨滴
    glLineWidth(1.0f);

    // 绘制雨滴-使用细长的线条
    glBegin(GL_LINES);
    for (const auto& p : particles) {
        if (!p.active || p.type != 0) continue;

        float alpha = p.life;
        if (alpha > 1.0f) alpha = 1.0f;
        if (alpha < 0.1f) alpha = 0.1f;  // 确保最低可见度

        // 雨滴颜色：更自然的蓝灰色
        glColor4f(0.6f, 0.8f, 0.9f, alpha * 0.8f);

        // 雨滴起点（顶部）
        float startX = p.x;
        float startY = p.y;
        float startZ = p.z;

        // 雨滴终点（底部）- 垂直下落
        float endX = p.x;  // 无水平偏移
        float endY = p.y - p.size * 25.0f;  // 增加长度系数
        float endZ = p.z;  // 无水平偏移

        // 绘制细长雨滴
        glVertex3f(startX, startY, startZ);
        glVertex3f(endX, endY, endZ);
    }
    glEnd();

    // 绘制水花 - 使用更小更透明的点
    glPointSize(2.5f);  // 减小点大小，使水花更细腻
    glBegin(GL_POINTS);
    for (const auto& p : particles) {
        if (!p.active || p.type != 1) continue;

        float alpha = p.life;
        if (alpha > 1.0f) alpha = 1.0f;
        if (alpha < 0.05f) alpha = 0.05f;  //提高最低可见度阈值

        // 水花颜色：更透明的白色，减小透明度
        float colorAlpha = alpha * 0.4f;  // 减小透明度系数
        glColor4f(0.9f, 0.95f, 1.0f, colorAlpha);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();

    // 只对少数水花添加短线效果
    glLineWidth(0.5f);
    glBegin(GL_LINES);
    for (const auto& p : particles) {
        if (!p.active || p.type != 1) continue;

        // 只对较大的水花粒子添加短线效果
        if (p.size > 2.2f && p.life > 0.3f) {
            float alpha = p.life * 0.3f;  // 线条更透明

            // 水花溅射的短线
            glColor4f(0.85f, 0.9f, 1.0f, alpha);

            // 绘制一条短线模拟溅射
            float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
            float length = p.size * 1.5f;  // 减小长度

            float endX = p.x + cos(angle) * length;
            float endY = p.y + sin(angle) * length * 0.3f;
            float endZ = p.z + sin(angle) * length;

            glVertex3f(p.x, p.y, p.z);
            glVertex3f(endX, endY, endZ);
        }
    }
    glEnd();

    // 恢复状态
    glDepthMask(GL_TRUE);        // 启用深度写入
    glDisable(GL_BLEND);         // 禁用混合
    glEnable(GL_LIGHTING);       // 启用光照
    glEnable(GL_TEXTURE_2D);     // 启用纹理

    // 恢复矩阵
    glPopMatrix();
}

// 切换下雨状态
void RainSystem::toggleRain() {
    isRaining = !isRaining;
    std::cout << "Rain toggled. Now isRaining: " << isRaining << std::endl;
    if (!isRaining) {
        clear();  // 停止下雨时清除所有粒子
    }
}

// 清除所有粒子
void RainSystem::clear() {
    particles.clear();
}
