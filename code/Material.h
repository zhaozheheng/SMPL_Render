//
// Created by Zheheng Zhao on 3/17/19.
//

#ifndef SMPL_RENDER_MATERIAL_H
#define SMPL_RENDER_MATERIAL_H

#include <nanogui/nanogui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Material {
public:

    nanogui::Color object_color;
    float shininess;

    Material() {
        // deep sky blue
        object_color = nanogui::Color(0.0f, 191.0f, 255.0f, 1.0f);
        shininess = 32.0f;
    }

    ~Material() {

    }
};


#endif //SMPL_RENDER_MATERIAL_H
