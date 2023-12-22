#ifndef MAIN_CPP
#define MAIN_CPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <chrono>

#include "ShaderUtils.h"
#include "Shader.h"
#include "stb_image.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera;

float opacity = 0.2f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void processInput(GLFWwindow *window, Shader shader) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } 

    camera.processInputForCamera(window);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    camera.processMouseInputForCamera(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processScrollForCamera(xoffset, yoffset);
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learning OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // setup for mouse movement -> camera movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // path from models folder to desired obj files...
    std::string path = std::string("backpack/backpack.obj");

    Model local_model(path);
    std::cout << "CREATED MODEL" << std::endl;

    Shader lightingShader("obj_material");
    lightingShader.use();
    std::cout << "CREATED SHADER" << std::endl;

    // create camera
    camera = Camera(SCR_WIDTH, SCR_HEIGHT);

    // setting light structs
    // setting Spot Light
    lightingShader.setVec3("spotLight.position", 0,0,0);
    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", 0.09f);
    lightingShader.setFloat("spotLight.quadratic", 0.032f);
    
    lightingShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
    lightingShader.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f); 
    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));


    // setting Directional Light
    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
    lightingShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f); 
    lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);


    unsigned int num_renders = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        processInput(window, lightingShader);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // setting up shader lighting uniforms
        lightingShader.use();
        lightingShader.setVec3("spotLight.position", camera.camera_pos);
        lightingShader.setVec3("viewPos", camera.camera_pos);
        lightingShader.setVec3("spotLight.direction", camera.camera_front);
        
        glm::mat4 projection = camera.getProjection();
        glm::mat4 view = camera.getView();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        // render the loaded model
        local_model.Draw(lightingShader);


        glfwSwapBuffers(window);
        glfwPollEvents();

        // time it takes for it to render 60 frames ...
        auto stop = std::chrono::high_resolution_clock::now();
        num_renders++;
        if (num_renders % 60 == 0) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "Has taken " << duration.count() << " mili seconds for 60 frames..." << std::endl;
            start = std::chrono::high_resolution_clock::now();
        }
    }

    glfwTerminate();
    return 0;
}

#endif