#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <assimp/material.h>
#include <assimp/scene.h>

#include "Shader.h"
#include "Mesh.h"

class Model {
    public: 
        Model(char* path) {
            loadModel(path);
        }
        void Draw(Shader &shader);
    private: 
        // model data
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif