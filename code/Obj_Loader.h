//
// Created by Zheheng Zhao on 3/17/19.
//

#ifndef SMPL_RENDER_OBJ_LOADER_H
#define SMPL_RENDER_OBJ_LOADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "Shader.h"

struct Vertex {
    // Position
    glm::vec3 Position;
};

enum Object_Mode {
    POINT = 0,
    LINE = 1,
    TRIANGLE = 2
};

enum Culling_Mode {
    CW = 0,
    CCW = 1
};

enum Shading_Mode {
    FLAT = 0,
    SMOOTH = 1
};

enum Depth_Mode {
    LESS = 0,
    ALWAYS = 1
};

class Obj_Loader {
public:
    std::vector<Vertex> vertices;
    GLuint vao, vbo;

    glm::vec3 max_bound;
    glm::vec3 min_bound;
    glm::vec3 center;

    Obj_Loader();
    ~Obj_Loader();

    void load_obj();
    void setup_model();
    void draw_object(Shader &shader, Object_Mode render_mode, Culling_Mode culling_mode, Shading_Mode shading_mode, Depth_Mode depth_mode);

private:
    void find_center();
};


#endif //SMPL_RENDER_OBJ_LOADER_H
