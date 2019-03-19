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

    double pose_set[78] = { -0.17192541, +0.36310464, +0.05572387, -0.42836206, -0.00707548, +0.03556427,
                            +0.18696896, -0.22704364, -0.39019834, +0.20273526, +0.07125099, +0.07105988,
                            +0.71328310, -0.29426986, -0.18284189, +0.72134655, +0.07865227, +0.08342645,
                            +0.00934835, +0.12881420, -0.02610217, -0.15579594, +0.25352553, -0.26097519,
                            -0.04529948, -0.14718626, +0.52724564, -0.07638319, +0.03324086, +0.05886086,
                            -0.05683995, -0.04069042, +0.68593617, -0.75870686, -0.08579930, -0.55086359,
                            -0.02401033, -0.46217096, -0.03665799, +0.12397343, +0.10974685, -0.41607569,
                            -0.26874970, +0.40249335, +0.21223768, +0.03365140, -0.05243080, +0.16074013,
                            +0.13433811, +0.10414972, -0.98688595, -0.17270103, +0.29374368, +0.61868383,
                            +0.00458329, -0.15357027, +0.09531648, -0.10624117, +0.94679869, -0.26851003,
                            +0.58547889, -0.13735695, -0.39952280, -0.16598853, -0.14982575, -0.27937399,
                            +0.12354536, -0.55101035, -0.41938681, +0.52238684, -0.23376718, -0.29814804,
                            -0.42671473, -0.85829819, -0.50662164, +1.97374622, -0.84298473, -1.29958491 };

    Obj_Loader();
    ~Obj_Loader();

    void load_obj();
    void setup_model();
    void draw_object(Shader &shader, Object_Mode render_mode, Culling_Mode culling_mode, Shading_Mode shading_mode, Depth_Mode depth_mode);
    void get_ran_pose();

private:
    void find_center();
};


#endif //SMPL_RENDER_OBJ_LOADER_H
