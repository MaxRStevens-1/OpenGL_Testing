#version 330 core

layout (location = 0) in vec3 aPos; // position has attribute position 0
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 texCoord;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
//out vec4 vertexColor; // specify a color output to fragment shader


void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
    texCoord = aTexCoord;

}