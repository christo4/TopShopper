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
	vec3 color;
	color = texture(imageTexture, uv).rgb;

    // ambient
    vec3 ambient = 3.25 * color;

    vec3 lighting = ambient;    
	FragmentColour = vec4(lighting, 0.7);
	
}
