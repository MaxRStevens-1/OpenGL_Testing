#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>





std::string SHADER_PATH = "./src/shaders/";


std::string VERTEX_EXTENSION = ".vert";
std::string FRAGMENT_EXTENSION = ".frag";


/**
 * @brief given an name of a shader directory, reads from files into text
 * 
 * @param directory_name 
 * @return an string vector in format [vertex_shader, fragment_shader]
 */
std::vector<std::string> loadShadersFromDirectory(std::string directory_name) {
    std::vector<std::string> shaders_vec(2);


    std::ifstream vertex(SHADER_PATH+directory_name+"/"+directory_name+VERTEX_EXTENSION);
    std::stringstream vertex_buffer;
    vertex_buffer << vertex.rdbuf();
    shaders_vec[0] = vertex_buffer.str();    

    std::ifstream fragment(SHADER_PATH+directory_name+"/"+directory_name+FRAGMENT_EXTENSION);
    std::stringstream fragment_buffer;
    fragment_buffer << fragment.rdbuf();
    shaders_vec[1] = fragment_buffer.str();

    return shaders_vec;    
}

#endif