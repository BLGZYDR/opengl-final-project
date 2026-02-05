#ifndef RAIN_H
#define RAIN_H

#include "headers.h"
#include <vector>

struct WeatherParticle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float size;
    bool active;
    int type; // 0-雨, 1-水花, 2-雪
};

struct SmokeParticle {
    float x, y, z;
    float vy;       // 上升速度
    float size;     // 大小（随时间变大）
    float life;     // 透明度/寿命
    float offsetX, offsetZ; // 飘动偏移
};

class WeatherSystem {
private:
    std::vector<WeatherParticle> particles;
    std::vector<SmokeParticle> smokeParticles;

    float snowAccumulation;

    void createRaindrop();
    void createSnowflake();
    void createSplash(float x, float y, float z);
    void drawSnowOnHouse();  // 【新增】绘制房子上的积雪

public:
    WeatherSystem();
    ~WeatherSystem();

    bool isRaining;
    bool isSnowing;

    void init(int maxParticles = 5000);
    void update(float deltaTime);
    void updateSmoke(float deltaTime);

    void draw();
    void drawSmoke();
    void renderRainAccumulation();
    void renderSnowAccumulation();

    void toggleRain();
    void toggleSnow();
    void clear();
    void clearSnow();

    // 获取积雪厚度
    float getSnowAccumulation() const { return snowAccumulation; }
};

extern WeatherSystem weatherSystem;

#endif
