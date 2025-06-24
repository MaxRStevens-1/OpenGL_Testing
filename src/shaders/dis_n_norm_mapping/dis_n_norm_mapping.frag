#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;
uniform sampler2D normalMap;  
uniform sampler2D depthTextureMap;


uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool do_normal_map = true;

uniform float far_plane;
uniform float height_scale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) { 
    // number of depth layers
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0)); 
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
    
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthTextureMap, currentTexCoords).r;
    
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthTextureMap, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthTextureMap, prevTexCoords).r - currentLayerDepth + layerDepth;
    
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;  
} 

float ShadowCalculation() {
    // !! NOTE !!
    // you NEED the non TBN stuff to do shadow calc
    vec3 frag_pos = fs_in.FragPos;
    vec3 view_pos = viewPos;
    vec3 light_pos = lightPos;

    vec3 fragToLight = frag_pos - light_pos; 
    // normalized depth vaslue between light source & closet fragment
    float closestDepth = texture(depthMap, fragToLight).r;

    // take closest depth from [0,1] -> [0, far_plane] 
    closestDepth *= far_plane;  
    
    // same (OR LARGER) range as closest dpeth
    float currentDepth = length(fragToLight); 

    // when done, renders depth
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);  

    // bias prevents shadow acne 
    //w/o sampling 
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;  
    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(view_pos - frag_pos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;  
    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);  

    return shadow;
}

void main() {           
    // vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal;
    vec3 frag_pos;
    vec3 light_pos;
    vec3 view_pos;

    // obtain normal from normal map in range [0,1]
    // normal = texture(normalMap, fs_in.TexCoords).rgb;
    // transform normal vector to range [-1,1]

    // use TBN to make sure its guchi
    frag_pos = fs_in.TangentFragPos;
    light_pos = fs_in.TangentLightPos;
    view_pos = fs_in.TangentFragPos;

    vec3 viewDir   = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);
    // normal = texture(normalMap, fs_in.TexCoords).rgb;

    // // lets see if this works?
    // if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    //     discard;
    vec3 color = texture(diffuseTexture, texCoords).xyz;
    normal = texture(normalMap, texCoords).xyz;
    normal = normal * 2.0 - 1.0;   

    // FragColor = texture(normalMap, texCoords);
    // return;

    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(light_pos - frag_pos);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    // vec3 viewDir = normalize(view_pos - frag_pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation();                                            
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}  