#include "shader.h"
#include <iostream> 

// -------------------------------------------------------------
// 1. Shadow Shader (深度图生成 - Pass 1)
// -------------------------------------------------------------
const char* shadowVS = R"( #version 120
void main() { 
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
} )";

const char* shadowFS = R"( #version 120
void main() { 
    // 深度值自动写入
} )";

// -------------------------------------------------------------
// 2. Standard Shader (常规物体渲染 - 木屋、家具、工具等)
// -------------------------------------------------------------
const char* standardVS = R"( #version 120
varying vec3 FragPos; 
varying vec3 Normal; 
varying vec2 TexCoords; 
varying vec4 FragPosLightSpace;

uniform mat4 lightSpaceMatrix;

void main() {
    vec4 worldPos = gl_ModelViewMatrix * gl_Vertex;
    FragPos = vec3(worldPos);
    FragPosLightSpace = lightSpaceMatrix * worldPos;
    Normal = normalize(gl_NormalMatrix * gl_Normal);
    TexCoords = gl_MultiTexCoord0.xy;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
} )";

const char* standardFS = R"( #version 120
varying vec3 FragPos; 
varying vec3 Normal; 
varying vec2 TexCoords; 
varying vec4 FragPosLightSpace;

uniform sampler2D texture1; 
uniform float useTexture; 
uniform sampler2D shadowMap;

uniform vec3 lightPos; 
uniform vec3 lightColor; 
uniform vec3 ambientColor; 
uniform vec3 skyColorTop; 
uniform vec3 skyColorBottom;   

// 篝火点光源 Uniforms
uniform vec3 campfirePos;
uniform vec3 campfireColor;
uniform float campfireIntensity;

vec3 GetSkyColor(vec3 dir) { 
    float t = dir.y * 0.5 + 0.5; 
    t = clamp(t, 0.0, 1.0); 
    return mix(skyColorBottom, skyColorTop, t); 
}

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) return 0.0;
    
    vec3 n = normalize(Normal);
    vec3 l = normalize(lightPos - FragPos);
    
    // 【关键】动态 Bias。垂直表面 bias 小，倾斜表面 bias 大
    // 将 0.005 缩小到更精密的值
    float bias = max(0.002 * (1.0 - dot(n, l)), 0.0005); 
    
    float shadow = 0.0;
    // 使用 PCF 滤波
    vec2 texelSize = 1.0 / vec2(4096.0, 4096.0); 
    for(int x = -1; x <= 1; ++x) { 
        for(int y = -1; y <= 1; ++y) { 
            float pcfDepth = texture2D(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0; 
        } 
    }
    return shadow / 9.0;
}

void main() {
    vec4 texData = (useTexture > 0.5) ? texture2D(texture1, TexCoords) : vec4(1.0);
    vec3 albedo = gl_FrontMaterial.diffuse.rgb * texData.rgb;
    
    // 获取材质的自发光分量（在 house.cpp 中我们会设置这个）
    vec3 emission = gl_FrontMaterial.emission.rgb; 

    vec3 N = normalize(Normal); 
    vec3 V = normalize(-FragPos); 

    // 太阳/月亮 光照
    vec3 L = normalize(lightPos - FragPos);
    float shadow = ShadowCalculation(FragPosLightSpace);
    float diff = max(dot(N, L), 0.0);
    vec3 directionalLight = diff * lightColor * albedo * (1.0 - shadow);

    // 环境光
    vec3 ambient = (ambientColor + GetSkyColor(N) * 0.2) * albedo;

    // 篝火光照
    vec3 fireDir = campfirePos - FragPos;
    float dFire = length(fireDir);
    float atten = 1.0 / (1.0 + 0.007 * dFire + 0.00008 * dFire * dFire);
    vec3 fireDiffuse = max(dot(N, normalize(fireDir)), 0.0) * campfireColor * campfireIntensity * atten * albedo;

    // 最终颜色 = 环境光 + 平行光 + 篝火 + 自发光
    vec3 result = ambient + directionalLight + fireDiffuse + emission;

    // 远景雾化 (Fog)
    float dist = length(FragPos); 
    float fogFactor = clamp((12000.0 - dist) / 10000.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.5, 0.7, 0.9); 
    
    gl_FragColor = vec4(mix(fogColor, result, fogFactor), texData.a);
} )";

// -------------------------------------------------------------
// 3. Terrain Shader (地形专用渲染)
// -------------------------------------------------------------
const char* terrainVS = R"( #version 120
attribute vec2 aPos; 
varying vec3 FragPos; 
varying vec3 Normal; 
varying float Height; 
varying vec4 FragPosLightSpace;

uniform vec2 offset; 
uniform mat4 lightSpaceMatrix;

float getHeight(float x, float z) {
    // 1. 获取河流中心线
    float riverZ = 260.0 + 100.0 * sin(x / 200.0) + 50.0 * cos(x / 50.0);
    float dRiver = abs(z - riverZ);

    // 2. 基础地形高度 (噪声 + 房屋平台)
    float noise = 60.0 * sin(x / 300.0) * cos(z / 300.0) + 
                  20.0 * sin(x / 100.0 + z / 150.0) + 
                  5.0 * sin(x / 20.0) * sin(z / 20.0);
    float naturalH = noise + 10.0;

    float hX = 350.0; float hZ = -120.0;
    float pH = 50.0; float fR = 125.0; float tR = 350.0;
    float dist = sqrt((x - hX)*(x - hX) + (z - hZ)*(z - hZ));
    float baseH;
    if (dist < fR) {
        baseH = pH;
    } else if (dist < tR) {
        float t = (dist - fR) / (tR - fR);
        float weight = 1.0 - (t * t * (3.0 - 2.0 * t));
        baseH = mix(naturalH + 15.0, pH, weight);
    } else {
        baseH = naturalH + 15.0;
    }

    // 3. 【核心河谷雕刻】
    // 河水半径是 40，水面高度约 -2.0
    if (dRiver < 50.0) {
        // 河床：半径 50 以内强行推平到 -15.0。
        // 这比水面(40.0)宽，确保水边有一圈平坦的小沙滩，不会穿进斜坡
        return -15.0; 
    } 
    else if (dRiver < 80.0) {
        // 河堤：从河床(-15.0) 攀升到 隆起的堤坝顶点 (baseH + 15.0)
        float t = (dRiver - 50.0) / 30.0;
        float smoothT = t * t * (3.0 - 2.0 * t);
        return mix(-15.0, baseH + 15.0, smoothT);
    } 
    else if (dRiver < 150.0) {
        // 坡度回落：从堤坝顶点回到正常地面 baseH
        float t = (dRiver - 80.0) / 70.0;
        float smoothT = t * t * (3.0 - 2.0 * t);
        return mix(baseH + 15.0, baseH, smoothT);
    }

    return baseH;
}

vec3 calcNormal(float x, float z) { 
    float e = 0.5; 
    float hL = getHeight(x - e, z); float hR = getHeight(x + e, z); 
    float hD = getHeight(x, z - e); float hU = getHeight(x, z + e); 
    return normalize(vec3(hL - hR, 1.0, hD - hU)); 
}

void main() {
    vec3 worldPos; 
    worldPos.x = aPos.x + offset.x; 
    worldPos.z = aPos.y + offset.y; 
    worldPos.y = getHeight(worldPos.x, worldPos.z);
    FragPos = worldPos; 
    Height = worldPos.y; 
    Normal = calcNormal(worldPos.x, worldPos.z);
    FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(worldPos, 1.0);
} )";

const char* terrainFS = R"( #version 120
varying vec3 FragPos; 
varying vec3 Normal; 
varying float Height; 
varying vec4 FragPosLightSpace;

uniform vec3 lightPos; 
uniform vec3 lightColor; 
uniform vec3 ambientColor;  
uniform sampler2D shadowMap;

uniform vec3 campfirePos; 
uniform vec3 campfireColor; 
uniform float campfireIntensity;

// 【新增】积雪系数 uniform
uniform float snowFactor; 

void main() {
    vec3 albedo = vec3(0.15, 0.4, 0.1); 
    if(Height > 48.0) albedo = vec3(0.4, 0.35, 0.2);

    // 2. 【核心】积雪逻辑：根据 snowFactor 混合白色
    // 我们让积雪主要堆积在平坦的表面（Normal.y 较大处）
    vec3 snowColor = vec3(0.9, 0.95, 1.0); // 略带冷色的雪白
    float snowDensity = clamp(snowFactor * (Normal.y * 1.5), 0.0, 1.0);
    vec3 finalAlbedo = mix(albedo, snowColor, snowDensity);
    
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - FragPos);

    // 阴影
    vec3 proj = FragPosLightSpace.xyz / FragPosLightSpace.w * 0.5 + 0.5;
    float shadow = 0.0;
    float bias = 0.005;
    float d = texture2D(shadowMap, proj.xy).r;
    shadow = proj.z - bias > d ? 1.0 : 0.0;

    // 使用 finalAlbedo 替换原来的 albedo 进行光照计算
    vec3 ambient = ambientColor * 0.5 * finalAlbedo;
    vec3 diffuse = max(dot(N, L), 0.0) * lightColor * finalAlbedo * (1.0 - shadow);
    
    // 篝火影响也应用积雪颜色
    vec3 fireDir = campfirePos - FragPos;
    float dFire = length(fireDir);
    float atten = 1.0 / (1.0 + 0.007 * dFire + 0.00008 * dFire * dFire);
    vec3 fireDiffuse = max(dot(N, normalize(fireDir)), 0.0) * campfireColor * campfireIntensity * atten * finalAlbedo;

    gl_FragColor = vec4(ambient + diffuse + fireDiffuse, 1.0);
} )";

// -------------------------------------------------------------
// 4. Screen Shader (后处理 - Pass 3: 寒冷、滤镜、UI反馈)
// -------------------------------------------------------------
const char* screenVS = R"( #version 120
attribute vec2 aPos; 
attribute vec2 aTexCoords; 
varying vec2 TexCoords;

void main() { 
    TexCoords = aTexCoords; 
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
} )";

const char* screenFS = R"( #version 120
varying vec2 TexCoords;
uniform sampler2D sceneColor; 
uniform sampler2D sceneDepth;
uniform float distToFire; 
uniform float health; // 传入玩家生命值

void main() {
    vec3 color = texture2D(sceneColor, TexCoords).rgb;
    
    // 1. 寒冷滤镜：远离火堆时画面变蓝
    float coldFactor = clamp((distToFire - 250.0) / 450.0, 0.0, 0.7);
    vec3 coldTint = vec3(0.7, 0.8, 1.2); 
    color = mix(color, color * coldTint, coldFactor);

    // 2. 濒死反馈：血量低时屏幕边缘变红
    if (health < 40.0) {
        float distToCenter = length(TexCoords - vec2(0.5));
        // 边缘暗角强度
        float vignette = smoothstep(0.3, 0.8, distToCenter);
        // 随时间脉动的红色效果
        float intensity = (1.0 - health / 40.0) * vignette;
        color = mix(color, vec3(0.7, 0.0, 0.0), intensity);
    }

    // 3. Tone Mapping (色调映射)
    vec3 mapped = color / (color + vec3(1.0));
    // 4. Gamma Correction (伽马校正)
    mapped = pow(mapped, vec3(1.0 / 2.2));
    
    gl_FragColor = vec4(mapped, 1.0);
} )";

// -------------------------------------------------------------
// Shader 类功能实现
// -------------------------------------------------------------
Shader::Shader(const char* vCode, const char* fCode) {
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    Program = glCreateProgram();
    glAttachShader(Program, vertex);
    glAttachShader(Program, fragment);
    glLinkProgram(Program);
    checkCompileErrors(Program, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(Program);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(Program, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(Program, name.c_str()), x, y, z);
}

void Shader::setMat4(const std::string& name, const GLfloat* value) {
    glUniformMatrix4fv(glGetUniformLocation(Program, name.c_str()), 1, GL_FALSE, value);
}

void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "| ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
        }
    }
}
