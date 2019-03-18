//
// Created by Zheheng Zhao on 3/17/19.
//

#ifndef SMPL_RENDER_RENDERER_H
#define SMPL_RENDER_RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <nanogui/nanogui.h>

#include "Obj_Loader.h"
#include "Shader.h"
#include "Camera.h"
#include "Lighting.h"
#include "Material.h"

class Renderer {
public:
    const static GLuint width = 1600, height = 900;
    GLFWwindow* window;

    static Camera camera;

    static glm::vec3 ori_position;
    static nanogui::Color background_color;

    static GLfloat lastX;
    static GLfloat lastY;
    static bool keys[1024];

    static GLfloat deltaTime;
    static GLfloat lastFrame;

    static bool is_reload_model;

    static bool is_dynamic_light_rotate_x;
    static bool is_dynamic_light_rotate_y;
    static bool is_dynamic_light_rotate_z;

    static std::string model_name;
    static Object_Mode object_mode;
    static Culling_Mode culling_mode;
    static Shading_Mode shading_mode;
    static Depth_Mode depth_mode;

    static GLfloat rotate_value;

    static GLfloat z_near;
    static GLfloat z_far;

    Obj_Loader obj_loader;
    static Lighting lightings;

    static Material main_object;

    Renderer();
    ~Renderer();

    void nanogui_init(GLFWwindow *window);
    void init();
    void display(GLFWwindow *window);
    void run();

    static void do_movement();
};


#endif //SMPL_RENDER_RENDERER_H
