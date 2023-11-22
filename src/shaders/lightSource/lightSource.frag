#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;

void main() {
    // set so object color varies with lightsource color.
    FragColor = vec4(lightColor, 1); // set all 4 vector values to 1.0
}