#include "headers.h"
#include "geometry.h"
#include<iostream>
double fang[8][3];
double san[8][3];
double parallelogramPrism[8][3];
double triangularPrism[6][3];
struct RiverVertex {
    float x, y, z;
    float u, v;  // 纹理坐标
};
static std::vector<RiverVertex> riverVertices;
static std::vector<unsigned int> riverIndices;

// 绘制球体
void drawSphere(double r, int lats, int longs, double x, double y, double z) {
    int i, j;
    for (i = 0; i <= lats; i++) {
        double lat0 = PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);
        double lat1 = PI * (-0.5 + (double)i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);
        glBegin(GL_QUAD_STRIP);
        glScaled(100, 100, 100);
        for (j = 0; j <= longs; j++) {
            double lng = 2 * PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(zr0, zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(zr1, zr1, z1);
        }
        glEnd();
    }
}

// 构造梯形长方体
void cons2(double x, double y, double z, double x1, double y1, double z1) {
    san[0][0] = x;
    san[0][1] = y;
    san[0][2] = z;

    san[1][0] = x + x1;
    san[1][1] = y;
    san[1][2] = z;

    san[2][0] = x + x1;
    san[2][1] = y + y1;
    san[2][2] = z;

    san[3][0] = x;
    san[3][1] = y + y1 / 2;
    san[3][2] = z;

    for (int i = 0; i < 4; i++) {
        san[i + 4][0] = san[i][0];
        san[i + 4][1] = san[i][1];
        san[i + 4][2] = san[i][2] + z1;      // Z坐标相同
    }
}

// 构造长方体
void cons1(double x, double y, double z, double x1, double y1, double z1) {
    fang[0][0] = x;
    fang[0][1] = y;
    fang[0][2] = z;        // 第0个点

    fang[1][0] = x;
    fang[1][1] = y;
    fang[1][2] = z + z1;   // 第1个点

    fang[2][0] = x + x1;
    fang[2][1] = y;
    fang[2][2] = z + z1;   // 第2个点

    fang[3][0] = x + x1;
    fang[3][1] = y;
    fang[3][2] = z;        // 第3个点

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            if (j == 1)
                fang[i + 4][j] = fang[i][j] + y1;
            else
                fang[i + 4][j] = fang[i][j];
        }
    }
}

// 绘制梯形长方体
void build2() {
    glBegin(GL_POLYGON);
    glNormal3f(0.0, -1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(san[0][0], san[0][1], san[0][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(san[1][0], san[1][1], san[1][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(san[2][0], san[2][1], san[2][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(san[3][0], san[3][1], san[3][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(san[1][0], san[1][1], san[1][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(san[0][0], san[0][1], san[0][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(san[4][0], san[4][1], san[4][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(san[5][0], san[5][1], san[5][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(san[7][0], san[7][1], san[7][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(san[6][0], san[6][1], san[6][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(san[3][0], san[3][1], san[3][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(san[2][0], san[2][1], san[2][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(san[5][0], san[5][1], san[5][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(san[6][0], san[6][1], san[6][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(san[2][0], san[2][1], san[2][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(san[1][0], san[1][1], san[1][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(san[0][0], san[0][1], san[0][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(san[3][0], san[3][1], san[3][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(san[7][0], san[7][1], san[7][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(san[4][0], san[4][1], san[4][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(san[4][0], san[4][1], san[4][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(san[7][0], san[7][1], san[7][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(san[6][0], san[6][1], san[6][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(san[5][0], san[5][1], san[5][2]);
    glEnd();
}

// 绘制长方体
void build() {
    glBegin(GL_POLYGON);
    glNormal3f(0.0, -1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(fang[0][0], fang[0][1], fang[0][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(fang[1][0], fang[1][1], fang[1][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(fang[2][0], fang[2][1], fang[2][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(fang[3][0], fang[3][1], fang[3][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(fang[1][0], fang[1][1], fang[1][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(fang[0][0], fang[0][1], fang[0][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(fang[4][0], fang[4][1], fang[4][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(fang[5][0], fang[5][1], fang[5][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(fang[7][0], fang[7][1], fang[7][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(fang[6][0], fang[6][1], fang[6][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(fang[2][0], fang[2][1], fang[2][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(fang[3][0], fang[3][1], fang[3][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(fang[5][0], fang[5][1], fang[5][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(fang[6][0], fang[6][1], fang[6][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(fang[2][0], fang[2][1], fang[2][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(fang[1][0], fang[1][1], fang[1][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(fang[0][0], fang[0][1], fang[0][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(fang[3][0], fang[3][1], fang[3][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(fang[7][0], fang[7][1], fang[7][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(fang[4][0], fang[4][1], fang[4][2]);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(fang[4][0], fang[4][1], fang[4][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(fang[7][0], fang[7][1], fang[7][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(fang[6][0], fang[6][1], fang[6][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(fang[5][0], fang[5][1], fang[5][2]);
    glEnd();
}

//绘制平行四边形柱体
void cons3(double x1, double y1, double x2, double y2,
    double x3, double y3, double x4, double y4,
    double z_base, double height) {

    parallelogramPrism[0][0] = x1;
    parallelogramPrism[0][1] = y1;
    parallelogramPrism[0][2] = z_base;  // 点1

    parallelogramPrism[1][0] = x2;
    parallelogramPrism[1][1] = y2;
    parallelogramPrism[1][2] = z_base;  // 点2

    parallelogramPrism[2][0] = x3;
    parallelogramPrism[2][1] = y3;
    parallelogramPrism[2][2] = z_base;  // 点3

    parallelogramPrism[3][0] = x4;
    parallelogramPrism[3][1] = y4;
    parallelogramPrism[3][2] = z_base;  // 点4

    for (int i = 0; i < 4; i++) {
        parallelogramPrism[i + 4][0] = parallelogramPrism[i][0];
        parallelogramPrism[i + 4][1] = parallelogramPrism[i][1];
        parallelogramPrism[i + 4][2] = parallelogramPrism[i][2] + height;
    }
}

//绘制平行四边形柱体
void build3() {
    // 绘制底面
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(parallelogramPrism[0][0], parallelogramPrism[0][1], parallelogramPrism[0][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(parallelogramPrism[1][0], parallelogramPrism[1][1], parallelogramPrism[1][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(parallelogramPrism[2][0], parallelogramPrism[2][1], parallelogramPrism[2][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(parallelogramPrism[3][0], parallelogramPrism[3][1], parallelogramPrism[3][2]);
    glEnd();

    // 绘制顶面
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(parallelogramPrism[4][0], parallelogramPrism[4][1], parallelogramPrism[4][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(parallelogramPrism[5][0], parallelogramPrism[5][1], parallelogramPrism[5][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(parallelogramPrism[6][0], parallelogramPrism[6][1], parallelogramPrism[6][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(parallelogramPrism[7][0], parallelogramPrism[7][1], parallelogramPrism[7][2]);
    glEnd();

    // 绘制侧面
    glBegin(GL_POLYGON);
    //计算侧面法线
    double nx1 = parallelogramPrism[1][1] - parallelogramPrism[0][1];
    double ny1 = -(parallelogramPrism[1][0] - parallelogramPrism[0][0]);
    double length1 = sqrt(nx1 * nx1 + ny1 * ny1);
    if (length1 > 0) {
        nx1 /= length1;
        ny1 /= length1;
    }
    glNormal3f(nx1, ny1, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(parallelogramPrism[0][0], parallelogramPrism[0][1], parallelogramPrism[0][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(parallelogramPrism[1][0], parallelogramPrism[1][1], parallelogramPrism[1][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(parallelogramPrism[5][0], parallelogramPrism[5][1], parallelogramPrism[5][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(parallelogramPrism[4][0], parallelogramPrism[4][1], parallelogramPrism[4][2]);
    glEnd();

    // 侧面2
    glBegin(GL_POLYGON);
    double nx2 = parallelogramPrism[2][1] - parallelogramPrism[1][1];
    double ny2 = -(parallelogramPrism[2][0] - parallelogramPrism[1][0]);
    double length2 = sqrt(nx2 * nx2 + ny2 * ny2);
    if (length2 > 0) {
        nx2 /= length2;
        ny2 /= length2;
    }
    glNormal3f(nx2, ny2, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(parallelogramPrism[1][0], parallelogramPrism[1][1], parallelogramPrism[1][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(parallelogramPrism[2][0], parallelogramPrism[2][1], parallelogramPrism[2][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(parallelogramPrism[6][0], parallelogramPrism[6][1], parallelogramPrism[6][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(parallelogramPrism[5][0], parallelogramPrism[5][1], parallelogramPrism[5][2]);
    glEnd();

    // 侧面3
    glBegin(GL_POLYGON);
    double nx3 = parallelogramPrism[3][1] - parallelogramPrism[2][1];
    double ny3 = -(parallelogramPrism[3][0] - parallelogramPrism[2][0]);
    double length3 = sqrt(nx3 * nx3 + ny3 * ny3);
    if (length3 > 0) {
        nx3 /= length3;
        ny3 /= length3;
    }
    glNormal3f(nx3, ny3, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(parallelogramPrism[2][0], parallelogramPrism[2][1], parallelogramPrism[2][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(parallelogramPrism[3][0], parallelogramPrism[3][1], parallelogramPrism[3][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(parallelogramPrism[7][0], parallelogramPrism[7][1], parallelogramPrism[7][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(parallelogramPrism[6][0], parallelogramPrism[6][1], parallelogramPrism[6][2]);
    glEnd();

    // 侧面4
    glBegin(GL_POLYGON);
    double nx4 = parallelogramPrism[0][1] - parallelogramPrism[3][1];
    double ny4 = -(parallelogramPrism[0][0] - parallelogramPrism[3][0]);
    double length4 = sqrt(nx4 * nx4 + ny4 * ny4);
    if (length4 > 0) {
        nx4 /= length4;
        ny4 /= length4;
    }
    glNormal3f(nx4, ny4, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(parallelogramPrism[3][0], parallelogramPrism[3][1], parallelogramPrism[3][2]);
    glTexCoord2f(1.0, 0.0); glVertex3f(parallelogramPrism[0][0], parallelogramPrism[0][1], parallelogramPrism[0][2]);
    glTexCoord2f(1.0, 1.0); glVertex3f(parallelogramPrism[4][0], parallelogramPrism[4][1], parallelogramPrism[4][2]);
    glTexCoord2f(0.0, 1.0); glVertex3f(parallelogramPrism[7][0], parallelogramPrism[7][1], parallelogramPrism[7][2]);
    glEnd();
}
//绘制三角柱
void cons4(double x1, double y1, double x2, double y2, double x3, double y3, double z_base, double height){
    // 存储底面三个顶点
    triangularPrism[0][0] = x1;
    triangularPrism[0][1] = y1;
    triangularPrism[0][2] = z_base;

    triangularPrism[1][0] = x2;
    triangularPrism[1][1] = y2;
    triangularPrism[1][2] = z_base;

    triangularPrism[2][0] = x3;
    triangularPrism[2][1] = y3;
    triangularPrism[2][2] = z_base;

    // 生成顶面三个顶点（z坐标增加height）
    for (int i = 0; i < 3; i++) {
        triangularPrism[i + 3][0] = triangularPrism[i][0];
        triangularPrism[i + 3][1] = triangularPrism[i][1];
        triangularPrism[i + 3][2] = triangularPrism[i][2] + height;
    }
}

//绘制三角柱
void build4() {
    // 绘制底面三角形
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0, 0.0, -1.0);  // 底面法线向下
    glTexCoord2f(0.0, 0.0);
    glVertex3f(triangularPrism[0][0], triangularPrism[0][1], triangularPrism[0][2]);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(triangularPrism[1][0], triangularPrism[1][1], triangularPrism[1][2]);
    glTexCoord2f(0.5, 1.0);
    glVertex3f(triangularPrism[2][0], triangularPrism[2][1], triangularPrism[2][2]);
    glEnd();

    // 绘制顶面三角形
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0, 0.0, 1.0);  // 顶面法线向上
    glTexCoord2f(0.0, 0.0);
    glVertex3f(triangularPrism[3][0], triangularPrism[3][1], triangularPrism[3][2]);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(triangularPrism[4][0], triangularPrism[4][1], triangularPrism[4][2]);
    glTexCoord2f(0.5, 1.0);
    glVertex3f(triangularPrism[5][0], triangularPrism[5][1], triangularPrism[5][2]);
    glEnd();

    // 绘制侧面1（连接点1-2）
    glBegin(GL_QUADS);
    // 计算侧面1的法线
    double nx1 = triangularPrism[1][1] - triangularPrism[0][1];
    double ny1 = -(triangularPrism[1][0] - triangularPrism[0][0]);
    double length1 = sqrt(nx1 * nx1 + ny1 * ny1);
    if (length1 > 0) {
        nx1 /= length1;
        ny1 /= length1;
    }
    glNormal3f(nx1, ny1, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(triangularPrism[0][0], triangularPrism[0][1], triangularPrism[0][2]);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(triangularPrism[1][0], triangularPrism[1][1], triangularPrism[1][2]);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(triangularPrism[4][0], triangularPrism[4][1], triangularPrism[4][2]);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(triangularPrism[3][0], triangularPrism[3][1], triangularPrism[3][2]);
    glEnd();

    // 绘制侧面2（连接点2-3）
    glBegin(GL_QUADS);
    // 计算侧面2的法线
    double nx2 = triangularPrism[2][1] - triangularPrism[1][1];
    double ny2 = -(triangularPrism[2][0] - triangularPrism[1][0]);
    double length2 = sqrt(nx2 * nx2 + ny2 * ny2);
    if (length2 > 0) {
        nx2 /= length2;
        ny2 /= length2;
    }
    glNormal3f(nx2, ny2, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(triangularPrism[1][0], triangularPrism[1][1], triangularPrism[1][2]);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(triangularPrism[2][0], triangularPrism[2][1], triangularPrism[2][2]);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(triangularPrism[5][0], triangularPrism[5][1], triangularPrism[5][2]);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(triangularPrism[4][0], triangularPrism[4][1], triangularPrism[4][2]);
    glEnd();

    // 绘制侧面3（连接点3-1）
    glBegin(GL_QUADS);
    // 计算侧面3的法线
    double nx3 = triangularPrism[0][1] - triangularPrism[2][1];
    double ny3 = -(triangularPrism[0][0] - triangularPrism[2][0]);
    double length3 = sqrt(nx3 * nx3 + ny3 * ny3);
    if (length3 > 0) {
        nx3 /= length3;
        ny3 /= length3;
    }
    glNormal3f(nx3, ny3, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(triangularPrism[2][0], triangularPrism[2][1], triangularPrism[2][2]);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(triangularPrism[0][0], triangularPrism[0][1], triangularPrism[0][2]);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(triangularPrism[3][0], triangularPrism[3][1], triangularPrism[3][2]);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(triangularPrism[5][0], triangularPrism[5][1], triangularPrism[5][2]);
    glEnd();
}

