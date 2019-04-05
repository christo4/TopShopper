#version 410
//largely based on https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping


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

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;    // perspective divide
    projCoords = projCoords * 0.5 + 0.5;                              // transform to [0,1] range
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;


   // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
    return shadow;
}


void main()
{   

	//vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);
	vec3 lightPos = vec3(70.0f, 200.0f, 0.0f);


    vec3 color = texture(imageTexture, uv).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);

    // ambient
    vec3 ambient = 0.25 * color;

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


	float bias = max(0.010 * (1.0 - dot(Normal, lightDir)), 0.005); 

    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace, bias);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragmentColour = vec4(lighting, 1.0);
}
