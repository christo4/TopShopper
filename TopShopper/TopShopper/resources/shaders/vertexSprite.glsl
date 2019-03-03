
#version 410

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec2 VertexUV;


out vec2 uv;

uniform mat4 projection;

void main()
{

    gl_Position = projection * VertexPosition;

    uv = VertexUV;

}
