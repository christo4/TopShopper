#version 410
layout (location = 0) in vec4 VertexPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * VertexPosition;
}  