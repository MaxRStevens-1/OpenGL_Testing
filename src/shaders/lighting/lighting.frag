#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shiniess;
}
uniform Material material;

float ambientStrength = 0.1;
float specularStrength = 0.5;

int shininessFactor = 32;

out vec4 FragColor;

void main()
{
    //ambient
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessFactor);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
}