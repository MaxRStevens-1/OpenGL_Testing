
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;  

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal; // technically w/ tbh this isn't needed....
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool reverse_normals;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 normal_to_use = aNormal;
    if (reverse_normals) {
        normal_to_use *= -1;
    }
    
    vs_out.Normal = transpose(inverse(mat3(model))) * normal_to_use;    


    vs_out.TexCoords = aTexCoords;
    
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal_to_use, 0.0)));

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  