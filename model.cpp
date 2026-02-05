#include "model.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::map<std::string, GLuint> Model::textureCache;

std::string GetFileNameFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) return path.substr(lastSlash + 1);
    return path;
}

GLuint TextureFromFile(const char* path, const std::string& directory) {
    std::string filename = std::string(path);
    std::string simpleName = GetFileNameFromPath(filename);

    // 构建可能的路径
    std::string pathTextures = "textures/" + simpleName; // 注意文件夹名 texture 没加 s
    std::string pathRelative = directory + '/' + filename;

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = nullptr;

    // 尝试加载
    data = stbi_load(pathTextures.c_str(), &width, &height, &nrComponents, 0);
    if (!data) data = stbi_load(pathRelative.c_str(), &width, &height, &nrComponents, 0);
    if (!data) data = stbi_load(simpleName.c_str(), &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format = GL_RGB;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return textureID;
    }
    else {
        // 【关键修复】加载失败时，data 是 NULL，绝对不要调用 stbi_image_free(data)
        // 直接打印错误并返回 0
        std::cerr << "[ERROR] Texture failed to load: " << simpleName << std::endl;
        return 0;
    }
}

Model::Model(const std::string& path, const std::string& texturePath) {
    if (!texturePath.empty()) {
        forcedTextureID = TextureFromFile(texturePath.c_str(), "");
        // if (forcedTextureID != 0) std::cout << "[INFO] Manual texture applied: " << texturePath << std::endl;
    }

    Assimp::Importer importer;
    // 保留你的参数
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // =============================================================
    // 【新增功能】模型自动对齐与归一化 (Auto Alignment)
    // 解决“悬空”和“陷地”问题的终极方案
    // =============================================================

    float minX = 1e10f, maxX = -1e10f;
    float minY = 1e10f, maxY = -1e10f;
    float minZ = 1e10f, maxZ = -1e10f;

    // 1. 第一遍遍历：计算包围盒 (Bounding Box)
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            aiVector3D v = mesh->mVertices[j];
            if (v.x < minX) minX = v.x; if (v.x > maxX) maxX = v.x;
            if (v.y < minY) minY = v.y; if (v.y > maxY) maxY = v.y;
            if (v.z < minZ) minZ = v.z; if (v.z > maxZ) maxZ = v.z;
        }
    }

    // 2. 智能判断：树是朝哪个方向长的？
    // 通常树的高度(Height)远大于宽度。比较 Y轴跨度和 Z轴跨度。
    float spanY = maxY - minY;
    float spanZ = maxZ - minZ;

    // 计算需要平移的偏移量
    float offsetX = -(minX + maxX) / 2.0f; // X轴居中
    float offsetY = 0.0f;
    float offsetZ = 0.0f;

    if (spanY > spanZ) {
        // 这是一个 Y 轴向上的树 (标准情况)
        // 我们把它的最低点 (minY) 移动到 0
        offsetY = -minY;
        offsetZ = -(minZ + maxZ) / 2.0f; // Z轴居中
    }
    else {
        // 这是一个 Z 轴向上的树 (躺着的树)
        // 我们把它的最低点 (minZ) 移动到 0
        offsetY = -(minY + maxY) / 2.0f; // Y轴居中
        offsetZ = -minZ;
    }

    // 3. 第二遍遍历：应用偏移量，永久修改顶点数据
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            mesh->mVertices[j].x += offsetX;
            mesh->mVertices[j].y += offsetY;
            mesh->mVertices[j].z += offsetZ;
        }
    }
    // =============================================================

    directory = path.substr(0, path.find_last_of('/'));
    if (directory == path) directory = path.substr(0, path.find_last_of('\\'));

    //displayList = glGenLists(1);
    //glNewList(displayList, GL_COMPILE);
    processNode(scene->mRootNode, scene);
    //glEndList();

    std::cout << "[INFO] Model Loaded & Aligned: " << path << std::endl;
}
Model::~Model() {
    for (auto& mesh : meshes) { glDeleteBuffers(1, &mesh.VBO); glDeleteBuffers(1, &mesh.EBO); }
}
void Model::draw() { for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].draw(); }
void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) { aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; processMesh(mesh, scene); }
    for (unsigned int i = 0; i < node->mNumChildren; i++) processNode(node->mChildren[i], scene);
}

// ... processMesh 和 loadMaterialTexture ...
void Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
    GLuint textureID = 0;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        ModelVertex vertex;
        vertex.x = mesh->mVertices[i].x; vertex.y = mesh->mVertices[i].y; vertex.z = mesh->mVertices[i].z;
        if (mesh->HasNormals()) { vertex.nx = mesh->mNormals[i].x; vertex.ny = mesh->mNormals[i].y; vertex.nz = mesh->mNormals[i].z; }
        else { vertex.nx = 0.0f; vertex.ny = 1.0f; vertex.nz = 0.0f; }
        if (mesh->mTextureCoords[0]) { vertex.u = mesh->mTextureCoords[0][i].x; vertex.v = mesh->mTextureCoords[0][i].y; }
        else { vertex.u = 0.0f; vertex.v = 0.0f; }
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    if (forcedTextureID != 0) textureID = forcedTextureID;
    else if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) textureID = loadMaterialTexture(material, aiTextureType_DIFFUSE);
    // 兼容代码: else if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0) textureID = loadMaterialTexture(material, aiTextureType_BASE_COLOR);

    Mesh newMesh;
    newMesh.vertices = vertices;
    newMesh.indices = indices;
    newMesh.textureID = textureID;
    newMesh.setupMesh();
    meshes.push_back(newMesh);
}

GLuint Model::loadMaterialTexture(aiMaterial* mat, aiTextureType type) {
    aiString str;
    mat->GetTexture(type, 0, &str);
    std::string pathString = str.C_Str();
    std::string filename = GetFileNameFromPath(pathString);
    if (textureCache.find(filename) != textureCache.end()) return textureCache[filename];
    std::string finalPath = "texture/" + filename;
    GLuint textureID = TextureFromFile(finalPath.c_str(), "");
    if (textureID == 0) textureID = TextureFromFile(filename.c_str(), "");
    textureCache[filename] = textureID;
    return textureID;
}
