#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera 
{
private:
    float near = 0.1f;
    float far = 100.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float zoom = 45.0f;
    float last_x = 400, last_y = 300;
    float delta_time = 0.0f; // time between last n current frame
    float last_frame = 0.0f; // last frame time

    glm::vec3 camera_pos   = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f, 0.0f);

    bool first_mouse = true;

    int SCR_WIDTH;
    int SCR_HEIGHT;


public:

    Camera() {

    }

    Camera(int SCR_WIDTH, int SCR_HEIGHT) {
        this->SCR_HEIGHT = SCR_HEIGHT;
        this->SCR_WIDTH = SCR_WIDTH;
    }

    void processInputForCamera(GLFWwindow *window) {

        timeUpdate();

        float camera_speed = 2.5f * delta_time;



        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera_pos += camera_speed * camera_front;
        } 

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera_pos -= camera_front * camera_speed;
        } 

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
        } 

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
        }
    }

    void processMouseInputForCamera(double xpos, double ypos) {
        if (first_mouse) {
            last_x = xpos;
            last_y = ypos;
            first_mouse = false;
        }

        float xoffset = xpos - last_x;
        float yoffset = last_y - ypos; // y is reversed;

        last_x = xpos;
        last_y = ypos;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // look at mouse movement code
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_front = glm::normalize(direction);
    }

    void processScrollForCamera(double xoffset, double yoffset) {
        zoom -= (float)yoffset;
        if (zoom < 1.0f) 
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

    glm::mat4 getView() {
        return glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
    }

    glm::mat4 getProjection() {
        return glm::perspective(glm::radians(zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, near, far);
    }

    void timeUpdate() {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
    }
};


#endif