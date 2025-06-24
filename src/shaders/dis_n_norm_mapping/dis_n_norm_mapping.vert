
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
    // correct normal transform as learned in previous tutorials here

    mat3 normalMatrix = transpose(inverse(mat3(model))); 

    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  