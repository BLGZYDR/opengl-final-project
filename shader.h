#ifndef SHADER_H
#define SHADER_H

#include "headers.h"
#include <string> 

class Shader {
public:
    GLuint Program;
    Shader(const char* vertexSource, const char* fragmentSource);
    void use();
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setMat4(const std::string& name, const GLfloat* value);
private:
    void checkCompileErrors(GLuint shader, std::string type);
};

// 【新增】声明标准 Shader 的源码字符串
extern const char* standardVS;
extern const char* standardFS;

// 【新增】阴影 Shader (Pass 1)
extern const char* shadowVS;
extern const char* shadowFS;

// 【新增】屏幕后处理 Shader (SSAO + ToneMapping + Gamma)
extern const char* screenVS;
extern const char* screenFS;

#endif
