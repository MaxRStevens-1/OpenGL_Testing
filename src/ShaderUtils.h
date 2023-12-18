#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "stb_image.h"



std::string SHADER_PATH = "./src/shaders/";


std::string VERTEX_EXTENSION = ".vert";
std::string FRAGMENT_EXTENSION = ".frag";


/**
 * @brief given an name of a shader directory, reads from files into text
 * 
 * @param directory_name 
 * @return an string vector in format [vertex_shader, fr agment_shader]
 */
std::vector<std::string> loadShadersFromDirectory(std::string directory_name) {
    std::vector<std::string> shaders_vec(2);


    std::ifstream vertex(SHADER_PATH+directory_name+"/"+directory_name+VERTEX_EXTENSION);
    if (!vertex) {
        std::cout << "Failed to open file: " + SHADER_PATH+directory_name+"/"+directory_name+VERTEX_EXTENSION +"\n";
    }
    std::stringstream vertex_buffer;
    vertex_buffer << vertex.rdbuf();
    shaders_vec[0] = vertex_buffer.str();   

    if (vertex_buffer.str().size() == 0) {
        std::cout << "File size is 0 for vertex shader.\n";
    } 

    std::ifstream fragment(SHADER_PATH+directory_name+"/"+directory_name+FRAGMENT_EXTENSION);
    if (!fragment) {
        std::cout << "Failed to open file: " + SHADER_PATH+directory_name+"/"+directory_name+FRAGMENT_EXTENSION +"\n";
    }
    std::stringstream fragment_buffer;
    fragment_buffer << fragment.rdbuf();
    shaders_vec[1] = fragment_buffer.str();

    if (fragment_buffer.str().size() == 0) {
        std::cout << "File size for fragment shader is 0\n";
    }

    return shaders_vec;    
}

int generateTextureFromPath(std::string path, unsigned int texture) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    unsigned char *data = stbi_load(path.c_str(), &width, &height, 
        &nrChannels, 0);    
    
    // determing which color channel to select
    if (path.find(".png") != std::string::npos) {
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
    } else if (path.find(".jpg") != std::string::npos) {
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
    }
    stbi_image_free(data);
    return texture;
}

#endif