//
// Created by Zheheng Zhao on 3/17/19.
//

#ifndef SMPL_RENDER_CAMERA_H
#define SMPL_RENDER_CAMERA_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    ROTATE_X_UP,
    ROTATE_X_DOWN,
    ROTATE_Y_UP,
    ROTATE_Y_DOWN,
    ROTATE_Z_UP,
    ROTATE_Z_DOWN,
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat ROLL = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

class Camera {
public:
    // Camera Attributes
    glm::vec3 ori_position;
    glm::vec3 ori_front;

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Eular Angles
    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat Roll;
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH, GLfloat roll = ROLL) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->ori_position = position;
        this->ori_front = this->Front;
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->Roll = roll;
        this->updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch, GLfloat roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {

        this->Position = glm::vec3(posX, posY, posZ);
        this->ori_front = this->Front;
        this->ori_position = this->Position;
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->Roll = roll;
        this->updateCameraVectors();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    void reset() {
        this->Position = ori_position;
        this->Front = ori_front;
        this->Right = glm::normalize(glm::cross(this->Front,this->WorldUp));
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime;
        if (direction == FORWARD)
            this->Position += this->Front * velocity;
        if (direction == BACKWARD)
            this->Position -= this->Front * velocity;
        if (direction == LEFT)
            this->Position -= this->Right * velocity;
        if (direction == RIGHT)
            this->Position += this->Right * velocity;
        if (direction == UP)
            this->Position += this->Up * velocity;
        if (direction == DOWN)
            this->Position -= this->Up * velocity;
        if (direction == ROTATE_X_UP)
            rotate_x(velocity);
        if (direction == ROTATE_X_DOWN)
            rotate_x(-velocity);
        if (direction == ROTATE_Y_UP)
            rotate_y(velocity);
        if (direction == ROTATE_Y_DOWN)
            rotate_y(-velocity);
        if (direction == ROTATE_Z_UP)
            rotate_z(velocity);
        if (direction == ROTATE_Z_DOWN)
            rotate_z(-velocity);
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw += xoffset;
        this->Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Eular angles
        this->updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
            this->Zoom -= yoffset;
        if (this->Zoom <= 1.0f)
            this->Zoom = 1.0f;
        if (this->Zoom >= 45.0f)
            this->Zoom = 45.0f;
    }

    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front,up;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

        this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }

    void rotate_x(GLfloat angle){
        glm::vec3 up = this->Up;
        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, angle, this->Right);
        this->Up = glm::normalize(glm::vec3(rotationMat * glm::vec4(up, 1.0)));
        this->Front = glm::normalize(glm::cross(this->Up, this->Right));
    }

    void rotate_y(GLfloat angle) {
        glm::vec3 front = this->Front;
        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, angle, this->Up);
        this->Front = glm::normalize(glm::vec3(rotationMat * glm::vec4(front, 1.0)));
        this->Right = glm::normalize(glm::cross(this->Front, this->Up));
    }

    void rotate_z(GLfloat angle) {
        glm::vec3 right = this->Right;
        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, angle, this->Front);
        this->Right = glm::normalize(glm::vec3(rotationMat * glm::vec4(right, 1.0)));
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }
};


#endif //SMPL_RENDER_CAMERA_H
