#version 410
out vec4 FragColor;
  
in vec2 uv;

uniform sampler2D depthMap;

void main()
{             
    float depthValue = texture(depthMap, uv).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}  