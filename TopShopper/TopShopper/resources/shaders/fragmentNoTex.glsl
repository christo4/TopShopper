#version 410

// interpolated colour received from vertex stage
in vec3 color;
in vec3 Normal;
in vec3 FragPos;
in vec3 look;
in vec2 uv;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;


void main(void)
{
	float specularStrength = 0.7;

	//AMBIENT
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(vec3(0.0f, 30.0f, 0.0f) - FragPos);
	
	//DIFFUSE
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//SPECULAR
	vec3 viewDir = normalize(look - FragPos);
	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = specularStrength * spec * lightColor;  
	
	vec4 result = vec4((ambient + diffuse + specular), 1.0f) * vec4(color, 1.0f);
	FragmentColour = result;
}
