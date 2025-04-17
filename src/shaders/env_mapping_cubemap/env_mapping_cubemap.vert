#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 out_normal;
out vec3 out_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    out_normal = mat3(transpose(inverse(model))) * aNormal;
    out_pos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(out_pos, 1.0);
}  