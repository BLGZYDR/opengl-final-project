#include "headers.h"
#include "texture.h"
#include "scene.h"
#include "house.h"
#include "platform.h"
#include "weather.h" 

// 添加键盘回调函数
void keyboard(unsigned char key, int x, int y){
    switch (key) {
    case 'r':
    case 'R':
        weatherSystem.toggleRain();
        std::cout << "Rain toggled. Is raining: " << (weatherSystem.isRaining ? "Yes" : "No") << std::endl;
        break;
    case 's':
    case 'S':
        weatherSystem.toggleSnow();
        std::cout << "Snow toggled. Is snowing: " << (weatherSystem.isSnowing ? "Yes" : "No") << std::endl;
        break;
    case 'c':
    case 'C':
        // 清除所有天气效果
        if (weatherSystem.isRaining) weatherSystem.toggleRain();
        if (weatherSystem.isSnowing) weatherSystem.toggleSnow();
        weatherSystem.clearSnow();
        std::cout << "All weather effects cleared." << std::endl;
        break;
    }
    glutPostRedisplay();
}

void specialkeys(int key, int x, int y) {
    if (key == GLUT_KEY_RIGHT)
        rotate_y += 5;
    else if (key == GLUT_KEY_LEFT)
        rotate_y -= 5;
    else if (key == GLUT_KEY_UP)
        rotate_x += 5;
    else if (key == GLUT_KEY_DOWN)
        rotate_x -= 5;
    glutPostRedisplay();
}

void init(void) {
    // 简易光照系统
    GLfloat sun_direction[] = { 100.0, 100.0, 100.0, 0.0 };
    GLfloat sun_intensity[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat ambient_intensity[] = { 0.5, 0.5, 0.5, 1.0 };
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_intensity);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);

    // 启用颜色
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // 启用纹理功能
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 加载所有纹理
    loadTextures();

    // 初始化天气系统
    weatherSystem.init(5000);  // 最多5000个粒子

    cout << "The OpenGL version is: " << glGetString(GL_VERSION) << "\n";
    cout << glGetString(GL_VENDOR) << "\n";
    cout << "Weather Controls:" << std::endl;
    cout << "Press 'R' to toggle rain on/off" << std::endl;
    cout << "Press 'S' to toggle snow on/off" << std::endl;
    cout << "Press 'C' to clear all weather effects" << std::endl;

    glLineWidth(5);
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glOrtho(-w, w, -h, h, -w, w);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("A House with Textures and River Platform");

    init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // 加载场景与按键
    glutDisplayFunc(display);
    glutSpecialFunc(specialkeys);
    glutKeyboardFunc(keyboard);  // 添加键盘回调

    // 设置空闲函数,用于水流动画和天气粒子更新
    glutIdleFunc([]() {
        static int lastTime = 0;
        int currentTime = glutGet(GLUT_ELAPSED_TIME);

        if (currentTime - lastTime > 16) {  // 每16毫秒更新一次，约60FPS
            float deltaTime = (currentTime - lastTime) / 1000.0f;  // 转换为秒

            // 更新天气系统
            weatherSystem.update(deltaTime);

            lastTime = currentTime;
            glutPostRedisplay();
        }
        });

    glutMainLoop();

    return 0;
}