#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
uniform vec3 viewPos;

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_emission;
    float shininess;
};
// #define NR_MATERIALS 1
// uniform Material materials[NR_MATERIALS];
uniform Material material;
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight spotLight;

out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main() {
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    FragColor = vec4(vec3(depth), 1.0);
}
