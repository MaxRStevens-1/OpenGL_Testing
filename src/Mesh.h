#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "Shader.h"

const int BASE_SHINY_MULTIPLE = 128;


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};


class Mesh {
    public:
        // mesh data
        std::vector<Vertex> vertices;
        std::vector<Texture> textures;

        Mesh (std::vector<Vertex> vertices, std::vector<Texture> textures) {
            this->vertices = vertices;
            this->textures = textures;

            setupMesh();
        }

        void Draw(Shader &shader) {
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr   = 1;
            unsigned int bumpNr   = 1;
            for(unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
                // retrieve texture number (the N in diffuse_textureN)
                std::string number;
                std::string name = textures[i].type;
                if (name == "texture_diffuse") 
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular") 
                    number = std::to_string(specularNr++);
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++);
                else if (name == "texture_bump")
                    number = std::to_string(bumpNr++);
                shader.setFloat(("material." + name + number).c_str(), i);
                GLenum e = glGetError();
                if (e != GL_NO_ERROR) {
                    fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "setting material float", e, e);
                    exit(20);
                }
            }
            glActiveTexture(GL_TEXTURE0);

            shader.setFloat("material.shininess", .25f*(BASE_SHINY_MULTIPLE));

            // shader.setFloat("")
            // draw mesh
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            glBindVertexArray(0);

            GLenum e = glGetError();
            if (e != GL_NO_ERROR) {
                fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "draw arrays", e, e);
                exit(20);
            }
        }

    private:
        // render data
        unsigned int VAO, VBO;

        void setupMesh() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            
            
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
            // vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        
            GLenum e = glGetError();
            if (e != GL_NO_ERROR) {
                fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "mesh setup", e, e);
                exit(20);
            }
        }
};

#endif