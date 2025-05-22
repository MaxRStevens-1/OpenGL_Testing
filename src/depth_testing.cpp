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
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int load_cubemap (std::vector<std::string> faces);
void render_skybox(Shader skyboxShader, unsigned int skybox_texture, unsigned int skyboxVAO);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
AccelerationCamera camera(SCR_WIDTH, SCR_HEIGHT);
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

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

    // build and compile shaders
    // -------------------------
    Shader space_3d("space_3d");
    Shader instanced_space_3d("instance_space_3d");

    Shader skybox_shader("cubemap");

    // create models
    Model planet_model("planet/planet.obj");
    Model rock_model("rock/rock.obj");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // float cubeVertices[] = {
    //     // positions          // texture Coords
    //     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    //     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    //     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //     -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    //      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    //     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    //      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    //     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    // };
    // float planeVertices[] = {
    //     // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
    //      5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    //     -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    //     -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

    //      5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    //     -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
    //      5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    // };
    // float skyboxVertices[] = {
    //     // positions          
    //     -1.0f,  1.0f, -1.0f,
    //     -1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,
    //      1.0f,  1.0f, -1.0f,
    //     -1.0f,  1.0f, -1.0f,
    
    //     -1.0f, -1.0f,  1.0f,
    //     -1.0f, -1.0f, -1.0f,
    //     -1.0f,  1.0f, -1.0f,
    //     -1.0f,  1.0f, -1.0f,
    //     -1.0f,  1.0f,  1.0f,
    //     -1.0f, -1.0f,  1.0f,
    
    //      1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,
    
    //     -1.0f, -1.0f,  1.0f,
    //     -1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f, -1.0f,  1.0f,
    //     -1.0f, -1.0f,  1.0f,
    
    //     -1.0f,  1.0f, -1.0f,
    //      1.0f,  1.0f, -1.0f,
    //      1.0f,  1.0f,  1.0f,
    //      1.0f,  1.0f,  1.0f,
    //     -1.0f,  1.0f,  1.0f,
    //     -1.0f,  1.0f, -1.0f,
    
    //     -1.0f, -1.0f, -1.0f,
    //     -1.0f, -1.0f,  1.0f,
    //      1.0f, -1.0f, -1.0f,
    //      1.0f, -1.0f, -1.0f,
    //     -1.0f, -1.0f,  1.0f,
    //      1.0f, -1.0f,  1.0f
    // };
    // // cube VAO
    // unsigned int cubeVAO, cubeVBO;
    // glGenVertexArrays(1, &cubeVAO);
    // glGenBuffers(1, &cubeVBO);
    // glBindVertexArray(cubeVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glBindVertexArray(0);
    // // plane VAO
    // unsigned int planeVAO, planeVBO;
    // glGenVertexArrays(1, &planeVAO);
    // glGenBuffers(1, &planeVBO);
    // glBindVertexArray(planeVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glBindVertexArray(0);

    // // load textures
    // // -------------
    // unsigned int cubeTexture  = loadTexture("./textures/advanced/marble.jpg");
    // unsigned int floorTexture = loadTexture("./textures/advanced/metal.png");

    // verticies
    // float quad_verticies[] = {
    //     // positions     // colors
    //     -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
    //     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
    //     -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

    //     -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
    //     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
    //     0.05f,  0.05f,  0.0f, 1.0f, 1.0f		    		
    // };  
    // // setting the translations 2 pass 2 gpu
    // glm::vec2 translations[100];
    // int index = 0;
    // float offset = 0.1f;
    // for(int y = -10; y < 10; y += 2) {
    //     for(int x = -10; x < 10; x += 2) {
    //         glm::vec2 translation;
    //         translation.x = (float)x / 10.0f + offset;
    //         translation.y = (float)y / 10.0f + offset;
    //         translations[index++] = translation;
    //     }
    // }  


    // // vao & vbo set up
    // unsigned int quadVAO, quadVBO;
    // glGenVertexArrays(1, &quadVAO);
    // glGenBuffers(1, &quadVBO);
    // glBindVertexArray(quadVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verticies), &quad_verticies, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    // // setting up instancing vbo  

    // unsigned int instanceVBO;
    // glGenBuffers(1, &instanceVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);	
    // glVertexAttribDivisor(2, 1);  
    // glBindVertexArray(0);



    // sending translations 2 vert shade
    // shader.use();
    // for(unsigned int i = 0; i < 100; i++) {
    //     shader.setVec2(("offsets[" + std::to_string(i) + "]"), translations[i]);
    // }  



    // load cubemap
    // -------------
    // std::vector<std::string> faces = {
    //     "./textures/skyboxes/pretty_lake/right.jpg",
    //     "./textures/skyboxes/pretty_lake/left.jpg",
    //     "./textures/skyboxes/pretty_lake/top.jpg",
    //     "./textures/skyboxes/pretty_lake/bottom.jpg",
    //     "./textures/skyboxes/pretty_lake/front.jpg",
    //     "./textures/skyboxes/pretty_lake/back.jpg"
    // };
    // unsigned int skybox_texture = load_cubemap(faces);   

    // unsigned int skyboxVAO, skyboxVBO;
    // glGenVertexArrays(1, &skyboxVAO);
    // glGenBuffers(1, &skyboxVBO);
    // glBindVertexArray(skyboxVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glBindVertexArray(0);

    // vertex buffer object


    unsigned int amount = 100000;
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed	
    float radius = 200.0;
    float offset = 20.0f;
    for(unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    } 

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    
    for (unsigned int i = 0; i < rock_model.meshes.size(); i++) {
        unsigned int VAO = rock_model.meshes[i].VAO;
        glBindVertexArray(VAO);
        // vertex attributes
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3); 
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4); 
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
        glEnableVertexAttribArray(5); 
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6); 
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        // glVertexAttribDivisor(/)
        glBindVertexArray(0);

        GLenum e = glGetError();
        if (e != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "create rock matricies", e, e);
            exit(20);
        }        
    }  

    // shader configuration
    // --------------------
    space_3d.use();
    // render loop
    // -----------
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

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // we want to set up skybox first
        // render_skybox(skybox_shader, skybox_texture, skyboxVAO);
        // and then draw the rest of the scene on top of it

        space_3d.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getView();
        glm::mat4 projection = camera.getProjection();
        space_3d.setMat4("view", view);
        space_3d.setMat4("projection", projection);


        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        space_3d.setMat4("model", model);
        planet_model.Draw(space_3d);
        
        // draw meteorites
        instanced_space_3d.use();
        instanced_space_3d.setMat4("view", view);
        instanced_space_3d.setMat4("projection", projection);
        // Get the current time from the system clock
        instanced_space_3d.setFloat("time", currentFrame);
        std::cout << "miliseconds in float is " << currentFrame << "." << std::endl;
        for (unsigned int i = 0; i < rock_model.meshes.size(); i++)
        {
            glBindVertexArray(rock_model.meshes[i].VAO);
            glDrawArraysInstanced(
                GL_TRIANGLES, 0, rock_model.meshes[i].vertices.size(), amount
            );
            GLenum e = glGetError();
            if (e != GL_NO_ERROR) {
                fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", "draw elements", e, e);
                exit(20);
            }
        }  

        // glBindVertexArray(quadVAO);
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);  

        // // cubes
        // glBindVertexArray(cubeVAO);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, cubeTexture); 	
        // model = glm::translate(model, glm::vec3(-1.0f, 0.0005f, -1.0f));
        // shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3( 2.0f, 0.0005f, 0.0f));
        // shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // // floor
        // glBindVertexArray(planeVAO);
        // glBindTexture(GL_TEXTURE_2D, floorTexture);
        // shader.setMat4("model", glm::mat4(1.0f));
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &cubeVAO);
    // glDeleteVertexArrays(1, &planeVAO);
    // glDeleteBuffers(1, &cubeVBO);
    // glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
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
unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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

