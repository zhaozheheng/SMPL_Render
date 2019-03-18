//
// Created by Zheheng Zhao on 3/17/19.
//

#include "Renderer.h"

using namespace nanogui;

glm::vec3 Renderer::ori_position = glm::vec3(0.0f, 1.0f, 3.0f);

nanogui::Color Renderer::background_color = Color(0.1f, 0.1f, 0.1f, 1.0f);

Camera Renderer::camera = Camera(ori_position);

GLfloat Renderer::lastX = Renderer::width / 2.0;
GLfloat Renderer::lastY = height / 2.0;

GLfloat Renderer::deltaTime = 0.0f;
GLfloat Renderer::lastFrame = 0.0f;

bool Renderer::is_reload_model = false;

bool Renderer::is_dynamic_light_rotate_x = false;
bool Renderer::is_dynamic_light_rotate_y = false;
bool Renderer::is_dynamic_light_rotate_z = false;

// default LINE render mode
Object_Mode Renderer::object_mode = LINE;

Culling_Mode Renderer::culling_mode = CW;

Shading_Mode Renderer::shading_mode = FLAT;

Depth_Mode Renderer::depth_mode = LESS;

GLfloat Renderer::rotate_value = 0;

GLfloat Renderer::z_near = 0.1f;
GLfloat Renderer::z_far = 100.0f;

Lighting Renderer::lightings;
Material Renderer::main_object;

bool Renderer::keys[1024];

bool firstMouse = true;

Screen *screen = nullptr;

Renderer::Renderer() {

}

Renderer::~Renderer() {

}

void Renderer::nanogui_init(GLFWwindow *window) {
    // Create a nanogui screen and pass the glfw pointer to initialize
    screen = new Screen();
    screen->initialize(window, true);

    glViewport(0, 0, this->width, this->height);

    // Create nanogui gui
    bool enabled = true;
    FormHelper *gui_1 = new FormHelper(screen);
    FormHelper *gui_2 = new FormHelper(screen);
    ref<Window> nanoguiWindow_1 = gui_1->addWindow(Eigen::Vector2i(0, 0), "Model & Camera");
    ref<Window> nanoguiWindow_2 = gui_2->addWindow(Eigen::Vector2i(190, 0), "Light");

    gui_1->addGroup("Load SMPL");
    gui_1->addButton("Generate", []() {is_reload_model = true;});

    gui_1->addGroup("Position");
    gui_1->addVariable("X", camera.Position[0])->setSpinnable(true);
    gui_1->addVariable("Y", camera.Position[1])->setSpinnable(true);
    gui_1->addVariable("Z", camera.Position[2])->setSpinnable(true);

    gui_1->addGroup("Rotate");
    gui_1->addVariable("Rotate value", rotate_value)->setSpinnable(true);
    gui_1->addButton("Rotate right+", []() { camera.rotate_x(glm::radians(rotate_value));  });
    gui_1->addButton("Rotate right-", []() { camera.rotate_x(glm::radians(-rotate_value));  });
    gui_1->addButton("Rotate up+", []() { camera.rotate_y(glm::radians(rotate_value));  });
    gui_1->addButton("Rotate up-", []() { camera.rotate_y(glm::radians(-rotate_value));  });
    gui_1->addButton("Rotate front+", []() { camera.rotate_z(glm::radians(rotate_value));  });
    gui_1->addButton("Rotate front-", []() { camera.rotate_z(glm::radians(-rotate_value));  });

    gui_1->addGroup("Configuration");
    gui_1->addVariable("z near", z_near)->setSpinnable(true);
    gui_1->addVariable("z far", z_far)->setSpinnable(true);
    gui_1->addVariable("Render type", object_mode, enabled)->setItems({ "Point", "Line", "Triangle" });
    gui_1->addVariable("Culling type", culling_mode, enabled)->setItems({ "CW", "CCW" });
    gui_1->addVariable("Shading type", shading_mode, enabled)->setItems({ "FLAT", "SMOOTH" });
    gui_1->addVariable("Depth type", depth_mode, enabled)->setItems({ "LESS", "ALWAYS" });

    gui_1->addButton("Reset Camera", []() {
        camera.reset();
    });

    gui_2->addGroup("Lighting");
    gui_2->addVariable("Object color:", main_object.object_color);
    gui_2->addVariable("Object shininess", main_object.shininess);

    gui_2->addVariable("Direction light status:", lightings.direction_light.status);
    gui_2->addVariable("Direction light ambient color:", lightings.direction_light.ambient);
    gui_2->addVariable("Direction light diffuse color:", lightings.direction_light.diffuse);
    gui_2->addVariable("Direction light specular color:", lightings.direction_light.specular);


    gui_2->addVariable("Point light status:", lightings.point_light.status);
    gui_2->addVariable("Point light ambient color:", lightings.point_light.ambient);
    gui_2->addVariable("Point light diffuse color:", lightings.point_light.diffuse);
    gui_2->addVariable("Point light specular color:", lightings.point_light.specular);

    gui_2->addVariable("Point light rotate on X", is_dynamic_light_rotate_x);
    gui_2->addVariable("Point light rotate on Y", is_dynamic_light_rotate_y);
    gui_2->addVariable("Point light rotate on Z", is_dynamic_light_rotate_z);

    gui_2->addButton("Reset Point light", []() {
        lightings.point_light.position = camera.Position;
    });

    screen->setVisible(true);
    screen->performLayout();

    glfwSetCursorPosCallback(window,
                             [](GLFWwindow *window, double x, double y) {
                                 screen->cursorPosCallbackEvent(x, y);
                                 if (firstMouse)
                                 {
                                     lastX = x;
                                     lastY = y;
                                     firstMouse = false;
                                 }

                                 GLfloat xoffset = x - lastX;
                                 GLfloat yoffset = lastY - y;  // Reversed since y-coordinates go from bottom to left

                                 lastX = x;
                                 lastY = y;
                                 int left_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
                                 if (left_state == GLFW_PRESS) {
                                     camera.ProcessMouseMovement(xoffset, yoffset);
                                 }
                             }
    );

    glfwSetMouseButtonCallback(window,
                               [](GLFWwindow *, int button, int action, int modifiers) {
                                   screen->mouseButtonCallbackEvent(button, action, modifiers);
                               }
    );

    glfwSetKeyCallback(window,
                       [](GLFWwindow *window, int key, int scancode, int action, int mods) {
//                           screen->keyCallbackEvent(key, scancode, action, mods);
                           if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                               glfwSetWindowShouldClose(window, GL_TRUE);
                           if (key >= 0 && key < 1024)
                           {
                               if (action == GLFW_PRESS)
                                   keys[key] = true;
                               else if (action == GLFW_RELEASE)
                                   keys[key] = false;
                           }
                       }
    );

    glfwSetCharCallback(window,
                        [](GLFWwindow *, unsigned int codepoint) {
                            screen->charCallbackEvent(codepoint);
                        }
    );

    glfwSetDropCallback(window,
                        [](GLFWwindow *, int count, const char **filenames) {
                            screen->dropCallbackEvent(count, filenames);
                        }
    );

    glfwSetScrollCallback(window,
                          [](GLFWwindow *, double x, double y) {
                              screen->scrollCallbackEvent(x, y);
                          }
    );

    glfwSetFramebufferSizeCallback(window,
                                   [](GLFWwindow *, int width, int height) {
                                       screen->resizeCallbackEvent(width, height);
                                   }
    );
}

void Renderer::init() {
    // init GLFW
    glfwInit();

    // Set all required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFW window object that we can use for GLFW's functions
    this->window = glfwCreateWindow(this->width, this->height, "SMPL Demo", nullptr, nullptr);
    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM

    nanogui_init(this->window);
}

void Renderer::display(GLFWwindow *window) {
    // Build and compile our shader program
    Shader object_shader("code/shaders/basic.vs", "code/shaders/basic.frag");

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        do_movement();

        if (is_reload_model) {
            obj_loader.load_obj();
            obj_loader.setup_model();

            camera.Front = glm::normalize(obj_loader.center - camera.Position);
            camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
            camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
            camera.ori_front = camera.Front;
            lightings.point_light.position = camera.Position;
            camera.reset();
            is_reload_model = false;
        }

        if (is_dynamic_light_rotate_x) {
            glm::mat4 rotationMat(1);
            glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
            rotationMat = glm::rotate(rotationMat, deltaTime, x_axis);
            lightings.point_light.position = rotationMat * glm::vec4(lightings.point_light.position, 1.0f);
        }

        if (is_dynamic_light_rotate_y) {
            glm::mat4 rotationMat(1);
            glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
            rotationMat = glm::rotate(rotationMat, deltaTime, y_axis);
            lightings.point_light.position = rotationMat * glm::vec4(lightings.point_light.position, 1.0f);
        }

        if (is_dynamic_light_rotate_z) {
            glm::mat4 rotationMat(1);
            glm::vec3 z_axis(0.0f, 0.0f, 1.0f);
            rotationMat = glm::rotate(rotationMat, deltaTime, z_axis);
            lightings.point_light.position = rotationMat * glm::vec4(lightings.point_light.position, 1.0f);
        }

        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        object_shader.use();

        // Create camera transformations
        glm::mat4 view, projection;

        view = camera.GetViewMatrix();

        projection = glm::perspective(camera.Zoom, (GLfloat)width / (GLfloat)height, z_near, z_far);

        glUniform1f(glGetUniformLocation(object_shader.program, "z_near"), z_near);
        glUniform1f(glGetUniformLocation(object_shader.program, "z_far"), z_far);

        glUniform1i(glGetUniformLocation(object_shader.program, "shading_mode"), (bool)shading_mode);

        glUniform3f(glGetUniformLocation(object_shader.program, "camera_pos"), camera.Position.x, camera.Position.y, camera.Position.z);

        glUniform3f(glGetUniformLocation(object_shader.program, "main_object.object_color"), main_object.object_color[0], main_object.object_color[1], main_object.object_color[2]);
        glUniform1f(glGetUniformLocation(object_shader.program, "main_object.shininess"), main_object.shininess);

        glUniform1i(glGetUniformLocation(object_shader.program, "dir_light.status"), lightings.direction_light.status);
        glUniform3f(glGetUniformLocation(object_shader.program, "dir_light.direction"), lightings.direction_light.direction[0], lightings.direction_light.direction[1], lightings.direction_light.direction[2]);
        glUniform3f(glGetUniformLocation(object_shader.program, "dir_light.ambient"), lightings.direction_light.ambient[0], lightings.direction_light.ambient[1], lightings.direction_light.ambient[2]);
        glUniform3f(glGetUniformLocation(object_shader.program, "dir_light.diffuse"), lightings.direction_light.diffuse[0], lightings.direction_light.diffuse[1], lightings.direction_light.diffuse[2]);
        glUniform3f(glGetUniformLocation(object_shader.program, "dir_light.specular"), lightings.direction_light.specular[0], lightings.direction_light.specular[1], lightings.direction_light.specular[2]);

        glUniform1i(glGetUniformLocation(object_shader.program, "point_light.status"), lightings.point_light.status);
        glUniform3f(glGetUniformLocation(object_shader.program, "point_light.position"), lightings.point_light.position[0], lightings.point_light.position[1], lightings.point_light.position[2]);
        glUniform3f(glGetUniformLocation(object_shader.program, "point_light.ambient"), lightings.point_light.ambient[0], lightings.point_light.ambient[1], lightings.point_light.ambient[2]);
        glUniform3f(glGetUniformLocation(object_shader.program, "point_light.diffuse"), lightings.point_light.diffuse[0], lightings.point_light.diffuse[1], lightings.point_light.diffuse[2]);
        glUniform3f(glGetUniformLocation(object_shader.program, "point_light.specular"), lightings.point_light.specular[0], lightings.point_light.specular[1], lightings.point_light.specular[2]);
        glUniform1f(glGetUniformLocation(object_shader.program, "point_light.constant"), lightings.point_light.constant);
        glUniform1f(glGetUniformLocation(object_shader.program, "point_light.linear"), lightings.point_light.linear);
        glUniform1f(glGetUniformLocation(object_shader.program, "point_light.quadratic"), lightings.point_light.quadratic);


        // Pass the matrices to the shader
        glUniformMatrix4fv(glGetUniformLocation(object_shader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(object_shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



        obj_loader.draw_object(object_shader, object_mode, culling_mode, shading_mode, depth_mode);

        screen->drawWidgets();

        // Swap the screen buffer
        glfwSwapBuffers(window);
    }

    // Terminate GLFW
    glfwTerminate();
}

void Renderer::run() {
    init();
    display(this->window);
}

void Renderer::do_movement() {
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_Q])
        camera.ProcessKeyboard(UP, deltaTime);
    if (keys[GLFW_KEY_E])
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (keys[GLFW_KEY_I])
        camera.ProcessKeyboard(ROTATE_X_UP, deltaTime);
    if (keys[GLFW_KEY_K])
        camera.ProcessKeyboard(ROTATE_X_DOWN, deltaTime);
    if (keys[GLFW_KEY_J])
        camera.ProcessKeyboard(ROTATE_Y_UP, deltaTime);
    if (keys[GLFW_KEY_L])
        camera.ProcessKeyboard(ROTATE_Y_DOWN, deltaTime);
    if (keys[GLFW_KEY_U])
        camera.ProcessKeyboard(ROTATE_Z_UP, deltaTime);
    if (keys[GLFW_KEY_O])
        camera.ProcessKeyboard(ROTATE_Z_DOWN, deltaTime);
}