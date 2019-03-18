//
// Created by Zheheng Zhao on 3/17/19.
//

#ifndef SMPL_RENDER_LIGHTING_H
#define SMPL_RENDER_LIGHTING_H

#include <nanogui/nanogui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Lighting {
    struct Direction_Light {
        bool status;
        glm::vec3 direction;

        nanogui::Color ambient;
        nanogui::Color diffuse;
        nanogui::Color specular;
    };

    struct Point_Light {
        bool status;
        glm::vec3 position;
        float constant;
        float linear;
        float quadratic;

        nanogui::Color ambient;
        nanogui::Color diffuse;
        nanogui::Color specular;
    };

public:
    Direction_Light direction_light;
    Point_Light point_light;

    Lighting() {
        direction_light.status = true;
        direction_light.direction = glm::vec3(0.0f,-1.0f,-1.0f);
        // white
        direction_light.ambient = nanogui::Color(255.0f, 255.0f, 255.0f, 1.0f);
        direction_light.diffuse = nanogui::Color(255.0f, 255.0f, 255.0f, 1.0f);
        direction_light.specular = nanogui::Color(255.0f, 255.0f, 255.0f, 1.0f);

        point_light.status = true;
        point_light.position = glm::vec3(1.2f, 1.0f, 2.0f);
        // white
        point_light.ambient = nanogui::Color(255.0f, 255.0f, 255.0f, 1.0f);
        point_light.diffuse = nanogui::Color(255.0f, 255.0f, 255.0f, 1.0f);
        point_light.specular = nanogui::Color(255.0f, 255.0f, 255.0f, 1.0f);
        point_light.constant = 1.0f;
        point_light.linear = 0.09f;
        point_light.quadratic = 0.032f;
    }

    ~Lighting() {

    }
};


#endif //SMPL_RENDER_LIGHTING_H
