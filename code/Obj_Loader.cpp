//
// Created by Zheheng Zhao on 3/17/19.
//

#include "Obj_Loader.h"

#include "Load_SMPL.h"

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
    smpl_model model;
    handmodel hand_l, hand_r;
    loadModel(model, hand_l, hand_r);

    this->vertices.clear();
    std::vector<glm::vec3> positions;

    for (int i = 0; i < model.model.rows; ++i) {
        glm::vec3 vert_pos;
        vert_pos[0] = model.model.at<double>(i, 0);
        vert_pos[1] = model.model.at<double>(i, 1);
        vert_pos[2] = model.model.at<double>(i, 2);
        positions.push_back(vert_pos);
    }

    for (int i = 0; i < model.f.rows; ++i) {
        Vertex vertex_0;
        vertex_0.Position = positions[model.f.at<double>(i, 0)];
        Vertex vertex_1;
        vertex_1.Position = positions[model.f.at<double>(i, 1)];
        Vertex vertex_2;
        vertex_2.Position = positions[model.f.at<double>(i, 2)];
        this->vertices.push_back(vertex_0);
        this->vertices.push_back(vertex_1);
        this->vertices.push_back(vertex_2);
    }

    std::cout << "vertiecs size is: " << this->vertices.size() << std::endl;
    std::cout << "position size is: " << positions.size() << std::endl;
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
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        }
    }

    glBindVertexArray(0);
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