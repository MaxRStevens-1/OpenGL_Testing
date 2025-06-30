#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Model.h"
#include "AccelerationCamera.h"
#include "ShaderUtils.h"
#include "Shader.h"

#include <vector>
#include <iostream>

struct depthMapFBO_cubemap {
    unsigned int depthMapFBO;
    unsigned int depth_cubemap;
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int load_cubemap (std::vector<std::string> faces);
void render_skybox(Shader skyboxShader, unsigned int skybox_texture, unsigned int skyboxVAO);
void render_scene(
    Shader skybox_shader,
    unsigned int skybox_texture,
    unsigned int skyboxVAO,
    Shader shader,
    unsigned int planeVAO,
    unsigned int cube_texture,
    unsigned int cubeVAO,
    bool is_shadow_pass
);
void render_scene_cube_shadows(
    Shader shader,
    unsigned int cube_texture,
    unsigned int cubeVAO,
    unsigned int planeVAO,
    unsigned int move_frame
);
std::vector<float> calculate_tangent_and_bitangent_for_vert(
    std::vector<float> vertices,
    int indicies_per_point,
    int num_triangles
);
// glm::mat4 configure_shader_and_matrices();
void set_shadow_cube_shader(GeoShader cube_shader, int current_light);
void set_up_lights();
depthMapFBO_cubemap create_depth_cubemap();
void set_shader_from_light(PointLight light, int index, Shader shader);
// lights
const int NUM_LIGHTS = 3;

// shadow setup
glm::vec3 light_pos   = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 light_pos_2 = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 light_pos_3 = glm::vec3(-0.2f, -0.2f, -0.2f);

PointLight light_1;
PointLight light_2;
PointLight light_3;

// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;
const int MSSA_SAMPLES = 4;  
// camera
AccelerationCamera camera(SCR_WIDTH, SCR_HEIGHT);
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

bool move_light = false;
bool do_bump = true;
bool do_skybox = true;

const float near = 0.5f;
const float far = 50.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool move_box = true;
glm::vec3 translate_vec = glm::vec3(0.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, MSSA_SAMPLES);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    int maxUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
    std::cout << "Max texture units: " << maxUnits << std::endl;

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
    glEnable(GL_MULTISAMPLE);  

    // build and compile shaders
    // -------------------------
    // Shader shader("shadow_renderer");
    // Shader simple_depth("simple_depth");
    Shader skybox_shader("cubemap");

    // new shaders
    GeoShader depth_cube("depth_cubemap");
    Shader render_depth_cube("render_depth_cube");
    Shader normal_shadow_map("hdr_bloom_multi_light");
    // Shader normal_shadow_map("cube_shadow_map");
    
    Shader light_source_shader("lightSource");
    Shader hdr_shader("hdr_shader");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float quad_vertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    float cube_vertices[] = {
        // Positions          // Normals             // Texture Coords
        // Back face (Z = -1)
        -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

        // Front face (Z = +1)
        -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

        // Left face (X = -1)
        -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

        // Right face (X = +1)
         1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,

        // Bottom face (Y = -1)
        -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

        // Top face (Y = +1)
        -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
         1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f
    };

    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  25.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   25.0f, 25.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   25.0f, 0.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  25.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   25.0f, 0.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  25.0f			
    };


    std::vector<float> cube_verts {cube_vertices, cube_vertices + std::size(cube_vertices)};
    std::vector<float> new_cube_vec = calculate_tangent_and_bitangent_for_vert(
        cube_verts,
        8,
        12
    );

    const int cube_vert_size = new_cube_vec.size();

    float new_cube_verts[cube_vert_size];
    std::copy(new_cube_vec.begin(), new_cube_vec.end(), new_cube_verts);


    std::vector<float> plane_verts {planeVertices, planeVertices + std::size(planeVertices)};
    std::vector<float> new_plane_vec = calculate_tangent_and_bitangent_for_vert(
        plane_verts,
        8,
        2
    );
    const int plane_verts_size = new_plane_vec.size();
    float new_plane_verts[plane_verts_size];
    std::copy(new_plane_vec.begin(), new_plane_vec.end(), new_plane_verts);

    // for (int i = 0; i <new_cube_vec.size(); i++) {
    //     if (i % 14 == 0) {
    //         std::cout << std::endl;
    //     } 
    //     std::cout << new_cube_verts[i] << ", ";
    // }
    // std::cout << std::endl;

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cube_vert_size * sizeof(float), new_cube_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(new_plane_verts), &new_plane_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glBindVertexArray(0);
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // load textures
    // -------------
    unsigned int cube_texture  = loadTexture("./textures/advanced/bricks2_diff.jpg");
    unsigned int cube_bump = loadTexture("./textures/advanced/bricks2_normal.jpg");
    
    // create shadowmaps framebuffer depth buffer

    // creating shadow cubemap
    unsigned int depthMapFBO,   depthMapFBO_2,   depthMapFBO_3;
    unsigned int depth_cubemap, depth_cubemap_2, depth_cubemap_3;


    depthMapFBO_cubemap curr_info = create_depth_cubemap();
    depthMapFBO = curr_info.depthMapFBO;
    depth_cubemap = curr_info.depth_cubemap;
    curr_info = create_depth_cubemap();
    depthMapFBO_2 = curr_info.depthMapFBO;
    depth_cubemap_2 = curr_info.depth_cubemap;
    curr_info = create_depth_cubemap();
    depthMapFBO_3 = curr_info.depthMapFBO;
    depth_cubemap_3 = curr_info.depth_cubemap;

    // unsigned int depthMapFBO;
    // glGenFramebuffers(1, &depthMapFBO);  
    // unsigned int depth_cubemap;
    // glGenTextures(1, &depth_cubemap);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);
    // for (unsigned int i = 0; i < 6; ++i) {
    //     glTexImage2D(
    //         GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
    //         0, 
    //         GL_DEPTH_COMPONENT, 
    //         SHADOW_WIDTH, 
    //         SHADOW_HEIGHT, 
    //         0, 
    //         GL_DEPTH_COMPONENT, 
    //         GL_FLOAT, 
    //         NULL
    //     ); 
    // }

    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap, 0);
    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);  


    // load cubemap
    // -------------

    std::vector<std::string> faces = {
        "./textures/skyboxes/pretty_lake/right.jpg",
        "./textures/skyboxes/pretty_lake/left.jpg",
        "./textures/skyboxes/pretty_lake/top.jpg",
        "./textures/skyboxes/pretty_lake/bottom.jpg",
        "./textures/skyboxes/pretty_lake/front.jpg",
        "./textures/skyboxes/pretty_lake/back.jpg"
    };
    unsigned int skybox_texture = load_cubemap(faces);   

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // lets create hdr buffer
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // loading const data
    // ------------------

    // set up lights
    set_up_lights();


    // lets grab some const mat4's
    // shader configuration
    // --------------------
    normal_shadow_map.use();
    normal_shadow_map.setInt("diffuseTexture", 0);
    normal_shadow_map.setInt("normalMap", 1);
    normal_shadow_map.setInt("depthMap_1", 2);
    normal_shadow_map.setInt("depthMap_2", 3);
    normal_shadow_map.setInt("depthMap_3", 4);


    render_depth_cube.use();
    render_depth_cube.setInt("depthMap", 0);

    skybox_shader.use();
    skybox_shader.setInt("skybox", 0);
    // render loop
    // -----------

    float moved_frames = 0;
    while(!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // lets change light pos over time?
        if (move_box) {
            // light_1.position.z = static_cast<float>(sin(light_moved_frames * 0.001) * 3.0);
            // light_1.position.y = static_cast<float>(cos(light_moved_frames * 0.001) * 3.0);
            moved_frames++;
        }

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render to depth map
        glCullFace(GL_FRONT);


        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        for (int i = 0; i < NUM_LIGHTS; i++) {
            depth_cube.use();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            set_shadow_cube_shader(depth_cube, i);

            unsigned int current_depthMapFBO;
            unsigned int current_depth_cubemap;

            if (i == 0) {
                current_depthMapFBO = depthMapFBO;
                current_depth_cubemap = depth_cubemap;
            } else if (i == 1) {
                current_depthMapFBO = depthMapFBO_2;
                current_depth_cubemap = depth_cubemap_2;
            } else if (i == 2) {
                current_depthMapFBO = depthMapFBO_3;
                current_depth_cubemap = depth_cubemap_3;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, current_depthMapFBO);
                glClear(GL_DEPTH_BUFFER_BIT);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, current_depth_cubemap);
                render_scene_cube_shadows(
                    depth_cube,
                    cube_texture,
                    cubeVAO,
                    planeVAO,
                    moved_frames
                );
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        
        // use frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        // // render scene as normal
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // first lets draw skybox
        if (do_skybox) {
            render_skybox(skybox_shader, skybox_texture, skyboxVAO);
        }

        normal_shadow_map.use();

        for (int i = 0; i < NUM_LIGHTS; i++) {
            PointLight current_light;

            if (i == 0) {
                current_light = light_1;
            } else if (i == 1) {
                current_light = light_2;
            } else if (i == 2) {
                current_light = light_3;
            }

            set_shader_from_light(current_light, i, normal_shadow_map);
        }
        // normal_shadow_map.setVec3("lightPos[0]", light_1.position);
        // normal_shadow_map.setVec3("lightPos[1]", light_2.position);
        // normal_shadow_map.setVec3("lightPos[2]", light_3.position);
        normal_shadow_map.setVec3("viewPos", camera.camera_pos);
        normal_shadow_map.setFloat("far_plane", far);
        normal_shadow_map.setBool("do_normal_map", do_bump);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube_bump);  
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);      
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap_2);      
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap_3);      

        render_scene_cube_shadows(
            normal_shadow_map,
            cube_texture,
            cubeVAO,
            planeVAO,
            moved_frames
        );
        

        // ntoe we have to do this last, as otherwise the light would block itself
        // now im going to go simple & draw a light source box
        light_source_shader.use();

        glm::mat4 view = camera.getView();
        glm::mat4 projection = camera.getProjection();

        light_source_shader.setMat4("view", view);
        light_source_shader.setMat4("projection", projection);
        for (int i = 0; i < NUM_LIGHTS; i++) {
            PointLight current_light;

            if (i == 0) {
                current_light = light_1;
            } else if (i == 1) {
                current_light = light_2;
            } else if (i == 2) {
                current_light = light_3;
            }


            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 temp_color = (current_light.color * current_light.intensity);
            light_source_shader.setVec3("lightColor", temp_color);
            model = glm::translate(model, current_light.position);
            model = glm::scale (model, glm::vec3(0.1));
            light_source_shader.setMat4("model", model);
            glBindVertexArray(skyboxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        
        // lets unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // now lets render the scene from the framebuffer
        hdr_shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

struct vertex {
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float u;
    float v;

    std::vector<float> getList() {
        return {x,y,z,nx,ny,nz,u,v};
    }

};

vertex get_vertex_from_index(std::vector<float> vertices, int index) {
    vertex vert;
    vert.x = vertices[index];
    vert.y = vertices[index+1];
    vert.z = vertices[index+2];
    vert.nx = vertices[index+3];
    vert.ny = vertices[index+4];
    vert.nz = vertices[index+5];
    vert.u = vertices[index+6];
    vert.v = vertices[index+7];
    // };
    return vert;
}

std::vector<float> calculate_tangent_and_bitangent_for_vert(
    std::vector<float> vertices,
    int indicies_per_point,
    int num_triangles
) {

    std::vector<float> new_vertices;

    
    for (int i = 0; i < num_triangles; i++) {
        // this should give me the ind of the current triangle
        // so in a thing with 36 triangles
        // 0*8*3 = 0
        // 1 *8*3 =24
        glm::vec3 tangent = glm::vec3(0);
        glm::vec3 bitangent = glm::vec3(0);
        int current_index = i*indicies_per_point*3;

        vertex v0 = get_vertex_from_index(vertices, current_index);
        vertex v1 = get_vertex_from_index(vertices, current_index + (indicies_per_point*1)); 
        vertex v2 = get_vertex_from_index(vertices, current_index + (indicies_per_point*2)); 

        glm::vec3 delta_pos1 = glm::vec3(v1.x, v1.y, v1.z) - glm::vec3(v0.x, v0.y, v0.z);
        glm::vec3 delta_pos2 = glm::vec3(v2.x, v2.y, v2.z) - glm::vec3(v0.x, v0.y, v0.z);

        glm::vec2 delta_uv1 = glm::vec2(v1.u, v1.v) - glm::vec2(v0.u, v0.v);
        glm::vec2 delta_uv2 = glm::vec2(v2.u, v2.v) - glm::vec2(v0.u, v0.v);

        float denominator = (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
        if (abs(denominator) < 1e-8f) denominator = 1.0f;  // Avoid divide-by-near-zero
        float r = 1.0f / denominator;
        
        tangent += (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * r;
        bitangent += (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) * r; 

        // lets add everything to new list
        std::vector<float> local_list = v0.getList();
        new_vertices.insert(new_vertices.end(), local_list.begin(), local_list.end());
        new_vertices.push_back(tangent.x);
        new_vertices.push_back(tangent.y);
        new_vertices.push_back(tangent.z);

        new_vertices.push_back(bitangent.x);
        new_vertices.push_back(bitangent.y);
        new_vertices.push_back(bitangent.z);

        local_list = v1.getList();
        new_vertices.insert(new_vertices.end(), local_list.begin(), local_list.end());

        new_vertices.push_back(tangent.x);
        new_vertices.push_back(tangent.y);
        new_vertices.push_back(tangent.z);

        new_vertices.push_back(bitangent.x);
        new_vertices.push_back(bitangent.y);
        new_vertices.push_back(bitangent.z);

        local_list = v2.getList();
        new_vertices.insert(new_vertices.end(), local_list.begin(), local_list.end());        
        new_vertices.push_back(tangent.x);
        new_vertices.push_back(tangent.y);
        new_vertices.push_back(tangent.z);

        new_vertices.push_back(bitangent.x);
        new_vertices.push_back(bitangent.y);
        new_vertices.push_back(bitangent.z);
    }

    std::cout << "vertices went from size " << vertices.size() << " to " << new_vertices.size() << std::endl; 

    return new_vertices;
}



void set_shadow_cube_shader(GeoShader cube_shader, int current_light) {
    float aspect = (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT;

    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far); 

    glm::vec3 current_light_pos;
    if (current_light == 0) {
        current_light_pos = light_1.position;
    } else if (current_light == 1) {
        current_light_pos = light_2.position;
    } else if (current_light == 2) {
        current_light_pos = light_3.position;
    }

    // look at each face of the cube form light pos
    // order is right, left, top, bottom, near, far
    std::vector<glm::mat4> shadow_transforms;
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            current_light_pos, 
            current_light_pos + glm::vec3( 1.0, 0.0, 0.0), 
            glm::vec3(0.0,-1.0, 0.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            current_light_pos, 
            current_light_pos + glm::vec3(-1.0, 0.0, 0.0), 
            glm::vec3(0.0,-1.0, 0.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            current_light_pos, 
            current_light_pos + glm::vec3( 0.0, 1.0, 0.0), 
            glm::vec3(0.0, 0.0, 1.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            current_light_pos, 
            current_light_pos + glm::vec3( 0.0,-1.0, 0.0), 
            glm::vec3(0.0, 0.0,-1.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            current_light_pos, 
            current_light_pos + glm::vec3( 0.0, 0.0, 1.0), 
            glm::vec3(0.0,-1.0, 0.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
            glm::lookAt(
                current_light_pos, 
                current_light_pos + glm::vec3( 0.0, 0.0,-1.0), 
                glm::vec3(0.0,-1.0, 0.0)
            )
    );

    cube_shader.use();
    cube_shader.setFloat("far_plane", far);
    // light pos here is kept as a single as we are doing 1 cube map per light
    cube_shader.setVec3("light_pos", current_light_pos);
    cube_shader.setMat4("shadowMatrices[0]", shadow_transforms.at(0));
    cube_shader.setMat4("shadowMatrices[1]", shadow_transforms.at(1));
    cube_shader.setMat4("shadowMatrices[2]", shadow_transforms.at(2));
    cube_shader.setMat4("shadowMatrices[3]", shadow_transforms.at(3));
    cube_shader.setMat4("shadowMatrices[4]", shadow_transforms.at(4));
    cube_shader.setMat4("shadowMatrices[5]", shadow_transforms.at(5));

}

// glm::mat4 configure_shader_and_matrices() {
//     float near_plane = 1.0f, far_plane = 10.5f;
//     // this determines what is & what isn't in the depth map. Make sure all objects wanted are in here
//     glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

//     // having light source's position looking at the scenes center
//     glm::mat4 lightView = glm::lookAt(
//         light_pos, // light world pos
//         glm::vec3( 0.0f, 0.0f,  0.0f), // where its looking
//         glm::vec3( 0.0f, 1.0f,  0.0f)  // up direction
//     ); 

//     glm::mat4 lightSpaceMatrix = lightProjection * lightView;
//     return lightSpaceMatrix; 
// }

void render_scene_cube_shadows(
    Shader shader,
    unsigned int cube_texture,
    unsigned int cubeVAO,
    unsigned int planeVAO,
    unsigned int move_frames
) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getView();
    glm::mat4 projection = camera.getProjection();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    model = glm::translate(model, glm::vec3(10.0f, -3.0f, -20.0));
    model = glm::scale(model, glm::vec3(5.0f));
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 4);

    if (move_box) {
        if (move_frames % 1000 > 500) {
            translate_vec.x += static_cast<float>(sin((move_frames)  * 0.001) * 0.01f);
        } else {
            translate_vec.x -= static_cast<float>(sin((move_frames) * 0.001) * 0.01f);
        }
    }



    // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    // glEnable(GL_CULL_FACE); 
    // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    // shader.setBool("reverse_normals", true);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // shader.setBool("reverse_normals", false); // and of course disable it
    // glDisable(GL_CULL_FACE);
    // cubes
    glBindVertexArray(cubeVAO);
    model = glm::mat4(1.0f);
    model = glm::translate(model, translate_vec + glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, translate_vec + glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, translate_vec + glm::vec3(-1.5f, -3.0f, -3.0));
    model = glm::rotate(model, (float)glfwGetTime() * -1.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void render_scene(
    Shader skybox_shader,
    unsigned int skybox_texture,
    unsigned int skyboxVAO,
    Shader shader,
    unsigned int planeVAO,
    unsigned int cube_texture,
    unsigned int cubeVAO,
    bool do_skybox
) {
    // skybox shouldn't be calc'd in depthpass
    if (!do_skybox) {
        // we want to set up skybox first
        render_skybox(skybox_shader, skybox_texture, skyboxVAO);
        // and then draw the rest of the scene on top of it
    }

    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getView();
    glm::mat4 projection = camera.getProjection();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    // floor
    glBindVertexArray(planeVAO);
    // glBindTexture(GL_TEXTURE_2D, cube_texture);
    model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // reset
    // glActiveTexture(GL_TEXTURE0);
    // cubes
    glBindVertexArray(cubeVAO);
    // glBindTexture(GL_TEXTURE_2D, cube_texture); 	
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


depthMapFBO_cubemap create_depth_cubemap() {
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);  
    unsigned int depth_cubemap;
    glGenTextures(1, &depth_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, 
            GL_DEPTH_COMPONENT, 
            SHADOW_WIDTH, 
            SHADOW_HEIGHT, 
            0, 
            GL_DEPTH_COMPONENT, 
            GL_FLOAT, 
            NULL
        ); 
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    depthMapFBO_cubemap return_obj;
    return_obj.depth_cubemap = depth_cubemap;
    return_obj.depthMapFBO = depthMapFBO;

    return return_obj;
}

std::string get_light_str(int index) {
    return "lights[" + std::to_string(index) + "]."; 
}

void set_shader_from_light(PointLight light, int index, Shader shader) {
    shader.setVec3(get_light_str(index) + "position", light.position);

    shader.setVec3(get_light_str(index) + "color", light.color);
    shader.setFloat(get_light_str(index) + "intensity", light.intensity);
    

    shader.setFloat(get_light_str(index) + "constant", light.constant);
    shader.setFloat(get_light_str(index) + "linear", light.linear);
    shader.setFloat(get_light_str(index) + "quadratic", light.quadratic);

}

void set_up_lights() {
    // light 1
    light_1.position = light_pos;
    light_1.linear = 0.07f;
    light_1.quadratic = 0.017f;
    light_1.color = glm::vec3(0.95f, 0.0f, 0.0f);
    light_1.intensity = 0.5f;
    // light_1.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    // light_1.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // light 2
    light_2.position = light_pos_2;
    light_2.linear = 0.07f;
    light_2.quadratic = 0.017f;
    light_2.color = glm::vec3(1.0f);
    light_2.intensity = 0.5f;

    // light_2.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    // light_2.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // light 3
    light_3.position = light_pos_3;
    light_3.linear = 0.07f;
    light_3.quadratic = 0.017f;
    light_3.color = glm::vec3(0.0f, 0.0f, 0.95f);
    light_3.intensity = 0.5f;
    // light_3.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    // light_3.specular = glm::vec3(1.0f, 1.0f, 1.0f);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } 

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        move_box = false;
        std::cout << "setting move box to: false" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        move_box = true;
        std::cout << "setting move box to: true" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_INSERT) == GLFW_PRESS) {
        do_skybox = true;
        std::cout << "setting bump to: true" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
        do_skybox = false;
        std::cout << "setting bump to: true" << std::endl;
    }
    camera.processInputForCamera(window);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    camera.processMouseInputForCamera(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processScrollForCamera(xoffset, yoffset);
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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

// render skybox
void render_skybox(
    Shader skybox_shader,
    unsigned int skybox_texture,
    unsigned int skyboxVAO
) {
    glDepthMask(GL_FALSE);
    skybox_shader.use();

    // we don't want translate skybox, we want it to appear stil.
    // so we are removing translation portion of the matrix
    glm::mat4 view = glm::mat4(glm::mat3(camera.getView()));
    glm::mat4 projection = camera.getProjection();

    skybox_shader.setMat4("view", view);
    skybox_shader.setMat4("projection", projection);
    skybox_shader.setInt("skybox", 0);
    

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

// util for loading a cubemap
unsigned int load_cubemap (std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}  