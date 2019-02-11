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

uniform mat4 MVP;

// output to be interpolated between vertices and passed to the fragment stage
out vec3 color;
out float z;

void main()
{
    // assign vertex position without modification
    gl_Position =  MVP * VertexPosition;

    // assign output colour to be interpolated
    color = VertexColor;
	z = VertexPosition[1];
}
