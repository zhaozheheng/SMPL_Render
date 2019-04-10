//
// Created by Zheheng Zhao on 3/17/19.
//

#include <random>

#include "Obj_Loader.h"

#include "Load_SMPL.h"
#include "Load_FLAME.h"

glm::vec3 cubePositions[] = {

        glm::vec3(0.0f,  0.0f,  0.0f),

        glm::vec3(2.0f,  5.0f, -15.0f),

        glm::vec3(-1.5f, -2.2f, -2.5f),

        glm::vec3(-3.8f, -2.0f, -12.3f),

        glm::vec3(2.4f, -0.4f, -3.5f),

        glm::vec3(-1.7f,  3.0f, -7.5f),

        glm::vec3(1.3f, -2.0f, -2.5f),

        glm::vec3(1.5f,  2.0f, -2.5f),

        glm::vec3(1.5f,  0.2f, -1.5f),

        glm::vec3(-1.3f,  1.0f, -1.5f)

};

Obj_Loader::Obj_Loader() {

}

Obj_Loader::~Obj_Loader() {

}

void Obj_Loader::load_obj() {
    smpl_model smpl;
    handmodel hand_l, hand_r;
    flame_model flame;
    loadModel(smpl, hand_l, hand_r, pose_set);
    loadFlame(flame);

    this->vertices.clear();
    std::vector<glm::vec3> positions;

    for (int i = 0; i < smpl.model.rows; ++i) {    // get SMPL vertices
        glm::vec3 vert_pos;
        vert_pos[0] = smpl.model.at<double>(i, 0);
        vert_pos[1] = smpl.model.at<double>(i, 1);
        vert_pos[2] = smpl.model.at<double>(i, 2);
        positions.push_back(vert_pos);
    }

    Mat diff;
    diff = smpl.J.row(12) - flame.J.row(0);
    std::cout << "diff: " << diff << std::endl;

    for (int i = 0; i < flame.model.rows; ++i) {    // get FLAME vertices
        glm::vec3 vert_pos;
        vert_pos[0] = flame.model.at<double>(i, 0) + diff.at<double>(0, 0);
        vert_pos[1] = flame.model.at<double>(i, 1) + diff.at<double>(0, 1);
        vert_pos[2] = flame.model.at<double>(i, 2) + diff.at<double>(0, 2);
        positions.push_back(vert_pos);
    }

    for (int i = 0; i < smpl.f.rows; ++i) {    // get SMPL faces
        Vertex vertex_0;
        vertex_0.Position = positions[smpl.f.at<double>(i, 0)];
        Vertex vertex_1;
        vertex_1.Position = positions[smpl.f.at<double>(i, 1)];
        Vertex vertex_2;
        vertex_2.Position = positions[smpl.f.at<double>(i, 2)];
        this->vertices.push_back(vertex_0);
        this->vertices.push_back(vertex_1);
        this->vertices.push_back(vertex_2);
    }

    for (int i = 0; i < flame.f.rows; ++i) {    // get FLAME faces
        Vertex vertex_0;
        vertex_0.Position = positions[flame.f.at<double>(i, 0) + smpl.model.rows];
        Vertex vertex_1;
        vertex_1.Position = positions[flame.f.at<double>(i, 1) + smpl.model.rows];
        Vertex vertex_2;
        vertex_2.Position = positions[flame.f.at<double>(i, 2) + smpl.model.rows];
        this->vertices.push_back(vertex_0);
        this->vertices.push_back(vertex_1);
        this->vertices.push_back(vertex_2);
    }

//    for (int i = 0; i < smpl.J.rows; ++i) {
//        glm::vec3 vert_pos;
//        vert_pos[0] = smpl.J.at<double>(i, 0);
//        vert_pos[1] = smpl.J.at<double>(i, 1);
//        vert_pos[2] = smpl.J.at<double>(i, 2);
//        positions.push_back(vert_pos);
//    }

    std::cout << "vertiecs size is: " << this->vertices.size() << std::endl;
    std::cout << "position size is: " << positions.size() << std::endl;
//    std::cout << "smpl head is: " << smpl.J << std::endl;
//    std::cout << "flame head is: " << flame.J << std::endl;
//    std::cout << "smpl trans is: " << smpl.trans << std::endl;
//    std::cout << "flame trans is: " << flame.trans << std::endl;
}

void Obj_Loader::setup_model() {
    find_center();

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

    glBindVertexArray(0);
}

void Obj_Loader::draw_object(Shader &shader, Object_Mode render_mode, Culling_Mode culling_mode, Shading_Mode shading_mode, Depth_Mode depth_mode) {
    glBindVertexArray(this->vao);
    glm::mat4 model;

    glEnable(GL_DEPTH_TEST);

    if (depth_mode == LESS) {
        glDepthFunc(GL_LESS);
    }

    if (depth_mode == ALWAYS) {
        glDepthFunc(GL_ALWAYS);
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    if (culling_mode == CW) {
        glFrontFace(GL_CW);
    }
    if (culling_mode == CCW) {
        glFrontFace(GL_CCW);
    }

    for (int i = 0; i < 1; ++i) {
        model = glm::mat4();
        model = glm::translate(model, cubePositions[i]);

        glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (render_mode == LINE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (render_mode == POINT)
            glDrawArrays(GL_POINTS, 0, this->vertices.size());
        if (render_mode == TRIANGLE) {
//            glDrawArrays(GL_LINES, 0, this->vertices.size());
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        }
    }

    glBindVertexArray(0);
}

void Obj_Loader::get_ran_pose() {
    int len = sizeof(pose_set) / sizeof(pose_set[0]);

    //normal distribution random
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);

    //(0,1) normal distribution
    std::normal_distribution<double> distribution (0.0,1.0);

    for (int i = 0; i < len; ++i) {
        pose_set[i] = distribution(generator) * 0.2;
    }
}

void Obj_Loader::find_center() {
    max_bound = glm::vec3(INT_MIN);
    min_bound = glm::vec3(INT_MAX);
    for (auto vertex : vertices) {
        max_bound[0] = max(vertex.Position[0], max_bound[0]);
        max_bound[1] = max(vertex.Position[1], max_bound[1]);
        max_bound[2] = max(vertex.Position[2], max_bound[2]);
        min_bound[0] = min(vertex.Position[0], min_bound[0]);
        min_bound[1] = min(vertex.Position[1], min_bound[1]);
        min_bound[2] = min(vertex.Position[2], min_bound[2]);
    }
    center = (max_bound + min_bound) * 0.5f;
}