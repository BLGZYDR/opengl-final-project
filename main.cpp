#include "headers.h"
#include "texture.h"
#include "scene.h"
#include "platform.h"  // 添加平台头文件

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
    GLfloat sun_direction[] = { 100.0, 100.0, 100.0, 0.0 };
    GLfloat sun_intensity[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat ambient_intensity[] = { 0.5, 0.5, 0.5, 1.0 };

    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_intensity);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    loadTextures();
    initPlatformTextures();

    cout << "The OpenGL version is: " << glGetString(GL_VERSION) << "\n";
    cout << glGetString(GL_VENDOR) << "\n";

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

    glutDisplayFunc(display);
    glutSpecialFunc(specialkeys);

    // 设置空闲函数,用于水流动画
    glutIdleFunc([]() {
        static int lastTime = 0;
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        if (currentTime - lastTime > 30) {  // 每30毫秒更新一次
            lastTime = currentTime;
            glutPostRedisplay();
        }
        });

    glutMainLoop();

    return 0;
}