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

#include <iostream>

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
    unsigned int cubeVAO
);
glm::mat4 configure_shader_and_matrices();
void set_shadow_cube_shader(GeoShader cube_shader);

// shadow setup
glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 0.0f);
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

bool move_light = true;

const float near = 1.0f;
const float far = 25.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
    Shader cube_shadow_map("cube_shadow_map");
    Shader light_source_shader("lightSource");
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
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
    };
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f			
    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
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
    unsigned int cube_texture  = loadTexture("./textures/advanced/wood.png");
    
    // create shadowmaps framebuffer depth buffer

    // creating shadow cubemap
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
    // create depth texture
    // const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;


    // unsigned int depth_map;
    // glGenTextures(1, &depth_map);
    // glBindTexture(GL_TEXTURE_2D, depth_map);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
    //             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

    // // lets attach it to framebuffer's depth buffer
    // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
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

    // loading const data
    // ------------------
    // lets grab some const mat4's
    glm::mat4 light_space_matrix = configure_shader_and_matrices();

    // shader configuration
    // --------------------
    cube_shadow_map.use();
    cube_shadow_map.setInt("diffuseTexture", 0);
    cube_shadow_map.setInt("depthMap", 1);
    // shader.setInt("shadowMap", 1);
    render_depth_cube.use();
    render_depth_cube.setInt("depthMap", 0);
    // render loop
    // -----------

    float light_moved_frames = 0;
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
        if (move_light) {
            light_pos.z = static_cast<float>(sin(light_moved_frames * 0.01) * 3.0);
            light_pos.y = static_cast<float>(cos(light_moved_frames * 0.01) * 3.0);
            light_moved_frames++;
        }

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render to depth map
        glCullFace(GL_FRONT);


        depth_cube.use();
        set_shadow_cube_shader(depth_cube);
        // light_space_matrix = configure_shader_and_matrices();
        // simple_depth.setMat4("lightSpaceMatrix", light_space_matrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);
            render_scene_cube_shadows(
                depth_cube,
                cube_texture,
                cubeVAO
            );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glCullFace(GL_BACK); 
        // glCullFace(GL_FRONT);


        // DEBUG_RENDER_DEPTH_FROM_LIGHT
        // glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // render_depth_cube.use();
        // render_depth_cube.setFloat("near_plane", 1.0f);
        // render_depth_cube.setFloat("far_plane", 10.5f);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);        
        //     glBindVertexArray(quadVAO);
        //     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, 0);        

        

        // // render scene as normal
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        cube_shadow_map.use();
        cube_shadow_map.setVec3("lightPos", light_pos);
        cube_shadow_map.setVec3("viewPos", camera.camera_pos);
        cube_shadow_map.setFloat("far_plane", far);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);        

        render_scene_cube_shadows(
            cube_shadow_map,
            cube_texture,
            cubeVAO
        );

        // now im going to go simple & draw a light source box
        light_source_shader.use();
        glm::mat4 view = camera.getView();
        glm::mat4 projection = camera.getProjection();
        glm::mat4 model = glm::mat4(1.0f);
        light_source_shader.setMat4("view", view);
        light_source_shader.setMat4("projection", projection);
        model = glm::translate(model, light_pos);
        model = glm::scale (model, glm::vec3(0.1));
        light_source_shader.setMat4("model", model);
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
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

void set_shadow_cube_shader(GeoShader cube_shader) {
    float aspect = (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT;

    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far); 

    // look at each face of the cube form light pos
    // order is right, left, top, bottom, near, far
    std::vector<glm::mat4> shadow_transforms;
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            light_pos, 
            light_pos + glm::vec3( 1.0, 0.0, 0.0), 
            glm::vec3(0.0,-1.0, 0.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            light_pos, 
            light_pos + glm::vec3(-1.0, 0.0, 0.0), 
            glm::vec3(0.0,-1.0, 0.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            light_pos, 
            light_pos + glm::vec3( 0.0, 1.0, 0.0), 
            glm::vec3(0.0, 0.0, 1.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            light_pos, 
            light_pos + glm::vec3( 0.0,-1.0, 0.0), 
            glm::vec3(0.0, 0.0,-1.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
        glm::lookAt(
            light_pos, 
            light_pos + glm::vec3( 0.0, 0.0, 1.0), 
            glm::vec3(0.0,-1.0, 0.0)
        )
    );
    shadow_transforms.push_back(
        shadowProj * 
            glm::lookAt(
                light_pos, 
                light_pos + glm::vec3( 0.0, 0.0,-1.0), 
                glm::vec3(0.0,-1.0, 0.0)
            )
    );

    cube_shader.use();
    cube_shader.setFloat("far_plane", far);
    cube_shader.setVec3("light_pos", light_pos);
    cube_shader.setMat4("shadowMatrices[0]", shadow_transforms.at(0));
    cube_shader.setMat4("shadowMatrices[1]", shadow_transforms.at(1));
    cube_shader.setMat4("shadowMatrices[2]", shadow_transforms.at(2));
    cube_shader.setMat4("shadowMatrices[3]", shadow_transforms.at(3));
    cube_shader.setMat4("shadowMatrices[4]", shadow_transforms.at(4));
    cube_shader.setMat4("shadowMatrices[5]", shadow_transforms.at(5));

}

glm::mat4 configure_shader_and_matrices() {
    float near_plane = 1.0f, far_plane = 10.5f;
    // this determines what is & what isn't in the depth map. Make sure all objects wanted are in here
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

    // having light source's position looking at the scenes center
    glm::mat4 lightView = glm::lookAt(
        light_pos, // light world pos
        glm::vec3( 0.0f, 0.0f,  0.0f), // where its looking
        glm::vec3( 0.0f, 1.0f,  0.0f)  // up direction
    ); 

    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix; 
}

void render_scene_cube_shadows(
    Shader shader,
    unsigned int cube_texture,
    unsigned int cubeVAO
) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getView();
    glm::mat4 projection = camera.getProjection();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    
    model = glm::scale(model, glm::vec3(5.0f));
    shader.setMat4("model", model);
    glBindVertexArray(cubeVAO);
    // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    // glDisable(GL_CULL_FACE); 
    // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    shader.setBool("reverse_normals", true);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    shader.setBool("reverse_normals", false); // and of course disable it
    // glEnable(GL_CULL_FACE);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
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
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
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


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } 

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        move_light = !move_light;
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
    skybox_shader.setInt("skybox", 1);
    

    glBindVertexArray(skyboxVAO);
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