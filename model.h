#ifndef MODEL_H
#define MODEL_H

#include "headers.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <map>

struct ModelVertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct Mesh {
    // 【修改】移除了 VAO，只保留 VBO/EBO，防止状态冲突崩溃
    GLuint VBO, EBO;
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
    GLuint textureID;

    // 初始化
    void setupMesh() {
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ModelVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // 绘制 (使用旧式指针，完美兼容光照系统)
    void draw() {
        if (textureID != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else {
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.2f, 0.6f, 0.2f);
        }

        // 绑定缓冲
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // 启用客户端状态
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // 设置指针
        // Vertex: 偏移 0
        glVertexPointer(3, GL_FLOAT, sizeof(ModelVertex), (void*)0);
        // Normal: 偏移 12 (3个float)
        glNormalPointer(GL_FLOAT, sizeof(ModelVertex), (void*)(3 * sizeof(float)));
        // TexCoord: 偏移 24 (3+3个float)
        glTexCoordPointer(2, GL_FLOAT, sizeof(ModelVertex), (void*)(6 * sizeof(float)));

        // 绘制
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

        // 清理状态 (防止影响地形绘制)
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        if (textureID != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        }
    }
};

class Model {
public:
    Model(const std::string& path, const std::string& texturePath = "");
    ~Model();
    void draw();

private:
    std::vector<Mesh> meshes;
    std::string directory;
    static std::map<std::string, GLuint> textureCache;
    GLuint forcedTextureID = 0;

    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    GLuint loadMaterialTexture(aiMaterial* mat, aiTextureType type);
};

#endif
