#include "weather.h"
#include "geometry.h"
#include "terrain.h" 
#include "lighting.h" 
#include "texture.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm> 
extern LightingSystem lightingSystem;

WeatherSystem weatherSystem;

WeatherSystem::WeatherSystem() : isRaining(false), isSnowing(false), snowAccumulation(0.0f) {
    srand(static_cast<unsigned>(time(0)));
}
void WeatherSystem::drawSnowOnHouse() {}

WeatherSystem::~WeatherSystem() {
    particles.clear();
    smokeParticles.clear();
}

void WeatherSystem::init(int maxParticles) {
    if (maxParticles < 50000) maxParticles = 50000;
    particles.reserve(maxParticles);
    isRaining = false;
    isSnowing = false;
    snowAccumulation = 0.0f;
    smokeParticles.clear();
}

// 创建新的雨滴粒子
void WeatherSystem::createRaindrop() {
    if (particles.size() >= particles.capacity()) return;

    WeatherParticle p;
    // 【改进】扩大生成范围：-6000 ~ 6000
    p.x = (rand() % 12000) - 6000.0f;
    p.y = 800.0f;
    p.z = (rand() % 12000) - 6000.0f;

    p.vx = 0.0f;
    p.vy = -300.0f - static_cast<float>(rand() % 150);
    p.vz = 0.0f;
    p.life = 3.0f;
    p.size = 1.5f + static_cast<float>(rand() % 5) / 10.0f;
    p.active = true;
    p.type = 0;
    particles.push_back(p);
}

// 创建新的雪花粒子
void WeatherSystem::createSnowflake() {
    if (particles.size() >= particles.capacity()) return;

    WeatherParticle p;
    // 【改进】扩大生成范围：-6000 ~ 6000
    p.x = (rand() % 12000) - 6000.0f;
    p.y = 800.0f + static_cast<float>(rand() % 300);
    p.z = (rand() % 12000) - 6000.0f;

    p.vx = (static_cast<float>(rand() % 100) - 50.0f) * 0.5f;
    p.vy = -60.0f - static_cast<float>(rand() % 30);
    p.vz = (static_cast<float>(rand() % 100) - 50.0f) * 0.5f;
    p.life = 10.0f;
    p.size = 3.0f + static_cast<float>(rand() % 20) / 10.0f;
    p.active = true;
    p.type = 2;
    particles.push_back(p);
}

void WeatherSystem::createSplash(float x, float y, float z) {
    int splashCount = 8 + rand() % 8;
    for (int i = 0; i < splashCount; i++) {
        if (particles.size() >= particles.capacity()) break;
        WeatherParticle p;
        p.x = x + (static_cast<float>(rand() % 20) - 10.0f);
        p.y = y + 0.5f;
        p.z = z + (static_cast<float>(rand() % 20) - 10.0f);
        float angle = static_cast<float>(rand() % 360);
        float speed = 10.0f + static_cast<float>(rand() % 20);
        p.vx = cos(angle) * speed;
        p.vy = 10.0f + static_cast<float>(rand() % 20);
        p.vz = sin(angle) * speed;
        p.life = 0.4f;
        p.size = 1.0f;
        p.active = true;
        p.type = 1;
        particles.push_back(p);
    }
}

void WeatherSystem::update(float deltaTime) {
    updateSmoke(deltaTime);
    if (isSnowing) {
        snowAccumulation += deltaTime * 0.2f;
        if (snowAccumulation > 10.0f) snowAccumulation = 10.0f;
    }
    else if (snowAccumulation > 0.0f) {
        snowAccumulation -= deltaTime * 0.1f;
        if (snowAccumulation < 0.0f) snowAccumulation = 0.0f;
    }

    if (isRaining) {
        static float rainTimer = 0.0f;
        rainTimer += deltaTime;
        if (rainTimer > 0.0005f) {
            for (int i = 0; i < 200; i++) createRaindrop();
            rainTimer = 0.0f;
        }
    }

    if (isSnowing) {
        static float snowTimer = 0.0f;
        snowTimer += deltaTime;
        if (snowTimer > 0.05f) {
            for (int i = 0; i < 50; i++) createSnowflake();
            snowTimer = 0.0f;
        }
    }

    for (auto& p : particles) {
        if (!p.active) continue;
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
        p.z += p.vz * deltaTime;
        p.life -= deltaTime;
        if (p.type == 1) p.vy -= 120.0f * deltaTime;

        if (p.type == 0 && p.y <= 10.0f) {
            p.active = false;
            if (abs(p.x) < 800 && abs(p.z) < 800 && rand() % 100 < 50) {
                createSplash(p.x, 10.0f, p.z);
            }
        }
        if (p.type == 2 && p.y <= 10.0f) p.active = false;
        if (p.life <= 0.0f || p.y < -50.0f) p.active = false;
        if (p.x < -2500 || p.x > 2500 || p.z < -2500 || p.z > 2500) p.active = false;
    }

    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const WeatherParticle& p) { return !p.active; }), particles.end());
}

void WeatherSystem::updateSmoke(float deltaTime) {
    static float smokeTimer = 0.0f;
    smokeTimer += deltaTime;

    if (smokeTimer > 0.05f) {
        smokeTimer = 0.0f;
        if (smokeParticles.size() < 300) {
            SmokeParticle p;
            float chimneyX = 290;
            float chimneyZ = 20;

            p.x = chimneyX + (rand() % 10 - 5) / 2.0f;
            p.z = chimneyZ + (rand() % 10 - 5) / 2.0f;
            p.y = 185;

            p.vy = 15.0f + (rand() % 50) / 10.0f;
            p.size = 2.0f;
            p.life = 2.5f;
            p.offsetX = (rand() % 20 - 10) / 5.0f;
            p.offsetZ = (rand() % 20 - 10) / 5.0f;
            smokeParticles.push_back(p);
        }
    }

    for (auto& p : smokeParticles) {
        p.y += p.vy * deltaTime;
        p.x += p.offsetX * deltaTime;
        p.z += p.offsetZ * deltaTime;
        p.size += 8.0f * deltaTime;
        p.life -= 0.4f * deltaTime;
    }
    smokeParticles.erase(std::remove_if(smokeParticles.begin(), smokeParticles.end(),
        [](const SmokeParticle& p) { return p.life <= 0.0f; }), smokeParticles.end());
}

void WeatherSystem::drawSmoke() {
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto& p : smokeParticles) {
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glColor4f(0.9f, 0.9f, 0.9f, p.life * 0.01f);
        glutSolidSphere(p.size, 8, 8);
        glPopMatrix();
    }
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}


void WeatherSystem::draw() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // 1. 绘制雨滴（线）
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (const auto& p : particles) {
        if (!p.active || p.type != 0) continue;
        glColor4f(0.7f, 0.8f, 1.0f, 0.6f);
        glVertex3f(p.x, p.y, p.z);
        glVertex3f(p.x + p.vx * 0.05f, p.y + p.vy * 0.05f, p.z + p.vz * 0.05f);
    }
    glEnd();

    // 2. 绘制雪花（点）
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (const auto& p : particles) {
        if (!p.active || p.type != 2) continue;
        glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();

    // 3. 绘制水花（点）
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (const auto& p : particles) {
        if (!p.active || p.type != 1) continue;
        glColor4f(0.8f, 0.9f, 1.0f, 0.5f);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();

    // 【新增】如果正在下雪，绘制房子上的积雪
    if (isSnowing) {
        drawSnowOnHouse();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPointSize(1.0f);
}


void WeatherSystem::clearSnow() {
    snowAccumulation = 0.0f;
}

extern void startSound(const std::string& alias);
extern void pauseSound(const std::string& alias);

void WeatherSystem::toggleRain() {
    isRaining = !isRaining;
    if (isRaining) {
        isSnowing = false;
        pauseSound("snow_sfx");    // 关雪声
        pauseSound("forest_bgm");  // 关背景音
        clear();
        startSound("rain_sfx");    // 开雨声循环
    }
    else {
        pauseSound("rain_sfx");    // 关雨声
        startSound("forest_bgm");  // 恢复背景音循环
    }
}

void WeatherSystem::toggleSnow() {
    isSnowing = !isSnowing;
    if (isSnowing) {
        isRaining = false;
        pauseSound("rain_sfx");
        pauseSound("forest_bgm");
        clear();
        startSound("snow_sfx");    // 开雪声循环
    }
    else {
        pauseSound("snow_sfx");
        startSound("forest_bgm");
    }
}

void WeatherSystem::clear() {
    particles.clear();
    // 停止天气音效并恢复BGM
    pauseSound("rain_sfx");
    pauseSound("snow_sfx");
    startSound("forest_bgm");
}

void WeatherSystem::renderSnowAccumulation() {
    if (snowAccumulation <= 0.001f) return;

    glPushMatrix();

    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 计算当前的透明度 (假设 snowAccumulation 最大值是 10.0)
    // 请检查这里：如果最大值是10，除以10才能得到 0.0-1.0 的范围
    float alpha = snowAccumulation / 10.0f;
    if (alpha > 0.95f) alpha = 0.95f;
    if (alpha < 0.0f) alpha = 0.0f;

    const GLfloat* currentAmbient = lightingSystem.getGlobalAmbient();
    float baseBrightness = (currentAmbient[0] + currentAmbient[1] + currentAmbient[2]) / 3.0f;
    float brightnessFactor = std::max(0.2f, baseBrightness);

    GLfloat mat_ambient[4], mat_diffuse[4], mat_specular[4];
    GLfloat shininess;

    bool isDay = lightingSystem.isDaytime();
    if (isDay) {
        // 白天材质
        mat_ambient[0] = 0.8f * brightnessFactor;
        mat_ambient[1] = 0.85f * brightnessFactor;
        mat_ambient[2] = 0.9f * brightnessFactor;

        mat_diffuse[0] = 1.0f * brightnessFactor;
        mat_diffuse[1] = 1.0f * brightnessFactor;
        mat_diffuse[2] = 1.0f * brightnessFactor;

        mat_specular[0] = 0.8f; mat_specular[1] = 0.8f; mat_specular[2] = 0.9f;
        shininess = 100.0f;
    }
    else {
        // 夜晚材质
        float nightFactor = 0.3f + 0.7f * brightnessFactor;
        mat_ambient[0] = 0.4f * nightFactor;
        mat_ambient[1] = 0.5f * nightFactor;
        mat_ambient[2] = 0.6f * nightFactor;

        mat_diffuse[0] = 0.6f * nightFactor;
        mat_diffuse[1] = 0.7f * nightFactor;
        mat_diffuse[2] = 0.8f * nightFactor;

        mat_specular[0] = 0.3f; mat_specular[1] = 0.3f; mat_specular[2] = 0.4f;
        shininess = 30.0f;
    }

    // 【关键修复 1】：将计算出的 alpha 赋给材质的第四个分量
    mat_ambient[3] = alpha;
    mat_diffuse[3] = alpha;
    mat_specular[3] = alpha;

    // 设置材质
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    // --- 【修改 1：对齐对齐逻辑】 ---
    // 房屋中心在 (350, -120)，必须查询该点的地形高度
    float houseY = terrainSystem.getHeight(350.0f, -120.0f);

    // 平移必须与 scene.cpp 绘制房屋的代码完全同步！
    // 如果 scene.cpp 是 glTranslatef(0, houseY - 0.5f, -120.0f); 
    // 那么这里也必须减去 0.5f
    glTranslatef(0, houseY - 50.5f, -120.0f);

    glPushMatrix();
    // 这里的 0.2f 是积雪相对于房顶的偏移。
    // 如果你觉得积雪“悬空”，将 0.2f 调小，例如 0.02f，使其贴紧房顶
    glTranslatef(0.0f, 0.02f, 0.0f);

    // 绘制几何体 (保持原样)
    cons3(350, 220, 350, 230, 260, 130, 260, 140, -65, 180); build3();
    cons3(350, 220, 350, 230, 440, 130, 440, 140, -65, 180); build3();
    cons1(305, 185, 5, -30, 5, 30); build();
    cons3(350, 135, 350, 140, 320, 110, 320, 105, 110, 70); build3();
    cons3(350, 135, 350, 140, 380, 110, 380, 105, 110, 70); build3();

    glPopMatrix();

    glDisable(GL_BLEND);
    glPopMatrix();

    // 恢复状态
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();

    // 恢复默认材质（防止影响其他物体）
    GLfloat default_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat default_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
}
