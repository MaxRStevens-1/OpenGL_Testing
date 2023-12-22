
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

#ifndef MODEL_H
#define MODEL_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <map>

#include "Mesh.h"

std::string MODEL_PATH = "./src/models/";

class Model {
    public:
        std::string path;

        Model(std::string local_path) {
            path = local_path;
            load_model(local_path);
        }

        void Draw(Shader &shader) {
            for (unsigned int i = 0; i < meshes.size(); i++) {
                meshes[i].Draw(shader);
            }
        }
    private:
        std::vector<Mesh> meshes;
        std::vector<Texture> textures_loaded;

        // implementation largerly bastardization of opengl assimp / tinyobjloader examples
        void load_model(std::string path) {
            tinyobj::ObjReaderConfig reader_config;
            reader_config.mtl_search_path = ""; // Path to material files

            tinyobj::ObjReader reader;

            std::cout << "PATH IS " << path << std::endl;

            if (!reader.ParseFromFile(MODEL_PATH+path, reader_config)) {
                if (!reader.Error().empty()) {
                    std::cerr << "TinyObjReader: " << reader.Error();
                }
                exit(1);
            }

            if (!reader.Warning().empty()) {
                std::cout << "TinyObjReader: " << reader.Warning();
            }

            auto& attrib = reader.GetAttrib();
            auto& shapes = reader.GetShapes();
            auto& materials = reader.GetMaterials();

            // loops over shapes
            for (size_t s = 0; s < shapes.size(); s++) {
                process_mesh(attrib, shapes[s], materials);
            }
        }

        void process_mesh(tinyobj::attrib_t attrib, tinyobj::shape_t shape, std::vector<tinyobj::material_t> materials)
        {
            // std::cout << "SIZE OF MATERIALOS IS:" << materials.size() << std::endl;

            std::vector<Vertex> vertices;
            std::vector<Texture> textures;
            
            std::map<std::string, bool> texture_names;

            size_t index_offset = 0;
            // goes over polygon faces
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shape.mesh.num_face_vertices[f]);
                // goes over vertices in face
                for (size_t v = 0; v < fv; v++) {
                    Vertex local_vertex;
                    // gets vertex
                    
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                    local_vertex.Position =  glm::vec3(vx, vy, vz);

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                        tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                        tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

                        local_vertex.Normal = glm::vec3(nx, ny, nz);
                    } else {
                        local_vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                        tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                        local_vertex.TexCoords = glm::vec2(tx, ty);
                    } else {
                        local_vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                    }

                    vertices.push_back(local_vertex);
                }

                index_offset += fv;

                // per face material
                tinyobj::material_t local_material = materials[shape.mesh.material_ids[f]];
            }

            for (size_t i = 0; i < materials.size(); i++) {
                auto pos = path.find_last_of('/');
                if (pos != std::string::npos) {
                    std::string dir = path.substr(0, pos);
                    // get texture of diffuse
                    std::string diffuse_path = MODEL_PATH+dir+"/"+materials[i].diffuse_texname;
                    std::vector<Texture> diffuse_textures = load_texture(diffuse_path, "texture_diffuse");
                    textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());
                    // std::cout << "attempting to create texture of path: " + full_path << std::endl;
                    // unsigned int texture_id = texture_from_file(full_path);
                }
            }
            // now create mesh and add to mesh list
            Mesh mesh(vertices, textures);
            meshes.push_back(mesh);
        }

        std::vector<Texture> load_texture(std::string path, std::string texture_type) {
            std::vector<Texture> textures;
            bool skip = false;
            for (unsigned int i = 0; i < textures_loaded.size(); i++) {
                if (std::strcmp(textures_loaded[i].path.data(), path.data()) == 0) {
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                Texture texture;
                texture.id = texture_from_file(path);
                texture.type = texture_type;
                texture.path = path;
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }

            return textures;
        }

        unsigned int texture_from_file(std::string path) {
            unsigned int textureID;
            glGenTextures(1, &textureID);


            int width, height, nrComponents;
            unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
            if (data) {
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
            else {
                std::cout << "Texture failed to load at path: " << path << std::endl;
                stbi_image_free(data);
            }

            return textureID;
        }

};

#endif