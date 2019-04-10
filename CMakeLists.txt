cmake_minimum_required(VERSION 3.11)
project(SMPL_Render)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-framework OpenGL")

find_package(OpenCV REQUIRED)

set(GLAD_INCLUDE_DIRS "/Users/zhaozheheng/nanogui/ext/glad/include/")
set(GLFW_INCLUDE_DIRS "/Users/zhaozheheng/nanogui/ext/glfw/include/")
set(NANOGUI_INCLUDE_DIRS "/Users/zhaozheheng/nanogui/include/")
set(NANOVG_INCLUDE_DIRS "/Users/zhaozheheng/nanogui/ext/nanovg/src/")
set(EIGEN_DIRS "/Users/zhaozheheng/nanogui/ext/eigen/")
set(GLM_INCLUDE_DIRS "/usr/local/include/")
set(NANOGUI_LIBS "/Users/zhaozheheng/nanogui/lib")

set(SMPL_PART code/Load_SMPL.h code/Posemapper.h code/lbs.h code/Load_FLAME.h)
set(RENDER_PART code/main.cpp code/Shader.h glad.c code/Obj_Loader.cpp code/Obj_Loader.h code/Renderer.cpp code/Renderer.h code/Lighting.h code/Camera.h code/Material.h)

set(SOURCE_FILES ${SMPL_PART} ${RENDER_PART})
add_executable(SMPL_Render ${SOURCE_FILES})

include_directories(${OpenCV_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${NANOGUI_INCLUDE_DIRS} ${NANOVG_INCLUDE_DIRS} ${EIGEN_DIRS} ${GLM_INCLUDE_DIRS})

target_link_libraries(SMPL_Render ${OpenCV_LIBS} ${NANOGUI_LIBS}/libnanogui.dylib)