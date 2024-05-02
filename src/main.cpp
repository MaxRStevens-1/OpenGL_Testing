#ifndef MAIN_CPP
#define MAIN_CPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

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
#include "skeleton_loader_helper.hpp"
#include "rotations_test.hpp"
#include "dancingVampireUtils.hpp"
#include "AssimpGLMHelpers.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const unsigned int ANIMATION_UPDATE_FRAMES = 5;

unsigned int current_frame = 0;

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
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        current_frame += 1;
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


void getBaseWorldPosFromChildren(AssimpNodeData currentNode, glm::mat4 transform, float scale) {
    const auto localTransform = transform * currentNode.transformation;
    const auto localPosition = localTransform * glm::vec4(0,0,0,1) * scale;

    std::string name = currentNode.name;
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::toupper(c); });
        
    if (joint_to_index.find(name) != joint_to_index.end()) {
        vamp_pos[joint_to_index[name]] = position(localPosition.x, localPosition.y, localPosition.z);
        std::cout << name << ": " << position(localPosition.x, localPosition.y, localPosition.z).toString() << std::endl;

        for (auto child : currentNode.children) {
            getBaseWorldPosFromChildren(child, localTransform, scale);
        }
    }
}

void getWorldPositionFromBones(AssimpNodeData baseNode, float scale) {
    const auto localTransform = baseNode.transformation;
    const auto localPosition = localTransform * glm::vec4(0,0,0,1) * scale;

    // getting name as all caps
    std::string name = baseNode.name;
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::toupper(c); });

    vamp_pos[joint_to_index[name]] = position(localPosition.x, localPosition.y, localPosition.z);

    std::cout << name << ": " << position(localPosition.x, localPosition.y, localPosition.z).toString() << std::endl;

    for (auto child : baseNode.children) {
        getBaseWorldPosFromChildren(child, localTransform, scale);
    }
}

void dump_vampire_into_file(bodymodel vamp) {

    std::ofstream vamp_dump_file ("vamp_dump.txt");
    std::string vamp_line = "[";
    for (int i = 0; i < vamp.positions.size(); i++) {
    // for (position pos : adjusted_base_model.positions) {
        position pos = vamp.positions[i];
        vamp_line += pos.toString();
        if (i != vamp.positions.size() - 1)
            vamp_line += ", ";
    }
    vamp_line += "]";
    vamp_dump_file << vamp_line << std::endl;
    vamp_dump_file.close();
    std::cout << "POSITIONS DUMP: " << std::endl;
    std::cout << vamp_line << std::endl;
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
    // auto [base_model, name_rotation_list] = load_blaze_model_from_file("ymca_4_adjusted_matrix.txt");
    // bodymodel current_model = base_model;
    // std::vector<std::vector<float>> positions = base_model.vectorify_positions_in_order();
    // // flatten vertices seperate by time to single list for easy retrieval
    // std::vector<float> flat_positions = flatten(positions);


    auto [base_model, name_rotation_list] = load_vamp_model_from_file("vamp_ymca_blaze_vamp.txt");
    bodymodel current_model = base_model;
    std::vector<std::vector<float>> positions = base_model.vectorify_positions_in_order();
    // flatten vertices seperate by time to single list for easy retrieval
    std::vector<float> flat_positions = flatten(positions);

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
    // animator.UpdateAnimation(5.0f);

    
    bodymodel dancing_vampire = create_local_dancing_vampire_model();
    // auto baseBone = danceAnimation.FindBone("HIPS");
    std::cout << "_____" << std::endl;
    auto baseNode = danceAnimation.m_RootNode.children[0];
    getWorldPositionFromBones(baseNode, 0.01f);

    
    // auto index_name_map = hashtable_from_const();
    // auto info_map = danceAnimation.GetBoneIDMap();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(FileSystem::getPath(path), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return -1;
    }

    dancing_vampire.set_positions(vamp_pos);
    std::cout << dancing_vampire.toString() << std::endl;
    std::cout << "___________________" << std::endl;
    std::cout << current_model.toString() << std::endl;
    // std::cout << danceAnimation.FindBone("Hips").
    // flat_positions = flatten(dancing_vampire.vectorify_positions_in_order());

    // dump_vampire_into_file(dancing_vampire);
    bodymodel blaze_model = create_adjusted_blaze_model();

    // current_model = base_model.rotate_self_by_rotations(name_rotation_list[0], current_model);
    auto [new_current_model, translation_map] = apply_rotations_to_vamp_model(name_rotation_list[0], current_model, blaze_model);
    current_model = new_current_model;
    std::cout << "_____________" << std::endl;
    for (bone bj : dancing_vampire.base_bones) {
        position new_pos;
        if (translation_map.find(bj.name) != translation_map.end()) 
            new_pos = translation_map[bj.name].add(dancing_vampire.positions[bj.child_index]);
        else 
            new_pos = dancing_vampire.positions[bj.child_index];

        std::cout << "joint: " << bj.name << " pos: " << dancing_vampire.positions[bj.child_index].toString() 
            << ", new pos: "  << new_pos.toString() << std::endl;
        for (bone cj : dancing_vampire.bones_flow[bj]) {
        position new_pos;
        if (translation_map.find(cj.name) != translation_map.end()) 
            new_pos = translation_map[cj.name].add(dancing_vampire.positions[cj.child_index]);
        else 
            new_pos = dancing_vampire.positions[cj.child_index];

        std::cout << "joint: " << cj.name << " pos: " << dancing_vampire.positions[cj.child_index].toString() 
            << ", new pos: "  << new_pos.toString() << std::endl;
        }
    }

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
        glDrawArrays(GL_POINTS, 0, flat_positions.size()/3);
        glDrawArrays(GL_LINES, 0, flat_positions.size()/3);
        GLenum e = glGetError();
        if (e != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "draw", e, e);
            exit(20);
        }

        // // update animation every 5 frames
        // if (false) {
        if (true) {
            std::cout << "at frame: " << current_frame << std::endl;
            current_frame %= name_rotation_list.size();
            auto [new_current_model, new_translation_map] = apply_rotations_to_vamp_model(name_rotation_list[current_frame], base_model, blaze_model);
            current_model = new_current_model;
            translation_map = new_translation_map;
            // current_model = base_model.rotate_self_by_rotations(name_rotation_list[current_frame], current_model);  
            flat_positions = flatten(current_model.vectorify_positions_in_order());
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * flat_positions.size(), &flat_positions[0], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0); 
            if (num_renders % ANIMATION_UPDATE_FRAMES == 0 && !should_stop)
                current_frame = (current_frame + 1);
            // starting_pos += positions[current_frame].size() / 3;
            // current_frame = (current_frame + 1) % positions.size();
            // if (current_frame == 0) {
            //     starting_pos = 0;
            // }
            
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