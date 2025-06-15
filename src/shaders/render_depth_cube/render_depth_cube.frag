#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform samplerCube depthMap;

void main() {
    vec2 mapCoord = 2.0 * TexCoords - 1.0;

    // each of these renders a different face of the cubemap
    // pos x
    // float depthValue = texture(depthMap, vec3(1.0, mapCoord)).r;
    // negative x
    // float depthValue = texture(depthMap, vec3(-1.0, mapCoord)).r;
    // pos y
    // float depthValue = texture(depthMap, vec3(mapCoord.x, 1.0, mapCoord.y)).r;
    // neg y
    float depthValue = texture(depthMap, vec3(mapCoord.x, -1.0, mapCoord.y)).r;
    // pos z
    // float depthValue = texture(depthMap, vec3(mapCoord.xy, 1.0)).r;
    // neg z
    // float depthValue = texture(depthMap, vec3(mapCoord.xy, -1.0)).r;
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}