
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

#include "Mesh.h"

class Model {
    public:
        Model(std::string path) {
            load_model(path);
        }

        void Draw(Shader &shader) {

        }
    private:
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;


        // implementation largerly bastardization of opengl assimp / tinyobjloader examples
        void load_model(std::string path) {
            tinyobj::ObjReaderConfig reader_config;
            reader_config.mtl_search_path = ""; // Path to material files

            tinyobj::ObjReader reader;

            std::cout << "PATH IS " << path << std::endl;

            if (!reader.ParseFromFile(path, reader_config)) {
                if (!reader.Error().empty()) {
                    std::cerr << "TinyObjReader: " << reader.Error();
                }
                // exit(1);
            }
            std::cout << "PARSED" << path << std::endl;

            if (!reader.Warning().empty()) {
                std::cout << "TinyObjReader: " << reader.Warning();
            }

            auto& attrib = reader.GetAttrib();
            auto& shapes = reader.GetShapes();
            auto& materials = reader.GetMaterials();

            // loops over shapes
            for (size_t s = 0; s < shapes.size(); s++) {
                processMesh(attrib, shapes[s], materials);
            }
        }

        void processMesh(tinyobj::attrib_t attrib, tinyobj::shape_t shape, std::vector<tinyobj::material_t> materials)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            
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
                // if (local_material.)
                Texture local_texture;
                std::cout << "READING TEX NAMES" << std::endl;
                // std::cout << local_material.diffuse_texname << std::endl;
                // std::cout << local_material.specular_texname << std::endl;
                // std::cout << local_material.bump_texname << std::endl;
                // std::cout << local_material.normal_texname << std::endl;
                
            }

        }

};

#endif