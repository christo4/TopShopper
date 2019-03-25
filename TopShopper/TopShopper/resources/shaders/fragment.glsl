#version 410

// interpolated colour received from vertex stage
in vec3 Normal;
in vec3 FragPos;
in vec3 look;
in vec2 uv;
in vec4 FragPosLightSpace;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D imageTexture;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;    // perform perspective divide
    projCoords = projCoords * 0.5 + 0.5;                              // transform to [0,1] range


    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

	float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;

}

void main()
{   

	vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);

    vec3 color = texture(imageTexture, uv).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);

    // ambient
    vec3 ambient = 0.15 * color;

    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(look - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    

    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragmentColour = vec4(lighting, 1.0);
}
