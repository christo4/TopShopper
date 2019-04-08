#version 410
//largely based on https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping


// interpolated colour received from vertex stage
in vec3 Normal;
in vec3 FragPos;
in vec3 look;
in vec2 uv;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D imageTexture;

void main()
{   

	//vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);
	vec3 lightPos = vec3(70.0f, 200.0f, 0.0f);

	vec3 color;

	color = texture(imageTexture, uv).rgb;
	
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


    // calculate shadow
    vec3 lighting = (ambient + diffuse + specular) * color;    
	FragmentColour = vec4(lighting, 0.3);
	
}
