#ifndef MAIN_CPP
#define MAIN_CPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <chrono>
#include <fstream>
#include <regex>

#include "ShaderUtils.h"
#include "Shader.h"
#include "stb_image.h"
// #include "Camera.h"
#include "model_animation.h"
#include "Mesh.h"
#include "AccelerationCamera.h"
#include "Animation.hpp"
#include "filesystem.h"
#include "Animator.hpp"
#include "joints_loader_helper.hpp"
#include "rotations_test.hpp"
#include "dancingVampireUtils.hpp"
#include "AssimpGLMHelpers.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned int ANIMATION_UPDATE_FRAMES = 5;

bool should_stop = false;

AccelerationCamera camera;

float opacity = 0.2f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

auto joint_to_index = name_to_index();
auto index_to_joint = hashtable_from_const();
std::vector<position> vamp_pos(62);

void processInput(GLFWwindow *window, Shader shader) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } 
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        should_stop = !should_stop;
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


void GetJointWorldPositions(const aiNode* node, const aiMatrix4x4& parentTransform = aiMatrix4x4()) {
    aiMatrix4x4 transform = parentTransform;


    std::string name = node->mName.C_Str();
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::toupper(c); });

    std::cout << "Joint Name: " << name << std::endl;

    if (joint_to_index.find(name) != joint_to_index.end()) {
        transform = node->mTransformation * parentTransform;
        // Check if this node corresponds to a joint
        aiVector3D jointPosition = transform * aiVector3D(0.0f, 0.0f, 0.0f);
        // jointPosition = jointPosition + p_vec;
        glm::vec3 loc_vec = AssimpGLMHelpers::GetGLMVec(jointPosition);
        position local_p(loc_vec.x, loc_vec.y, loc_vec.z);
        local_p = local_p.scale(.01);
        std::cout << ", Joint World Position: (" << local_p.x << ", " << local_p.y << ", " << local_p.z << ")" << std::endl;

        vamp_pos[joint_to_index[name]] = local_p;
    } 

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        GetJointWorldPositions(node->mChildren[i], transform);
    }
}

int main()
{

    // test_basic_rotations();
    // return 0;

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
    // glDepthFunc(GL_LESS);  

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // setup for mouse movement -> camera movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    Shader lightingShader("point_shader");
    lightingShader.use();

    // now I have the vertices of blazepose model
    // std::vector<std::vector<float>> positions = load_joints_all_lines("bow.txt");
    auto [base_model, name_rotation_list] = load_blaze_model_from_file("ymca_4_adjusted_matrix.txt");
    bodymodel current_model = base_model;
    std::vector<std::vector<float>> positions = base_model.vectorify_positions_in_order();
    // flatten vertices seperate by time to single list for easy retrieval
    std::vector<float> flat_positions = flatten(positions);

    // change to using 4x4 matrices, and translation matrices

    // now what I create a local model representation of the 3d model
    // also create a way to grab out positions of 3d model for a base model
    // then create an model -> adjusted blaze model conversion
    //  perhaps add an another var to joint, marking whether or not to perform local rotation calc?
    //  also have a hashmap from adjusted model bone to 3d model bone 


    // path from models folder to desired obj files...
    std::string path = std::string("./src/models/dancing_vampire/dancing_vampire.dae");

    Model local_model(path);

    std::cout << "CREATED SHADER" << std::endl;    

    // create camera
    camera = AccelerationCamera(SCR_WIDTH, SCR_HEIGHT);

    // setting up animation

    Animation danceAnimation(FileSystem::getPath(path),
        &local_model);
    Animator animator(&danceAnimation);
    animator.UpdateAnimation(5.0f);

    bodymodel dancing_vampire = create_local_dancing_vampire_model();
    auto index_name_map = hashtable_from_const();
    auto info_map = danceAnimation.GetBoneIDMap();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(FileSystem::getPath(path), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return -1;
    }

    GetJointWorldPositions(scene->mRootNode);
    dancing_vampire.set_positions(vamp_pos);
    dancing_vampire.positions[0] = position(0, 0, 0);
    std::cout << dancing_vampire.toString() << std::endl;
    flat_positions = flatten(dancing_vampire.vectorify_positions_in_order());

    // current_model = base_model.rotate_self_by_rotations(name_rotation_list[0], current_model);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * flat_positions.size(), &flat_positions[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); 

    GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
        fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "binding buffers", e, e);
        exit(20);
    }
    



    // // setting light structs
    // // setting Spot Light
    // lightingShader.setVec3("spotLight.position", 0,0,0);
    // lightingShader.setFloat("spotLight.constant", 1.0f);
    // lightingShader.setFloat("spotLight.linear", 0.09f);
    // lightingShader.setFloat("spotLight.quadratic", 0.032f);
    
    // lightingShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
    // lightingShader.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f); 
    // lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

    // lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    // lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));


    // // setting Directional Light
    // lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    // lightingShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
    // lightingShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f); 
    // lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

    // Assimp::Importer importer;

    unsigned int num_renders = 0;
    auto start = std::chrono::high_resolution_clock::now();

    float deltaTime = glfwGetTime();
    float lastFrame = glfwGetTime();

    // glLineWidth(10.0f); my gl implementation doesn't support line widths :(
    glPointSize(10.0f);
    unsigned int current_frame = 0;
    unsigned int starting_pos = 0;
    while (!glfwWindowShouldClose(window)) {

        // per frame logic 

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        processInput(window, lightingShader);
        // animator.UpdateAnimation(deltaTime);


        // glClearColor(0.4f, 0.3f, 0.2f, 0.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // setting up shader lighting uniforms

        // lightingShader.setVec3("spotLight.position", camera.camera_pos);
        // lightingShader.setVec3("viewPos", camera.camera_pos);
        // lightingShader.setVec3("spotLight.direction", camera.camera_front);
        
        glm::mat4 projection = camera.getProjection();
        glm::mat4 view = camera.getView();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // // auto transforms = animator.GetFinalBoneMatrices();
        // // for (int i = 0; i < transforms.size(); ++i)
        // //     lightingShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	
        lightingShader.setMat4("model", model);
        // render the loaded model
        // local_model.Draw(lightingShader);

        glBindVertexArray(VAO); 
        // unsigned int ending_pos = positions[current_frame].size()+starting_pos;
        glDrawArrays(GL_POINTS, 0, flat_positions.size()/3);
        glDrawArrays(GL_LINES, 0, flat_positions.size()/3);
        GLenum e = glGetError();
        if (e != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "draw", e, e);
            exit(20);
        }

        // // update animation every 5 frames
        if (false) {
        // if (num_renders % ANIMATION_UPDATE_FRAMES == 0 && !should_stop) {
            std::cout << "at frame: " << current_frame << std::endl;
            current_model = base_model.rotate_self_by_rotations(name_rotation_list[current_frame], current_model);  
            flat_positions = flatten(current_model.vectorify_positions_in_order());
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * flat_positions.size(), &flat_positions[0], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0); 
            current_frame = (current_frame + 1) % name_rotation_list.size();
        //     std::cout << "at frame: " << current_frame << " attempting to print vertices from " << starting_pos << " to " << ending_pos << ". " << "frame has size: " << positions[current_frame].size()  << std::endl;
        //     starting_pos += positions[current_frame].size() / 3;
        //     current_frame = (current_frame + 1) % positions.size();
        //     if (current_frame == 0) {
        //         starting_pos = 0;
        //     }
            
        }

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