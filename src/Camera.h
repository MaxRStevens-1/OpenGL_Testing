#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


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

    float MOVEMENT_CONSTANT = 1.0f;

    bool first_mouse = true;

    int SCR_WIDTH;
    int SCR_HEIGHT;


public:
    glm::vec3 camera_pos   = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up    = glm::vec3(0.0f, 1.0f, 0.0f);

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



        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            camera_pos -= camera_speed * camera_up;
        }


        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera_pos += camera_speed * camera_up;
        }    
    }

    void processInputForGroundCamera(GLFWwindow *window) {

        timeUpdate();

        float camera_speed = 2.5f * delta_time;

        // move forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera_pos.z -= camera_speed * MOVEMENT_CONSTANT;
        } 

        // move backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera_pos.z += camera_speed * MOVEMENT_CONSTANT;
        } 

        // strafe left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera_pos.x -= camera_speed * MOVEMENT_CONSTANT;
        } 

        // strafe right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera_pos.x += camera_speed * MOVEMENT_CONSTANT;
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
        // return cameraLookAt(camera_pos, camera_pos + camera_front, camera_up);
    }

    glm::mat4 cameraLookAt(glm::vec3 pos, glm::vec3 forward, glm::vec3 up) {
        glm::mat4 orientationMatrix = glm::mat4(1.0f);
        
        glm::vec3 zaxis = glm::normalize(pos - forward);

        glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis));
        glm::vec3 yaxis = glm::cross(zaxis, xaxis);

        orientationMatrix[0][0] = xaxis.x;
        orientationMatrix[1][0] = xaxis.y;
        orientationMatrix[2][0] = xaxis.z;

        orientationMatrix[0][1] = yaxis.x;
        orientationMatrix[1][1] = yaxis.y;
        orientationMatrix[2][1] = yaxis.z;

        orientationMatrix[0][2] = zaxis.x;
        orientationMatrix[1][2] = zaxis.y;
        orientationMatrix[2][2] = zaxis.z;

        glm::mat4 posMatrix = glm::mat4(1.0f);
        posMatrix[3][0] = -pos.x;
        posMatrix[3][1] = -pos.y;
        posMatrix[3][2] = -pos.z;

        return orientationMatrix * posMatrix;
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