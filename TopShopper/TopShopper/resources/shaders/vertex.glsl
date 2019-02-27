// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec3 VertexColor;
layout(location = 2) in vec3 VertexNormal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 ColorMeme;


// output to be interpolated between vertices and passed to the fragment stage
out vec3 color;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    // assign vertex position without modification
    gl_Position =  Projection* View * Model * VertexPosition;

    // assign output colour to be interpolated
    color = ColorMeme;
	//Normal = vec3(Model * vec4(VertexNormal, 1.0f));
	Normal = VertexNormal;
	FragPos = vec3(Model * VertexPosition);
}
