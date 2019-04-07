#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec4 VertexPosition;
layout(location = 2) in vec3 VertexNormal;
layout(location = 1) in vec2 VertexUV;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 LightView;
uniform mat4 LightProjection;
uniform vec3 CameraPos;
uniform float Flash;

// output to be interpolated between vertices and passed to the fragment stage
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec3 look;
out vec2 uv;
out float flash;


void main()
{

    gl_Position =  Projection* View * Model * VertexPosition;

	//Normal = mat3(transpose(inverse(Model))) * VertexNormal;
	Normal = VertexNormal;
	FragPos = vec3(Model * VertexPosition);
	FragPosLightSpace = LightProjection * LightView * vec4(FragPos, 1.0);
	look = CameraPos;
	uv = VertexUV;
	flash = Flash;
}
