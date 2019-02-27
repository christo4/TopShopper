// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// interpolated colour received from vertex stage
in vec3 color;
in vec3 Normal;
in vec3 FragPos;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

uniform sampler2D imageTexture;

void main(void)
{

	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	float ambientStrength = 0.9;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(vec3(0.0f, 30.0f, 0.0f) - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	vec3 result = (ambient + diffuse) * color;

	FragmentColour = vec4(result, 1.0);
    //FragmentColour = texture(imageTexture, uv);
}
