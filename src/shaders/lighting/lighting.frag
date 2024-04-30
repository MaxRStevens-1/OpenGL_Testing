#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
// uniform vec3 objectColor;
uniform vec3 viewPos;

// struct Material {
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
//     float shininess;
// };

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

uniform Material material;

struct Light {
    vec3 position; // no longer necessary when using directional lights.
    // vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform Light light;

out vec4 FragColor;

void main() {

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // ambient
    // vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    // vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    // specular

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
    material.shininess);

    // what I want to do is take the emission, and if not black set color to texture color
    vec3 emission = vec3(texture(material.emission, TexCoords));
    
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // atttenuation calc
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular; // + emission;

    FragColor = vec4(result, 1.0);
}
