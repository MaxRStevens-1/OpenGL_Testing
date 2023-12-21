/*
#ifndef MODEL_H
#define MODEL_H

#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "Shader.h"
#include "Mesh.h"

class Model {
    public: 
        Model(std::string path) {
            load_model(path);
        }
        void Draw(Shader &shader) {
            for (unsigned int i = 0; i < meshes.size(); i++) {
                meshes[i].Draw(shader);
            }
        }
    private: 
        // model data
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded; 


    void load_model(std::string path) {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(&path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));
        std::cout << "directory is " << directory;

        // process ASSIMP's root node recursively
        process_node(scene->mRootNode, scene);
    }

        void process_node(aiNode *node, const aiScene *scene) {
            // process all the node’s meshes (if any)
            for(unsigned int i = 0; i < node->mNumMeshes; i++) {
               aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(process_mesh(mesh, scene));
            }
            
            // then do the same for each of its children
            for(unsigned int i = 0; i < node->mNumChildren; i++) {
                process_node(node->mChildren[i], scene);
            }
        }

        
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene) {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;

                glm::vec3 local_vector;
                local_vector.x = mesh->mVertices[i].x;
                local_vector.y = mesh->mVertices[i].y;
                local_vector.z = mesh->mVertices[i].z;
                vertex.Position = local_vector;

                local_vector.x = mesh->mNormals[i].x;
                local_vector.y = mesh->mNormals[i].y;
                local_vector.z = mesh->mNormals[i].z;
                vertex.Normal = local_vector;

                if (mesh->mTextureCoords[0]) {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;
                } else {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }
                vertices.push_back(vertex);
                
            }

            // process indices
            for(unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for(unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }  

            // process material
            if(mesh->mMaterialIndex >= 0) {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Texture> diffuseMaps = loadMaterialTextures(material, 
                                                    aiTextureType_DIFFUSE, "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
                std::vector<Texture> specularMaps = loadMaterialTextures(material, 
                                                    aiTextureType_SPECULAR, "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }  

            return Mesh(vertices, indices, textures);

            
        }

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
            std::vector<Texture> textures;
            for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                bool skip = false;
                for(unsigned int j = 0; j < textures_loaded.size(); j++)
                {
                    if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                    {
                        textures.push_back(textures_loaded[j]);
                        skip = true; 
                        break;
                    }
                }
                if(!skip)
                {   // if texture hasn't been loaded already, load it
                    Texture texture;
                    texture.id = TextureFromFile(str.C_Str(), directory);
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    textures.push_back(texture);
                    textures_loaded.push_back(texture); // add to loaded textures
                }
            }
            return textures;
        }  

        unsigned int TextureFromFile(const char *path, const std::string &directory) {
            std::string filename = std::string(path);
            filename = directory + '/' + filename;
            std::cout << path << "\n";
            std::cout << directory << "\n";
            std::cout << filename << "\n";
            std::cout << "_________" << "\n";

            unsigned int textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Texture failed to load at path: " << path << std::endl;
                stbi_image_free(data);
            }

            return textureID;
        }
};

#endif
*/