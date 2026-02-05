#include "headers.h"
#include "texture.h"
#include "platform.h"
#include "lighting.h"
#include <cmath>
#include <vector>

extern float camX;

// ==========================================
// 1. 河流路径生成 (核心修改)
// ==========================================
// 必须使用周期性函数 (sin/cos) 替代贝塞尔曲线，以支持无限延伸
// 且必须与 terrain.cpp 中的河谷生成公式保持一致，否则水会穿模
void generateRiverPath(float x, float& z) {
    // 公式说明：
    // 基准Z: 260.0f
    // 大弯曲: 振幅 100, 频率 x/200
    // 小弯曲: 振幅 50, 频率 x/50
    z = 260.0f + 100.0f * sin(x / 200.0f) + 50.0f * cos(x / 50.0f);
}

// ==========================================
// 2. 切线计算 (用于计算法线和河流宽度方向)
// ==========================================
// 通过对上面的路径公式求导得到准确的切线
// f(x) = 260 + 100*sin(x/200) + 50*cos(x/50)
// f'(x) = 100*(1/200)*cos(x/200) - 50*(1/50)*sin(x/50)
//       = 0.5*cos(x/200) - sin(x/50)
void getRiverTangent(float x, float& tx, float& tz) {
    // 计算斜率 (dz/dx)
    float dzdx = 0.5f * cos(x / 200.0f) - 1.0f * sin(x / 50.0f);

    // 切线向量是 (1, dzdx)
    // 归一化
    float length = sqrt(1.0f + dzdx * dzdx);
    tx = 1.0f / length;
    tz = dzdx / length;
}

// ==========================================
// 3. 绘制河流 (无限延伸版)
// ==========================================
void drawRiver() {
    applyRiverTexture();

    // 材质与颜色
    GLfloat mat_diffuse[] = { 0.2f, 0.4f, 0.8f, 0.7f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f, 0.7f, 1.0f, 0.7f);

    // 纹理动画
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    static float flow = 0.0f; flow += 0.005f;
    glTranslatef(flow, 0.0f, 0.0f);
    glScalef(0.03f, 0.15f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    // --- 关键参数 ---
    float riverWidth = 80.0f;  // 半径 40，确保比地形坑底(50)窄
    float riverSurfaceY = -2.0f;
    float riverBottomY = -30.0f; // 侧壁伸入地下的深度
    float range = 6000.0f;
    float startX = floor((camX - range) / 5.0f) * 5.0f;
    float endX = startX + range * 2;

    // 1. 绘制水面 (顶面)
    glBegin(GL_QUAD_STRIP);
    for (float x = startX; x <= endX; x += 5.0f) {
        float zCenter; generateRiverPath(x, zCenter);
        float tx, tz; getRiverTangent(x, tx, tz);
        float nx = -tz; float nz = tx; // 河流切线的法向量

        float leftZ = zCenter + (riverWidth / 2.0f) * nz;
        float rightZ = zCenter - (riverWidth / 2.0f) * nz;

        glNormal3f(0.0f, 1.0f, 0.0f);
        float texS = x / 200.0f;
        glTexCoord2f(texS, 0.0f); glVertex3f(x, riverSurfaceY, leftZ);
        glTexCoord2f(texS, 1.0f); glVertex3f(x, riverSurfaceY, rightZ);
    }
    glEnd();

    // 2. 绘制左侧壁 (侧裙)
    glBegin(GL_QUAD_STRIP);
    for (float x = startX; x <= endX; x += 5.0f) {
        float zCenter; generateRiverPath(x, zCenter);
        float tx, tz; getRiverTangent(x, tx, tz);
        float nz = tx;
        float leftZ = zCenter + (riverWidth / 2.0f) * nz;

        glTexCoord2f(x / 200.0f, 0.0f); glVertex3f(x, riverSurfaceY, leftZ);
        glTexCoord2f(x / 200.0f, 0.1f); glVertex3f(x, riverBottomY, leftZ);
    }
    glEnd();

    // 3. 绘制右侧壁 (侧裙)
    glBegin(GL_QUAD_STRIP);
    for (float x = startX; x <= endX; x += 5.0f) {
        float zCenter; generateRiverPath(x, zCenter);
        float tx, tz; getRiverTangent(x, tx, tz);
        float nz = tx;
        float rightZ = zCenter - (riverWidth / 2.0f) * nz;

        glTexCoord2f(x / 200.0f, 0.0f); glVertex3f(x, riverSurfaceY, rightZ);
        glTexCoord2f(x / 200.0f, 0.1f); glVertex3f(x, riverBottomY, rightZ);
    }
    glEnd();

    glDisable(GL_BLEND);
    glMatrixMode(GL_TEXTURE); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

// 废弃的旧函数，留空即可
void drawForestGround() {}
