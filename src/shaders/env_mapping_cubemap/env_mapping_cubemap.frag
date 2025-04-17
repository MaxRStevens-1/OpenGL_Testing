#version 330 core
out vec4 FragColor;

in vec3 out_normal;
in vec3 out_pos;

uniform vec3 camera_pos;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(out_pos - camera_pos);
    vec3 R = reflect(I, normalize(out_normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}