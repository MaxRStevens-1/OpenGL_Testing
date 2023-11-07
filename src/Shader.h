#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include "ShaderUtils.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


class Shader 
{
public: 
    unsigned int ID;

    Shader(const std::string directoryName) {
        std::vector<std::string> shader_files_raw = loadShadersFromDirectory(directoryName);
        const char* vertexShaderSource = shader_files_raw[0].c_str();
        const char* fragmentShaderSource = shader_files_raw[1].c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexShaderSource, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
                infoLog << std::endl;
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
                infoLog << std::endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        // print linking errors if any
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
                infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }   
    void use() {
        glUseProgram(ID);
    }

    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat (const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec3(const std::string &name, const float a, const float b, const float c) const {
        // glm::vec3 new_vec = glm::vec3(a, b, c);
        glUniform3f(glGetUniformLocation(ID, name.c_str()), a, b, c);
    }

    void setVec3(const std::string &name, glm::vec3 &new_vec) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &new_vec[0]);
    }

    void setMat4(const std::string &name,  const glm::mat4 &mat4) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, false, &mat4[0][0]);
    }

};


#endif