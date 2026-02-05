#include "lighting.h"
#include "weather.h"
#include "shader.h"
#include "player.h" 
#include <algorithm> 
#include <iterator> 
#include <cmath> // 确保包含数学库

LightingSystem lightingSystem;

LightingSystem::LightingSystem() {
    dayTime = 12.0f;
    timeSpeed = 0.2f;
    sunAngle = 0.0f;
    sunIntensity = 1.0f;
    moonIntensity = 0.0f;

    for (int i = 0; i < 4; i++) {
        sunPos[i] = 0.0f;
        moonPos[i] = 0.0f;
        globalAmbient[i] = 0.0f;
        sunColor[i] = 0.0f;
        moonColor[i] = 0.0f;
        skyColor[i] = 0.0f;
        adjustedSunColor[i] = 0.0f;
        adjustedMoonColor[i] = 0.0f;
    }
}

void LightingSystem::init() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

void LightingSystem::lerpColor(GLfloat* out, const GLfloat* a, const GLfloat* b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    out[0] = a[0] + (b[0] - a[0]) * t;
    out[1] = a[1] + (b[1] - a[1]) * t;
    out[2] = a[2] + (b[2] - a[2]) * t;
    out[3] = a[3] + (b[3] - a[3]) * t;
}

// 【新增】计算光源强度
void LightingSystem::calculateLightIntensities() {
    // 根据太阳/月亮位置计算强度
    // 太阳：位置越高，强度越大
    float sunYFactor = (sunPos[1] + 200.0f) / 400.0f; // 假设-200到200的Y范围
    sunIntensity = std::max(0.0f, std::min(1.0f, sunYFactor));

    // 月亮：夜晚时强度增加
    float moonYFactor = (moonPos[1] + 200.0f) / 400.0f;
    moonIntensity = std::max(0.0f, std::min(1.0f, moonYFactor));

    // 调整光源颜色（应用强度）
    for (int i = 0; i < 3; i++) {
        adjustedSunColor[i] = sunColor[i] * sunIntensity;
        adjustedMoonColor[i] = moonColor[i] * moonIntensity;
    }
    adjustedSunColor[3] = sunColor[3];
    adjustedMoonColor[3] = moonColor[3];
}

void LightingSystem::update(float deltaTime) {
    // 保存前一个状态用于平滑过渡
    static bool firstFrame = true;
    if (firstFrame) {
        memcpy(prevSunPos, sunPos, sizeof(GLfloat) * 4);
        memcpy(prevMoonPos, moonPos, sizeof(GLfloat) * 4);
        memcpy(prevGlobalAmbient, globalAmbient, sizeof(GLfloat) * 4);
        memcpy(prevSunColor, sunColor, sizeof(GLfloat) * 4);
        memcpy(prevMoonColor, moonColor, sizeof(GLfloat) * 4);
        memcpy(prevSkyColor, skyColor, sizeof(GLfloat) * 4);
        prevDayTime = dayTime;
        firstFrame = false;
    }

    // 检查是否需要开始过渡
    float newDayTime = dayTime + deltaTime * timeSpeed;
    if (newDayTime >= 24.0f) newDayTime -= 24.0f;

    // 检测时间变化是否跨越了关键时间点（黎明、黄昏）
    bool shouldTransition = false;
    float keyTimes[] = { 5.0f, 7.0f, 17.0f, 19.0f, 4.0f, 6.0f };
    for (float keyTime : keyTimes) {
        if ((prevDayTime < keyTime && newDayTime >= keyTime) ||
            (prevDayTime > keyTime && newDayTime < keyTime)) {
            shouldTransition = true;
            break;
        }
    }

    if (shouldTransition && !isTransitioning) {
        isTransitioning = true;
        transitionTime = 0.0f;

        // 保存当前状态作为过渡的起点
        memcpy(prevSunPos, sunPos, sizeof(GLfloat) * 4);
        memcpy(prevMoonPos, moonPos, sizeof(GLfloat) * 4);
        memcpy(prevGlobalAmbient, globalAmbient, sizeof(GLfloat) * 4);
        memcpy(prevSunColor, sunColor, sizeof(GLfloat) * 4);
        memcpy(prevMoonColor, moonColor, sizeof(GLfloat) * 4);
        memcpy(prevSkyColor, skyColor, sizeof(GLfloat) * 4);
        prevDayTime = dayTime;
    }

    // 更新时间
    prevDayTime = dayTime;
    dayTime = newDayTime;

    // 计算角度
    float angle = (dayTime - 6.0f) * (3.14159f / 12.0f);
    sunAngle = angle;

    // 计算新的目标光源位置 (设置在 4500.0f 防止穿模)
    float radius = 4500.0f;
    GLfloat targetSunPos[4] = {
        cos(angle) * radius,
        sin(angle) * radius,
        0.0f,
        0.0f
    };

    GLfloat targetMoonPos[4] = {
        -targetSunPos[0],
        -targetSunPos[1],
        -targetSunPos[2],
        0.0f
    };

    // --- 颜色定义 ---
    const GLfloat colNight[] = { 0.08f, 0.08f, 0.1f, 1.0f };    // 【修改】暗灰色深夜天空
    const GLfloat colBlueSky[] = { 0.53f, 0.81f, 0.92f, 1.0f }; // 白天蓝色
    const GLfloat colSunset[] = { 0.35f, 0.15f, 0.1f, 1.0f };   // 【修改】调暗黄昏，使其更自然过渡到灰色
    const GLfloat colSnowSky[] = { 0.6f, 0.65f, 0.7f, 1.0f };

    const GLfloat lightDay[] = { 1.0f, 0.95f, 0.9f, 1.0f };
    const GLfloat lightSunset[] = { 0.6f, 0.3f, 0.1f, 1.0f };
    const GLfloat lightNight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat moonLight[] = { 0.08f, 0.08f, 0.12f, 1.0f };  // 【修改】更暗的月光，减少发光感

    // 计算目标颜色
    GLfloat targetSunColor[4] = { 0, 0, 0, 1 };
    GLfloat targetMoonColor[4] = { 0, 0, 0, 1 };
    GLfloat targetGlobalAmbient[4] = { 0, 0, 0, 1 };
    GLfloat targetSkyColor[4] = { 0, 0, 0, 1 };

    if (dayTime >= 5.0f && dayTime < 19.0f) {
        if (dayTime < 7.0f) {
            float t = (dayTime - 5.0f) / 2.0f;
            lerpColor(targetSkyColor, colNight, colBlueSky, t);
            lerpColor(targetSunColor, lightSunset, lightDay, t);
            GLfloat ambDay[] = { 0.4f, 0.4f, 0.4f, 1.0f };
            GLfloat ambNight[] = { 0.05f, 0.05f, 0.06f, 1.0f }; // 【修改】深夜环境光设为极暗灰
            lerpColor(targetGlobalAmbient, ambNight, ambDay, t);
            targetMoonColor[0] = 0; targetMoonColor[1] = 0; targetMoonColor[2] = 0;
        }
        else if (dayTime > 17.0f) {
            float t = (dayTime - 17.0f) / 2.0f;
            lerpColor(targetSkyColor, colBlueSky, colSunset, t);
            lerpColor(targetSunColor, lightDay, lightSunset, t);
            GLfloat ambDay[] = { 0.4f, 0.4f, 0.4f, 1.0f };
            GLfloat ambSunset[] = { 0.2f, 0.15f, 0.12f, 1.0f }; // 【修改】黄昏环境光更暗
            lerpColor(targetGlobalAmbient, ambDay, ambSunset, t);
            lerpColor(targetMoonColor, lightNight, moonLight, t);
        }
        else {
            memcpy(targetSkyColor, colBlueSky, sizeof(GLfloat) * 4);
            memcpy(targetSunColor, lightDay, sizeof(GLfloat) * 4);
            targetGlobalAmbient[0] = 0.4f; targetGlobalAmbient[1] = 0.4f;
            targetGlobalAmbient[2] = 0.4f; targetGlobalAmbient[3] = 1.0f;
            targetMoonColor[0] = 0; targetMoonColor[1] = 0; targetMoonColor[2] = 0;
        }
    }
    else {
        float t = 1.0f;
        if (dayTime >= 19.0f && dayTime < 21.0f) {
            t = (dayTime - 19.0f) / 2.0f;
            lerpColor(targetSkyColor, colSunset, colNight, t);
            targetSunColor[0] = 0; targetSunColor[1] = 0; targetSunColor[2] = 0;
            lerpColor(targetMoonColor, lightNight, moonLight, t);
        }
        else if (dayTime >= 4.0f && dayTime < 5.0f) {
            t = (dayTime - 4.0f);
            lerpColor(targetSkyColor, colNight, colSunset, t);
            memcpy(targetMoonColor, moonLight, sizeof(GLfloat) * 4);
        }
        else {
            memcpy(targetSkyColor, colNight, sizeof(GLfloat) * 4);
            memcpy(targetMoonColor, moonLight, sizeof(GLfloat) * 4);
            targetSunColor[0] = 0;
        }
        // 深夜环境光：纯正暗灰色
        targetGlobalAmbient[0] = 0.05f; targetGlobalAmbient[1] = 0.05f;
        targetGlobalAmbient[2] = 0.06f; targetGlobalAmbient[3] = 1.0f;
    }

    // 应用天气对天空颜色的影响
    extern WeatherSystem weatherSystem;
    if (weatherSystem.isSnowing) {
        float snowFactor = weatherSystem.getSnowAccumulation() / 5.0f;
        if (snowFactor > 1.0f) snowFactor = 1.0f;
        GLfloat finalSkyColor[4];
        lerpColor(finalSkyColor, targetSkyColor, colSnowSky, snowFactor * 0.9f);
        memcpy(targetSkyColor, finalSkyColor, sizeof(GLfloat) * 4);

        targetGlobalAmbient[0] += 0.05f * snowFactor;
        targetGlobalAmbient[1] += 0.05f * snowFactor;
        targetGlobalAmbient[2] += 0.08f * snowFactor;
    }

    // 平滑过渡
    if (isTransitioning) {
        transitionTime += deltaTime;
        float t = transitionTime / transitionDuration;

        if (t >= 1.0f) {
            t = 1.0f;
            isTransitioning = false;
        }

        // 使用缓动函数使过渡更平滑
        float easeT = t * t * (3.0f - 2.0f * t); // 三次缓动

        // 对所有属性进行插值
        lerpColor(sunPos, prevSunPos, targetSunPos, easeT);
        lerpColor(moonPos, prevMoonPos, targetMoonPos, easeT);
        lerpColor(globalAmbient, prevGlobalAmbient, targetGlobalAmbient, easeT);
        lerpColor(sunColor, prevSunColor, targetSunColor, easeT);
        lerpColor(moonColor, prevMoonColor, targetMoonColor, easeT);
        lerpColor(skyColor, prevSkyColor, targetSkyColor, easeT);
    }
    else {
        // 直接设置到目标值
        memcpy(sunPos, targetSunPos, sizeof(GLfloat) * 4);
        memcpy(moonPos, targetMoonPos, sizeof(GLfloat) * 4);
        memcpy(globalAmbient, targetGlobalAmbient, sizeof(GLfloat) * 4);
        memcpy(sunColor, targetSunColor, sizeof(GLfloat) * 4);
        memcpy(moonColor, targetMoonColor, sizeof(GLfloat) * 4);
        memcpy(skyColor, targetSkyColor, sizeof(GLfloat) * 4);
    }

    // 计算光源强度
    calculateLightIntensities();
}

// 【新增】实现 updateShader
void LightingSystem::updateShader(Shader* shader) {
    shader->use();
    shader->setVec3("lightPos", sunPos[0], sunPos[1], sunPos[2]);
    shader->setVec3("lightColor", adjustedSunColor[0], adjustedSunColor[1], adjustedSunColor[2]);
    shader->setVec3("ambientColor", globalAmbient[0], globalAmbient[1], globalAmbient[2]);
    // viewPos 在观察空间计算中通常由 ModelView 矩阵自动处理，这里可以留空或传 camPos
}

void LightingSystem::applyLighting() {
    // 1. 设置背景色（天空颜色）
    glClearColor(skyColor[0], skyColor[1], skyColor[2], skyColor[3]);

    // 2. 将当前活跃光源的位置传给标准着色器
    // 判断谁在天上（Y坐标 > 0）
    if (standardShader) {
        standardShader->use();
        if (isDaytime()) {
            standardShader->setVec3("lightPos", sunPos[0], sunPos[1], sunPos[2]);
            standardShader->setVec3("lightColor", sunColor[0], sunColor[1], sunColor[2]);
        }
        else {
            // 晚上使用月亮位置作为光照起点
            standardShader->setVec3("lightPos", moonPos[0], moonPos[1], moonPos[2]);
            standardShader->setVec3("lightColor", adjustedMoonColor[0], adjustedMoonColor[1], adjustedMoonColor[2]);
        }
    }
}

// 修改绘制光球的函数，增加发光感
void LightingSystem::drawLightOrbs() {
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // 获取玩家位置，让日月随人移动（实现“无限远”效果，永远追不上）
    extern Player player;

    // 1. 绘制太阳
    if (sunPos[1] > -100.0f) {
        glPushMatrix();
        // 关键：将日月位移到玩家中心，再加上轨道偏移
        glTranslatef(player.x + sunPos[0], player.y + sunPos[1], player.z + sunPos[2]);

        // 太阳依然保持明亮的发光感
        glColor3f(1.0f, 0.95f, 0.5f);
        glutSolidSphere(120.0, 30, 30); // 距离远了，球体也要画大一点
        glPopMatrix();
    }

    // 2. 绘制月亮
    if (moonPos[1] > -100.0f) {
        glPushMatrix();
        glTranslatef(player.x + moonPos[0], player.y + moonPos[1], player.z + moonPos[2]);

        // 【修改点 2】月亮不发光，设为冷灰色/暗白色
        // 不再使用亮白色，而是像石头一样的灰色
        glColor3f(0.4f, 0.4f, 0.45f);
        glutSolidSphere(80.0, 30, 30);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
}

void LightingSystem::setMaterial(GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat shininess) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void LightingSystem::setMaterialWater() {
    GLfloat mat_ambient[] = { 0.1f, 0.1f, 0.3f, 1.0f };
    GLfloat mat_diffuse[] = { 0.4f, 0.6f, 0.9f, 0.6f };
    GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat shininess = 100.0f;
    setMaterial(mat_ambient, mat_diffuse, mat_specular, shininess);
}

bool LightingSystem::isDaytime() const {
    return (dayTime > 6.0f && dayTime < 18.0f);
}
