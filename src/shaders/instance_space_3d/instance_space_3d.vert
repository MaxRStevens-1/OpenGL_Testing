#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform float time;

void main() {

    float angle = time;
    float c = cos(angle);
    float s = sin(angle);
    
    mat4 rotation = mat4(
        vec4( c,  0.0, s,   0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4( -s, 0.0, c,   0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    mat4 rotated_model = rotation * instanceMatrix;

    gl_Position = projection * view * rotated_model * vec4(aPos, 1.0); 
    TexCoords = aTexCoords;
}