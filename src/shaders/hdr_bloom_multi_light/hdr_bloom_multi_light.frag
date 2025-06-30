#version 330 core
out vec4 FragColor;

#define NR_LIGHTS 3

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3[NR_LIGHTS] TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap_1;
uniform samplerCube depthMap_2;
uniform samplerCube depthMap_3;
uniform sampler2D normalMap;  

struct PointLight {
    vec3 position;
    
    vec3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight lights[NR_LIGHTS];

uniform vec3 viewPos;

uniform bool do_normal_map = true;

uniform float far_plane;

vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float shadowSample(samplerCube current_depth_map, vec3 fragToLight) {
    float shadow = 0.0;
    float closestDepth = texture(current_depth_map, fragToLight).r;
    vec3 frag_pos = fs_in.FragPos;
    vec3 view_pos = viewPos;
    // take closest depth from [0,1] -> [0, far_plane] 
    closestDepth *= far_plane;  
    
    // same (OR LARGER) range as closest dpeth
    float currentDepth = length(fragToLight); 

    // when done, renders depth
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);  

    // bias prevents shadow acne 
    //w/o sampling 
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(view_pos - frag_pos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;  
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(current_depth_map, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow;
}

// lets do this with MULTIPLE LIGHTS
float ShadowCalculation(int light_index) {
    // !! NOTE !!
    // you NEED the non TBN stuff to do shadow calc
    float shadow = 0.0;
    int samples  = 20;

    // for (int i = 0; i < NR_LIGHTS; i++) {

    vec3 light_pos = lights[light_index].position;
    vec3 frag_pos = fs_in.FragPos;

    vec3 fragToLight = frag_pos - light_pos; 

    if (light_index == 0) {
        shadow += shadowSample(depthMap_1, fragToLight) / float(samples);
    } else if (light_index == 1) {
        shadow += shadowSample(depthMap_2, fragToLight) / float(samples);
    } else if (light_index == 2) {
        shadow += shadowSample(depthMap_3, fragToLight) / float(samples);
    }
    // }


    return shadow;
}

void main() {
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normal * 2.0 - 1.0;       
    vec3 frag_pos  = fs_in.TangentFragPos;
    // vec3 light_pos;
    vec3 view_pos = fs_in.TangentViewPos;
    
    vec3 light_contribution = vec3(0.0);

    for (int i = 0; i < NR_LIGHTS; i++) {   
        PointLight light = lights[i];
        vec3 light_pos = fs_in.TangentLightPos[i];
        float distance = length(light_pos - frag_pos);
        // diffuse
        vec3 lightDir = normalize(light_pos - frag_pos);

        // attenuation
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
            light.quadratic * (distance * distance)); 

        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * light.color;
        // specular
        vec3 viewDir = normalize(view_pos - frag_pos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = 0.0;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        spec = clamp(spec, 0.0, 1.0);

        vec3 specular = spec * light.color;

        // ambient *= attenuation;
        diffuse *= attenuation * light.intensity;
        specular *= attenuation * light.intensity;

        float shadow = ShadowCalculation(i);                                      
        light_contribution += (1.0 - shadow) * (diffuse + specular);    
    }
    vec3 ambient_floor = vec3(0.2);
    vec3 lighting = ambient_floor * color;
    lighting += light_contribution * color;
    
    FragColor = vec4(lighting, 1.0);
}  