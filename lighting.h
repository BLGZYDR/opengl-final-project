#ifndef LIGHTING_H
#define LIGHTING_H

#include "headers.h"
#include <cmath>
#include <iostream>

// 光照系统类
class LightingSystem {
public:
    LightingSystem();

    // 初始化光照
    void init();

    // 更新时间、太阳/月亮位置
    void update(float deltaTime);

    // 应用光照到场景（每帧调用）
    void applyLighting();

    // 绘制太阳和月亮的光球（可视化光源）
    void drawLightOrbs();

    // 辅助函数：设置物体材质 (Phong模型)
    // ambient: 环境光系数, diffuse: 漫反射颜色, specular: 高光颜色, shininess: 高光硬度
    void setMaterial(GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat shininess);

    // 预设材质辅助函数
    void setMaterialWater();
    void setMaterialGrass();
    void setMaterialSnow();
    void setMaterialStone();

    // 获取当前是否为白天（用于其他逻辑判断）
    bool isDaytime() const;

    // 【新增】获取当前环境光
    const GLfloat* getGlobalAmbient() const { return globalAmbient; }

    // 【新增】获取当前天空颜色
    const GLfloat* getSkyColor() const { return skyColor; }

    // 【新增】获取当前太阳颜色
    const GLfloat* getSunColor() const { return sunColor; }

    // 【新增】获取当前月亮颜色
    const GLfloat* getMoonColor() const { return moonColor; }

    // 【新增】获取当前时间
    float getDayTime() const { return dayTime; }

    // 【新增】获取太阳位置
    const GLfloat* getSunPosition() const { return sunPos; }

    // 【新增】获取月亮位置
    const GLfloat* getMoonPosition() const { return moonPos; }

    // 【新增】获取太阳强度（0.0-1.0）
    float getSunIntensity() const { return sunIntensity; }

    // 【新增】获取月亮强度（0.0-1.0）
    float getMoonIntensity() const { return moonIntensity; }

    void updateShader(Shader* shader);

private:
    float dayTime;          // 当前时间 (0 - 24)
    float timeSpeed;        // 时间流逝速度
    float sunAngle;         // 太阳角度 (弧度)

    // 【新增】光源强度（用于平滑过渡）
    float sunIntensity;     // 太阳强度 (0.0-1.0)
    float moonIntensity;    // 月亮强度 (0.0-1.0)

    // 【新增】平滑过渡相关变量
    float transitionTime = 0.0f;       // 当前过渡时间
    float transitionDuration = 2.0f;    // 过渡持续时间（秒）
    bool isTransitioning = false;       // 是否正在过渡
    GLfloat prevSunPos[4];             // 前一个太阳位置
    GLfloat prevMoonPos[4];            // 前一个月亮位置
    GLfloat prevGlobalAmbient[4];      // 前一个环境光
    GLfloat prevSunColor[4];           // 前一个太阳颜色
    GLfloat prevMoonColor[4];          // 前一个月亮颜色
    GLfloat prevSkyColor[4];           // 前一个天空颜色
    float prevDayTime;                 // 前一个时间

    // 光源位置
    GLfloat sunPos[4];
    GLfloat moonPos[4];

    // 颜色变量
    GLfloat globalAmbient[4];
    GLfloat sunColor[4];
    GLfloat moonColor[4];
    GLfloat skyColor[4];

    // 【新增】调整后的光源颜色（带强度）
    GLfloat adjustedSunColor[4];
    GLfloat adjustedMoonColor[4];

    // 私有辅助：线性插值颜色
    void lerpColor(GLfloat* out, const GLfloat* a, const GLfloat* b, float t);

    // 【新增】平滑插值函数
    float smoothstep(float t) {
        t = std::max(0.0f, std::min(1.0f, t));
        return t * t * (3.0f - 2.0f * t); // 三次缓动函数
    }

    // 【新增】计算光源强度
    void calculateLightIntensities();
};

// 声明全局实例
extern LightingSystem lightingSystem;

#endif
