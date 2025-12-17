cmake_minimum_required(VERSION 3.15)
project(MyGLUTProject)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置默认构建类型为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 查找OpenGL
find_package(OpenGL REQUIRED)

# 使用FetchContent下载FreeGLUT
include(FetchContent)

FetchContent_Declare(
    freeglut
    URL https://github.com/FreeGLUTProject/freeglut/releases/download/v3.4.0/freeglut-3.4.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(freeglut)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/scene.cpp
    src/house.cpp
    src/platform.cpp
    src/texture.cpp
    src/geometry.cpp
    src/weather.cpp
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${freeglut_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    freeglut_static
)

# 设置编译定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
    FREEGLUT_STATIC
)

# Windows平台特定设置
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
        glu32
        winmm
        gdi32
    )
endif()

# 安装目标（可选）
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)