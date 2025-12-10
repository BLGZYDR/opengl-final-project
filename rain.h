#ifndef RAIN_H
#define RAIN_H

#include "headers.h"

// 粒子结构体
struct RainParticle {
    float x, y, z;          // 位置
    float vx, vy, vz;       // 速度
    float life;             // 生命周期
    float size;             // 粒子大小
    bool active;            // 是否活跃
    int type;               // 粒子类型：0-雨滴，1-水花
};

//下雨系统类
class RainSystem {
private:
    std::vector<RainParticle> particles;
    // 创建新的雨滴粒子
    void createRaindrop();
    // 创建水花粒子
    void createSplash(float x, float y, float z);
public:
    RainSystem();
    ~RainSystem();
    //是否正在下雨
    bool isRaining;         
    //初始化系统
    void init(int maxParticles = 5000);

    //更新粒子系统
    void update(float deltaTime);

    //绘制粒子
    void render();

    //切换下雨状态
    void toggleRain();

    //清除所有粒子
    void clear();
};

//全局下雨系统实例
extern RainSystem rainSystem;

#endif
